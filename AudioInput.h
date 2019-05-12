#ifndef I_AUDIO_INPUT_H
#define I_AUDIO_INPUT_H

#include <QObject>
#include <QAudioInput>
#include <aubio/aubio.h>
#include <QScopedPointer>
#include <memory>

#include "AlgorithManager.h"

class QtAudioDevice;

class AudioInput : public QObject {

    Q_OBJECT

public:
    explicit AudioInput(QObject *parent = nullptr);
    virtual ~AudioInput() = 0;

    virtual void stop()  = 0;
    virtual void start() = 0;

    void startAlgo();

    uint_t getWinSize() const;
    uint_t getHopSize() const;

    void setSamples(std::shared_ptr<quint32> samples, unsigned int sizeSample);

protected:

    void initSampleBuffer();

    QScopedPointer<AlgorithManager> algo;

    std::list< std::shared_ptr<uint_t> > * sampleBuffer;

    uint_t winSize;
    uint_t hopSize;
    uint_t fillSize;

private:
    void checkAndCopySample(std::shared_ptr<quint32> block,
                                        std::shared_ptr<quint32> sample,
                                        unsigned int size);

signals:
    void finish();
public slots:
    void delList();
};

class QtReader : public  AudioInput {

public:
    QtReader();
    virtual ~QtReader();

    void stop();
    void start();

private:
    void initializeAudio(const QAudioDeviceInfo & deviceInfo);

protected:

private:
    QScopedPointer<QAudioInput> audioInput;
    QScopedPointer<QtAudioDevice> audioDevice;

signals:

public slots:

};

/*class AubioReader : public  AudioInput {

public:
    AubioReader();
    virtual ~AubioReader();

    void stop();
    void start();


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
*/

#endif // I_AUDIO_INPUT_H
