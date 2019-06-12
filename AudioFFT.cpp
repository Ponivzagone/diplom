#include "AudioFFT.h"

#include "AudioInput.h"

#include <iostream>

AudioFFT::~AudioFFT() {

}


AubioFFT::AubioFFT( const uint_t _winSize ) {
    winSize  = _winSize;
    fft      = new_aubio_fft(_winSize);
    fftGrain = new_cvec(_winSize);


    fftIn    = new_fvec(_winSize);
    if(fftIn->data) { free(fftIn->data); }
}

AubioFFT::~AubioFFT() {
    del_aubio_fft(fft);
    del_cvec(fftGrain);

    fftIn->data = nullptr;
    fftIn->data = (smpl_t *)calloc(sizeof(smpl_t),1);
    del_fvec(fftIn);
}

void AubioFFT::fftDo(const std::shared_ptr< Sample > in ) {
    if(in->winSize != winSize) {
        throw std::runtime_error("aubio FFT winSize != input signal length");
    }
    in->convertAubio(fftIn);
    aubio_fft_do(fft, fftIn, fftGrain);
}

smpl_t * AubioFFT::getNorm() const {
    return fftGrain->norm;
}

smpl_t * AubioFFT::getPhase() const {
    return fftGrain->phas;
}

uint_t AubioFFT::getSizeFFT() const
{
    return winSize;
}
