#ifndef AUDIOFFT_H
#define AUDIOFFT_H

#include <aubio/aubio.h>
#include <stdexcept>
#include <memory>


struct Sample;

class AudioFFT
{
public:
    virtual ~AudioFFT() = 0;

    virtual void     fftDo(const std::shared_ptr< Sample > in) = 0;

    virtual smpl_t * getNorm()  const = 0;
    virtual smpl_t * getPhase() const = 0;
    virtual uint_t getSizeFFT() const = 0;
};

class AubioFFT final : public AudioFFT {
public:
    AubioFFT() = delete;
    AubioFFT(const uint_t _winSize);
    AubioFFT(const AubioFFT * _FFTAubio) = delete;
    AubioFFT & operator=(const AubioFFT & rhs) = delete;
    ~AubioFFT();

    void fftDo(const std::shared_ptr< Sample > in);

    smpl_t * getNorm()  const;
    smpl_t * getPhase() const;

    uint_t getSizeFFT() const;

private:
    uint_t        winSize;

    aubio_fft_t * fft;
    cvec_t      * fftGrain;
    fvec_t      * fftIn;
};


#endif // AUDIOFFT_H
