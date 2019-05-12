#include "AudioInput.h"
#include <QDebug>
#include <settings/config_reader.h>
#include "AudioDevice.h"


AudioInput::AudioInput(QObject *parent)
    : QObject(parent), fillSize(0) {
    winSize    = ConfigReader::instance().getValue<uint>(CoreSettings::frame_size, 8192);
    hopSize    = winSize / 32;
    sampleBuffer = new std::list<std::shared_ptr<quint32> >;

}

void AudioInput::startAlgo()
{

}

void AudioInput::checkAndCopySample(std::shared_ptr<quint32> block, std::shared_ptr<quint32> sample, unsigned int size) {
    if( (size + fillSize) <= winSize ) {
        memcpy(block.get() + fillSize, sample.get(), sizeof(quint32) * size);
        fillSize += size;
    } else {

        unsigned int newSizeSample = winSize - fillSize;
        memcpy(block.get() + fillSize, sample.get(), sizeof(quint32) * newSizeSample);
        fillSize += newSizeSample;

        unsigned int cutSampleSize = size - newSizeSample;
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

    if (fillSize >= winSize) {
        fillSize = 0;
        std::shared_ptr<quint32> newBlock(new quint32[winSize], [](quint32 *p) {
                                     qDebug() << "list~Shared" << endl;
                                     delete[] p;
                                 });

        std::shared_ptr<quint32> prevBlock = sampleBuffer->back();

        std::rotate(newBlock.get(),
                    prevBlock.get() + hopSize,
                    prevBlock.get() + winSize);
        fillSize = winSize - hopSize;
        checkAndCopySample(newBlock, sample, sizeSample);
        sampleBuffer->push_back(newBlock);
    } else {

        std::shared_ptr<quint32> prevBlock = sampleBuffer->back();
        checkAndCopySample(prevBlock, sample, sizeSample);
    }
}

void AudioInput::initSampleBuffer()
{
    sampleBuffer->push_back(std::shared_ptr<quint32>(new quint32[winSize], [](quint32 *p) {
                                delete[] p;
                            }));
    fillSize = 0;
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
}

QtReader::~QtReader()
{
    audioDevice->stop();
    audioInput->stop();
}


//void AudioInput::run() {
//    this->readyRead();
//    //TODO: invoke fft and tempo processing;
//}

//AudioInput::~AudioInput() {
//}

//AubioReader::AubioReader() : AudioInput() {
//    aubio_source_t * source = new_aubio_source(sourcePath, sampleRate, hopSize);
//    if (!source) {
//         aubio_cleanup();
//         throw std::runtime_error("aubio source not initialize check source Path");
//    }
//    if ( sampleRate == 0 ) {
//         sampleRate = aubio_source_get_samplerate(source);
//    }
//}

//AubioReader::~AubioReader() {
//    del_aubio_source(source);
//    aubio_cleanup();
//}

//void AubioReader::readyRead() {
//}


//void AubioReader::fvec_copy_to_end(const fvec_t * src, fvec_t * dist, const uint_t ind_beg_dist) {
//    if(src->length + ind_beg_dist > dist->length) {
//        printf("trying to copy %d elements to %d elements \n",
//            src->length + ind_beg_dist, dist->length);
//        return;
//    }
//    uint_t i;
//    for(i = 0; i < src->length; ++i) {
//        dist->data[i + ind_beg_dist] = src->data[i];
//    }
//}

//void AubioReader::fvec_copy_to_start(const fvec_t * src, const uint_t ind_beg_src, fvec_t * dist) {
//    if(src->length != dist->length) {
//        printf("length src %d != length dist %d \n",
//            src->length , dist->length);
//        return;
//    }
//    uint_t i;
//    uint_t size = dist->length - ind_beg_src;
//    for(i = 0; i < size; ++i) {
//        dist->data[i] = src->data[i + ind_beg_src];
//    }
//}


