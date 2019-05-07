#include "AudioInput.h"

AudioInput::AudioInput(QObject *parent) : QObject(parent) {
    sampleRate = 0;
    winSize    = 8192;
    hopSize    = winSize / 32;

}

void AudioInput::run() {
    this->readyRead();

    //TODO: invoke fft and tempo processing;
}

AudioInput::~AudioInput() {

}

AubioReader::AubioReader() : AudioInput() {
    aubio_source_t * source = new_aubio_source(sourcePath, sampleRate, hopSize);
    if (!source) {
         aubio_cleanup();
         throw std::runtime_error("aubio source not initialize check source Path");
    }

    if ( sampleRate == 0 ) {
         sampleRate = aubio_source_get_samplerate(source);
    }
}

AubioReader::~AubioReader() {
    del_aubio_source(source);
    aubio_cleanup();
}

void AubioReader::readyRead() {


}


void AubioReader::fvec_copy_to_end(const fvec_t * src, fvec_t * dist, const uint_t ind_beg_dist) {

    if(src->length + ind_beg_dist > dist->length) {
        printf("trying to copy %d elements to %d elements \n",
            src->length + ind_beg_dist, dist->length);
        return;
    }

    uint_t i;
    for(i = 0; i < src->length; ++i) {

        dist->data[i + ind_beg_dist] = src->data[i];
    }
}

void AubioReader::fvec_copy_to_start(const fvec_t * src, const uint_t ind_beg_src, fvec_t * dist) {

    if(src->length != dist->length) {
        printf("length src %d != length dist %d \n",
            src->length , dist->length);
        return;
    }

    uint_t i;
    uint_t size = dist->length - ind_beg_src;
    for(i = 0; i < size; ++i) {

        dist->data[i] = src->data[i + ind_beg_src];
    }
}
