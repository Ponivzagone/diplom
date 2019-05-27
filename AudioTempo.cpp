#include "AudioTempo.h"

#include "AudioInput.h"


TempoDetecting::~TempoDetecting() {

}

AubioTempo::AubioTempo(const uint_t winSize,
                       const uint_t hopSize,
                       const uint_t sampleRate) {
    out = new_fvec(1);
    tempo = new_aubio_tempo("default", winSize, hopSize, sampleRate);

    fftIn    = new_fvec(hopSize);
    if(fftIn->data) { free(fftIn->data); }
}


void AubioTempo::detecting(const std::shared_ptr< Sample > in) {

    for(uint i = in->uniqSampleHop; i >= 1; --i)
    {
        in->convertAubioHop(fftIn, i);
        aubio_tempo_do(tempo, fftIn, out);
        if (out->data[0] != 0.0f) {
            previousTempo = aubio_tempo_get_bpm(tempo);
        }
    }
}

AubioTempo::~AubioTempo() {
    del_fvec(out);
    del_aubio_tempo(tempo);
}

smpl_t AubioTempo::getTempo() {
    return previousTempo;
}
