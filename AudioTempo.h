#ifndef AUDIOTOOLS_H
#define AUDIOTOOLS_H

#include <aubio/aubio.h>

class TempoDetecting {
public:
    virtual ~TempoDetecting() = 0;
    virtual void detecting( const fvec_t * in ) = 0;

    virtual smpl_t getTempo() = 0;
};

class AubioTempo final : public TempoDetecting {
public:
    AubioTempo() = delete;
    AubioTempo(const uint_t winSize,
               const uint_t hopSize,
               const uint_t sampleRate);
    AubioTempo(const AubioTempo & _TempoDetecting) = delete;
    AubioTempo & operator=(const AubioTempo & rhs) = delete;
    ~AubioTempo();

    void  detecting( const fvec_t * in );

    smpl_t getTempo();

private:
    aubio_tempo_t * tempo;
    fvec_t        * out;

    smpl_t          previousTempo;
};


#endif // AUDIOTOOLS_H
