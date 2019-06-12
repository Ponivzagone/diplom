#include "AudioInput.h"
#include <QDebug>
#include <settings/config_reader.h>
#include "AudioDevice.h"
#include <thread>

#include <cstring>



void Sample::setEmpthy()
{
    for(quint32 i = winSize - (getHopSize() - fillSize); i < winSize; i++)
    {
        amplitude[i] = 0.0f;
    }
}

std::shared_ptr<fvec_t> Sample::convertAubio()
{
    std::shared_ptr<fvec_t> tmp((fvec_t *)calloc(sizeof(fvec_t),1), [](fvec_t * tmp){
                                                                free(tmp);
                                                            });
    tmp->data =  this->amplitude;
    tmp->length = winSize;
    return tmp;
}

void Sample::convertAubio(fvec_t * in)
{
    if(in->length != winSize) {
        throw std::runtime_error("convert winSize != input signal length");
    }
    in->data = this->amplitude;
}

void Sample::convertAubioHop(fvec_t * in)
{
    if(getHopSize() != in->length)
    {
        throw std::runtime_error("convert hopSize != input signal length");
    }
    int offset = (winSize - getHopSize());
    in->data = this->amplitude + offset;
}

uint Sample::getHopSize()
{
    return winSize / DIVIDER;
}


AudioInput::AudioInput(QObject * parent)
    : QObject(parent), winSize(0), hopSize(0), sampleRate(0), status(0)
{

    connect(this, &AudioInput::recFinished, this, &AudioInput::stopRecord);
    connect(&ConfigReader::instance(), &ConfigReader::configChanged, this, &AudioInput::configHandler);

    winSize = ConfigReader::instance().getValue<uint>(CoreSettings::frame_size, 8192);
    bitRate = ConfigReader::instance().getValue<uint>(CoreSettings::bit_rate, 16);
    hopSize = winSize / DIVIDER;
    sampleRate = ConfigReader::instance().getValue<uint>(CoreSettings::sample_rate, 8000);
    sourcePath = ConfigReader::instance().getValue<QString>(CoreSettings::source_path, "/srv/download/gamma.wav");
}

void AudioInput::checkAndCopySample(std::shared_ptr<Sample> block, std::shared_ptr<float> sample, unsigned int size)
{
    uint freePos = block->winSize - block->getHopSize() + block->fillSize;

    if( (size + block->fillSize) <= block->getHopSize() ) {
        std::memcpy( (block->amplitude + freePos), (sample.get()) , sizeof(float) * size);
        block->fillSize += size;        
    } else {

        uint newSizeSample = block->getHopSize() - block->fillSize;
        std::memcpy(block->amplitude + freePos, (sample.get()), sizeof(float) * newSizeSample);
        block->fillSize += newSizeSample;

        uint cutSampleSize = size - newSizeSample;
        std::shared_ptr<float> cutSample(new float[cutSampleSize], [](float *p) {
             delete[] p;
        });
        std::memcpy(cutSample.get(), (sample.get() + newSizeSample), sizeof(float) * cutSampleSize);
        this->setSamples(cutSample, cutSampleSize);
    }
}

void AudioInput::setSamples(std::shared_ptr<float> sample, unsigned int sizeSample)
{
    std::shared_ptr< Sample > prevBlock = sampleBuffer->back();

    if (prevBlock->fillSize >= prevBlock->getHopSize()) {
        std::shared_ptr< Sample > newBlock( new Sample(winSize));

        std::memcpy(newBlock->amplitude, prevBlock->amplitude, sizeof(float) * prevBlock->winSize);

        std::rotate(newBlock->amplitude, newBlock->amplitude + hopSize, newBlock->amplitude + winSize);
        newBlock->fillSize = 0;
        sampleBuffer->push_back(newBlock);
        checkAndCopySample(newBlock, sample, sizeSample);
    } else {
        checkAndCopySample(prevBlock, sample, sizeSample);
    }
}

void AudioInput::initSampleBuffer()
{
    sampleBuffer = new std::list< std::shared_ptr< Sample > >;
    sampleBuffer->push_back(std::shared_ptr< Sample >(new Sample(winSize)));
}



bool AudioInput::isRecord()
{
    return status & (1 << statusByte::record);
}

void AudioInput::changeRecordStatus()
{
    if(isRecord()) {
        status &= ~(1 << statusByte::record);
    } else {
        status |=  (1 << statusByte::record);
    }
}



