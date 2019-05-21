#ifndef I_AUDIO_INPUT_H
#define I_AUDIO_INPUT_H

#include <QObject>
#include <QAudioInput>
#include <aubio/aubio.h>
#include <QScopedPointer>
#include <memory>

#include <aubio/aubio.h>

#include "AlgorithManager.h"

class QtAudioDevice;

struct Sample {

    Sample() {}
    Sample(quint32 _winSize) : amplitude(new quint32[_winSize]), fillSize(0), winSize(_winSize) {}
    Sample(const Sample & tmp) : amplitude(new quint32[tmp.winSize]), fillSize(tmp.fillSize), winSize(tmp.winSize)
    {
        memcpy(amplitude, tmp.amplitude, sizeof(quint32) * tmp.winSize);
    }
    Sample & operator=(const Sample & rhs)
    {
        if(&rhs == this) {
            return *this;
        }
        winSize = rhs.winSize;
        fillSize = rhs.fillSize;
        if(!amplitude) { new quint32[rhs.winSize]; }
        memcpy(amplitude, rhs.amplitude, sizeof(quint32) * rhs.winSize);
        return *this;
    }
    ~Sample() { if(amplitude) { delete [] amplitude; } }

    void setEmpthy()
    {
        for(quint32 i = fillSize; i < winSize; i++)
        {
            amplitude[i] = 0;
        }
    }

    std::shared_ptr<fvec_t> convertAubio()
    {
        std::shared_ptr<fvec_t> tmp((fvec_t *)calloc(sizeof(fvec_t),1), [](fvec_t * tmp){
                                                                    free(tmp);
                                                                });
        tmp->data =  reinterpret_cast<smpl_t * >(this->amplitude);//полная хуйня
        tmp->length = winSize;
        return tmp;
    }

    void convertAubio(fvec_t * in)
    {
        if(in->length != winSize) {
            throw std::runtime_error("aubio FFT winSize != input signal length");
            return;
        }
        in->data = reinterpret_cast<smpl_t * >(this->amplitude);//полная хуйня
    }



    quint32 * amplitude = nullptr;
    quint32 fillSize;
    quint32 winSize;
};


class AudioInput : public QObject {

    Q_OBJECT

public:
    explicit AudioInput(QObject *parent = nullptr);
    virtual ~AudioInput() = 0;

    virtual void stop()  = 0;
    virtual void start() = 0;


    uint_t getWinSize() const;
    uint_t getHopSize() const;

    void setSamples(std::shared_ptr<quint32> samples, unsigned int sizeSample);

protected:

    void initSampleBuffer();

    QScopedPointer<AlgorithManager> algo;

    std::list< std::shared_ptr< Sample > > * sampleBuffer;

    quint32 winSize;
    quint32 hopSize;

private:
    void checkAndCopySample(std::shared_ptr<Sample> block,
                                        std::shared_ptr<quint32> sample,
                                        unsigned int size);

    void setLastSample();

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

class AubioReader : public  AudioInput {

public:
    AubioReader();
    virtual ~AubioReader();

    void stop();
    void start();


protected:

private:

    char_t * sourcePath;
    aubio_source_t * source;

    uint_t sampleRate;

signals:

public slots:

};


#endif // I_AUDIO_INPUT_H
