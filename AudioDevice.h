#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <QAudioInput>
#include <aubio/aubio.h>
#include <memory>

#include <QThread>
#include <cstring>

#include "AudioInput.h"

class QtAudioDevice : public QIODevice
{
    Q_OBJECT
public:
    explicit QtAudioDevice(const QAudioFormat & format, QtReader *parent = nullptr);
    virtual ~QtAudioDevice() override;

    void start();
    void stop();

protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

private:
    QtReader * audioInput;
    const QAudioFormat format;  

    int channelBytes;
    int sampleBytes;

    uint winSize;
    uint hopSize;
};

class AubioDevice : public QThread
{
    Q_OBJECT

    void run() override;

public:
    explicit AubioDevice(AubioReader *parent = nullptr)
        : audioInput(parent) {}
    virtual ~AubioDevice() override {}

private:
    AubioReader * audioInput;
};

#endif // AUDIODEVICE_H