void AudioInput::stopRecord()
{

    std::cout << "stopRecord" << std::endl;
    this->stop();
}

void AudioInput::startRecord()
{
    std::cout << "startRecord" << std::endl;
    this->start();
}

#include <fstream>


void AudioInput::startAlgo()
{
    algo.reset(new AlgorithManager(winSize, hopSize, sampleRate));
    if(sampleBuffer)
    {

        std::fstream ff;
        ff.open("/srv/dev/jopa.ly", std::fstream::in | std::fstream::out |std::fstream::trunc);
        ff << algo->algLoop(*sampleBuffer);
        ff.close();

        system("lilypond /srv/dev/jopa.ly");

        delete sampleBuffer;
        sampleBuffer = nullptr;
    }
    emit finishAlgo();
}

void AudioInput::configHandler(uint key)
{
    QString name;
    if(Settings::keyToString(static_cast<CoreSettings>(key), name) && !isRecord())
    {
        if(name == "sample_rate") {
            sampleRate = ConfigReader::instance().getValue<uint>(CoreSettings::sample_rate);
        } else if (name == "frame_size") {
            winSize = ConfigReader::instance().getValue<uint>(CoreSettings::frame_size);
            hopSize = winSize / DIVIDER;
        } else if (name == "bit_rate") {
            bitRate = ConfigReader::instance().getValue<uint>(CoreSettings::bit_rate);
        } else if (name == "source_path") {
            sourcePath = ConfigReader::instance().getValue<QString>(CoreSettings::source_path);
        }
    }
}

AudioInput::~AudioInput()
{
    aubio_cleanup();
}




QtReader::QtReader()
    : AudioInput()
{
}

void QtReader::initializeAudio(const QAudioDeviceInfo & deviceInfo)
{
    QAudioFormat format;

    format.setSampleRate(static_cast<int>(sampleRate));
    format.setChannelCount(1);
    format.setSampleSize(static_cast<int>(bitRate));
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");

    if (!deviceInfo.isFormatSupported(format)) {
        qDebug() << "Default format not supported, trying to use the nearest.";
        format = deviceInfo.nearestFormat(format);
        ConfigReader::instance().setValue(CoreSettings::bit_rate, format.sampleSize());
        ConfigReader::instance().setValue(CoreSettings::sample_rate, format.sampleRate());
    }

    audioDevice.reset(new QtAudioDevice(format, this));
    audioInput.reset(new QAudioInput(deviceInfo, format));
}

void QtReader::start()
{
    if(isRecord()) { return; }

    initializeAudio(QAudioDeviceInfo::defaultInputDevice());
    this->initSampleBuffer();
    connect(audioDevice.get(), &QtAudioDevice::readChannelFinished, this, &AudioInput::startAlgo );
    audioDevice->start();
    changeRecordStatus();
    audioInput->start(audioDevice.data());
}

void QtReader::stop()
{
    if(!isRecord()) { return; }

    audioDevice->stop();
    changeRecordStatus();
    audioInput->stop();
}

QtReader::~QtReader()
{

}





AubioReader::AubioReader()
    : AudioInput()
{
}

void AubioReader::start()
{
    if(isRecord()) { return; }

    this->initSampleBuffer();
    source = new_aubio_source(sourcePath.toStdString().c_str(), 0, hopSize);
    if (!source) {
        return;
    }
    ConfigReader::instance().setValue(CoreSettings::sample_rate, aubio_source_get_samplerate(source));

    audioDevice = new AubioDevice(this);
    connect(audioDevice, &AubioDevice::finished, this, &AudioInput::startAlgo );
    connect(audioDevice, &AubioDevice::finished, audioDevice ,&AubioDevice::deleteLater);

    changeRecordStatus();
    audioDevice->start();
}

void AubioReader::stop()
{
    if(!isRecord()) { return; }

    record_mutex.lock();
    std::cout << " Wait start " << std::endl;
    if(!done) { record_cond.wait(&record_mutex); }
    if(isRecord()){ changeRecordStatus(); }
    if(!done) { record_cond.wait(&record_mutex); }
    del_aubio_source(source);
    source = nullptr;
    std::cout << " Wait stop " << std::endl;
    record_mutex.unlock();

}

AubioReader::~AubioReader()
{

}
