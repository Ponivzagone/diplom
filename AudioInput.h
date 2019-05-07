#ifndef I_AUDIO_INPUT_H
#define I_AUDIO_INPUT_H

#include <QObject>
#include <aubio/aubio.h>

#include "AudioFFT.h"
#include "AudioTempo.h"

class AudioInput : public QObject {

    Q_OBJECT

public:
    explicit AudioInput(QObject *parent = nullptr);
    virtual ~AudioInput() = 0;
    void run();

protected:

    virtual void readyRead() = 0;

    uint_t sampleRate;
    uint_t winSize;
    uint_t hopSize;

private:

    AudioFFT       * FFTAlgo;
    TempoDetecting * TempoDetectAlgo;

signals:
    void updateWidget();

public slots:

};


class AubioReader : public AudioInput {

public:
    AubioReader();
    ~AubioReader();

protected:
    void readyRead();

private:

    void fvec_copy_to_start(const fvec_t * src,
                            const uint_t ind_beg_src,
                                  fvec_t * dist);

    void fvec_copy_to_end(const fvec_t * src,
                                fvec_t * dist,
                          const uint_t ind_beg_dist);



    char_t * sourcePath;
    aubio_source_t * source;

};


#endif // I_AUDIO_INPUT_H
