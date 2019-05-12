#ifndef I_AUDIO_INPUT_H
#define I_AUDIO_INPUT_H

#include <QObject>
#include <QAudioInput>
#include <aubio/aubio.h>

#include "AudioFFT.h"
#include "AudioTempo.h"

class AudioInput : public QObject {

    Q_OBJECT

public:
    explicit AudioInput(QObject *parent = nullptr);
    virtual ~AudioInput() = 0;

    virtual void stop()  = 0;
    virtual void start() = 0;

protected:

    uint_t winSize;
    uint_t hopSize;

private:



public slots:

};

class QtReader : public  AudioInput {

public:
    QtReader();
    virtual ~QtReader();

    void stop();
    void start();

protected:

private:
    QAudioInput * audio;

signals:

public slots:

};

class AubioReader : public  AudioInput {

public:
    AubioReader();
    virtual ~AubioReader();

protected:

private:
    void fvec_copy_to_start(const fvec_t * src,
                            const uint_t ind_beg_src,
                                  fvec_t * dist);

    void fvec_copy_to_end(const fvec_t * src,
                                fvec_t * dist,
                          const uint_t ind_beg_dist);

    char_t * sourcePath;
    aubio_source_t * source;

signals:

public slots:

};


#endif // I_AUDIO_INPUT_H
