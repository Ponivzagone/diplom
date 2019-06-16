#ifndef I_AUDIO_INPUT_H
#define I_AUDIO_INPUT_H

#include <QObject>
#include <QAudioInput>
#include <aubio/aubio.h>
#include <QScopedPointer>
#include <memory>

#include <QWaitCondition>
#include <QMutex>

#include <cstring>

#include <QImage>
#include <poppler/qt5/poppler-qt5.h>

#include "settings/config_reader.h"

#include <iostream>
#include "AlgorithManager.h"

#define DIVIDER 32


struct Sample {

    Sample() {}
    Sample(quint32 _winSize) : amplitude(new float[_winSize]), fillSize(0), winSize(_winSize), uniqSampleHop(0) {
         std::memset(amplitude, 0.0f, sizeof(float) * winSize);
    }
    Sample(const Sample & tmp) : amplitude(new float[tmp.winSize]), fillSize(tmp.fillSize), winSize(tmp.winSize)
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

    void setEmpthy();

    std::shared_ptr<fvec_t> convertAubio();


    void convertAubio(fvec_t * in);


    void convertAubioHop(fvec_t * in);

    uint getHopSize();


    smpl_t  * amplitude = nullptr;
    quint32   uniqSampleHop;
    quint32   fillSize;
    quint32   winSize;
};
#include <QUrl>

class AudioInput : public QObject {

    Q_OBJECT

public:

    enum statusByte { record = 2 };

    explicit AudioInput(QObject *parent = nullptr);
    virtual ~AudioInput() = 0;

    virtual void stop()  = 0;
    virtual void start() = 0;

    void setSamples(std::shared_ptr<float> samples, unsigned int sizeSample);

    Q_INVOKABLE void setUrl(const QUrl & fileUrl)
    {
        ConfigReader::instance().setValue<QString>(CoreSettings::source_path, fileUrl.toString());
    }

    Q_INVOKABLE void setNextDur(const double & nextDur)
    {
        ConfigReader::instance().setValue<double>(CoreSettings::round_dur, nextDur);
    }

    Q_INVOKABLE void setNoteProb(const double & noteProb)
    {
        ConfigReader::instance().setValue<double>(CoreSettings::note_prob, noteProb);
    }

    void Image();


protected:

    QScopedPointer<AlgorithManager> algo;

    std::list< std::shared_ptr< Sample > > * sampleBuffer = nullptr;
    void initSampleBuffer();

    uint sampleRate;
    uint bitRate;
    uint winSize;
    uint hopSize;

    QString sourcePath;

    bool isRecord();
    void changeRecordStatus();

    int status;



private:
    void checkAndCopySample(std::shared_ptr<Sample> block,
                                        std::shared_ptr<float> sample,
                                        unsigned int size);

signals:
    void finishAlgo(const QImage &image);
    void recFinished();



public Q_SLOTS:
    void stopRecord();
    void startRecord();


public slots:
    void startAlgo();
    void configHandler(uint key);
};

class QtAudioDevice;
class QtReader : public  AudioInput {

    friend QtAudioDevice;

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

class AubioDevice;
class AubioReader : public  AudioInput {

    friend AubioDevice;

public:

    AubioReader();
    virtual ~AubioReader();

    void stop();
    void start();

private:

    AubioDevice * audioDevice;
    aubio_source_t * source;


    QMutex record_mutex;
    QWaitCondition record_cond;

    bool done;

signals:


public slots:

};


#endif // I_AUDIO_INPUT_H
