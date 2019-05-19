#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <QAudioInput>
#include <aubio/aubio.h>
#include <QtCore/QPointF>
#include <QtCore/QVector>
#include <QScopedPointer>
#include <memory>
#include "AudioInput.h"


class QtAudioDevice : public QIODevice
{
    Q_OBJECT
public:
    explicit QtAudioDevice(const QAudioFormat & format, AudioInput *parent = nullptr);
    virtual ~QtAudioDevice() override;

    void start();
    void stop();

protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

private:
    AudioInput * audioInput;
    const QAudioFormat format;

    quint32 * buffer;
    quint32   fillSize;

    int channelBytes;
    int sampleBytes;

    uint winSize;
    uint hopSize;
};
#endif // AUDIODEVICE_H
