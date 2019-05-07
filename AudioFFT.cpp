#include "AudioFFT.h"

AudioFFT::~AudioFFT() {

}


AubioFFT::AubioFFT( const uint_t _winSize ) {
    winSize = _winSize;
    fft = new_aubio_fft(_winSize);
    fftGrain = new_cvec(_winSize);
}

AubioFFT::~AubioFFT() {
    del_aubio_fft(fft);
    del_cvec(fftGrain);
}

void AubioFFT::fftDo( const fvec_t * in ) {
    if(in->length != winSize) {
        throw std::runtime_error("aubio FFT winSize != input signal length");
    }
    aubio_fft_do(fft, in, fftGrain);
}

smpl_t * AubioFFT::getNorm() const {
    return fftGrain->norm;
}

smpl_t * AubioFFT::getPhase() const {
    return fftGrain->phas;
}
