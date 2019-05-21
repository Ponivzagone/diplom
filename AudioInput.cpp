#include "AudioInput.h"
#include <QDebug>
#include <settings/config_reader.h>
#include "AudioDevice.h"


AudioInput::AudioInput(QObject *parent)
    : QObject(parent) {
    winSize    = ConfigReader::instance().getValue<uint>(CoreSettings::frame_size, 8192);
    hopSize    = winSize / 32;
    sampleBuffer = new std::list< std::shared_ptr< Sample > >;

    algo.reset(new AlgorithManager(winSize, hopSize));

}



void AudioInput::checkAndCopySample(std::shared_ptr<Sample> block, std::shared_ptr<quint32> sample, unsigned int size)
{
    if( (size + block->fillSize) <= block->winSize ) {
        memcpy(block->amplitude + block->fillSize, sample.get() , sizeof(quint32) * size);
        block->fillSize += size;
    } else {

        quint32 newSizeSample = winSize - block->fillSize;
        memcpy(block->amplitude + block->fillSize, sample.get(), sizeof(quint32) * newSizeSample);
        block->fillSize += newSizeSample;

        quint32 cutSampleSize = size - newSizeSample;
        std::shared_ptr<quint32> cutSample(new quint32[cutSampleSize], [](quint32 *p) {
             qDebug() << "cut~Shared" << endl;
             delete[] p;
        });
        memcpy(cutSample.get(), sample.get() + newSizeSample, sizeof(quint32) * cutSampleSize);
        this->setSamples(cutSample, cutSampleSize);
    }
}

void AudioInput::setSamples(std::shared_ptr<quint32> sample, unsigned int sizeSample)
{
    std::shared_ptr< Sample > prevBlock = sampleBuffer->back();
    if (prevBlock->fillSize >= prevBlock->winSize) {
        std::shared_ptr< Sample > newBlock( new Sample(winSize));

        std::rotate(newBlock->amplitude,
                    prevBlock->amplitude + hopSize,
                    prevBlock->amplitude + winSize);
        newBlock->fillSize = newBlock->winSize - hopSize;
        checkAndCopySample(newBlock, sample, sizeSample);
        sampleBuffer->push_back(newBlock);
    } else {
        checkAndCopySample(prevBlock, sample, sizeSample);
    }
}

void AudioInput::initSampleBuffer()
{
    sampleBuffer->push_back(std::shared_ptr< Sample >(new Sample(winSize)));
}



void AudioInput::delList()
{
    this->stop();

    qDebug() << "delList" << sampleBuffer->size();
    delete sampleBuffer;
    emit finish();
}

uint_t AudioInput::getWinSize() const
{
    return winSize;
}

uint_t AudioInput::getHopSize() const
{
    return hopSize;
}

AudioInput::~AudioInput()
{

}




QtReader::QtReader()
    : AudioInput()
{
    initializeAudio(QAudioDeviceInfo::defaultInputDevice());
}

void QtReader::initializeAudio(const QAudioDeviceInfo & deviceInfo)
{
    QAudioFormat format;

    format.setSampleRate(static_cast<int>(ConfigReader::instance().getValue<uint>(CoreSettings::sample_rate, 8000)));
    format.setChannelCount(1);
    format.setSampleSize(static_cast<int>(ConfigReader::instance().getValue<uint>(CoreSettings::bit_rate, 16)));
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");

    if (!deviceInfo.isFormatSupported(format)) {
        qDebug() << "Default format not supported, trying to use the nearest.";
        format = deviceInfo.nearestFormat(format);
    }

    audioDevice.reset(new QtAudioDevice(format, this));

    audioInput.reset(new QAudioInput(deviceInfo, format));

    audioDevice->start();


}

void QtReader::start()
{
    this->initSampleBuffer();
    audioInput->start(audioDevice.data());
}

void QtReader::stop()
{
    audioDevice->stop();
    audioInput->stop();
    algo->algLoop(*sampleBuffer);


}

QtReader::~QtReader()
{
    audioDevice->stop();
    audioInput->stop();
}





AubioReader::AubioReader() : AudioInput() {

    sampleRate = ConfigReader::instance().getValue<uint>(CoreSettings::sample_rate, 0);
    //sourcePath = ConfigReader::instance().getValue<

    aubio_source_t * source = new_aubio_source(sourcePath, sampleRate, hopSize);
    if (!source) {
         aubio_cleanup();
         throw std::runtime_error("aubio source not initialize check source Path");
    }
    if ( sampleRate == 0 ) {
         sampleRate = aubio_source_get_samplerate(source);
    }
}

void AubioReader::stop()
{

}

void AubioReader::start()
{

}

AubioReader::~AubioReader() {
    del_aubio_source(source);
    aubio_cleanup();
}
