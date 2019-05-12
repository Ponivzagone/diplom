#include "AudioInput.h"
#include <QDebug>
#include <settings/config_reader.h>


AudioInput::AudioInput(QObject *parent) : QObject(parent) {
    winSize    = ConfigReader::instance().getValue<uint>(CoreSettings::frame_size, 8192);
    hopSize    = winSize / 32;
}

QtReader::QtReader()
{
    QAudioFormat format;

    format.setSampleRate(static_cast<int>(ConfigReader::instance().getValue<uint>(CoreSettings::sample_rate, 8000)));
    format.setChannelCount(1);
    format.setSampleSize(static_cast<int>(ConfigReader::instance().getValue<uint>(CoreSettings::bit_rate, 8)));
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        qDebug() << "Default format not supported, trying to use the nearest.";
        format = info.nearestFormat(format);
    }

    audio = new QAudioInput(format, this);
}

QtReader::~QtReader()
{

}

void QtReader::stop()
{
    audio->stop();
}

void QtReader::start()
{
    audio->start();
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


