#include "AudioTempo.h"


TempoDetecting::~TempoDetecting() {

}

AubioTempo::AubioTempo(const uint_t winSize,
                       const uint_t hopSize,
                       const uint_t sampleRate) {
    out = new_fvec(1);
    tempo = new_aubio_tempo("default", winSize, hopSize, sampleRate);
}


void AubioTempo::detecting(const fvec_t * in) {
    aubio_tempo_do(tempo, in, out);
    if (out->data[0] != 0.0f) {
        previousTempo= out->data[0];
    }
}

AubioTempo::~AubioTempo() {
    del_fvec(out);
    del_aubio_tempo(tempo);
}

smpl_t AubioTempo::getTempo() {
    return previousTempo;
}
