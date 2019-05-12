#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <QtCore/QIODevice>
#include <aubio/aubio.h>
#include <QtCore/QPointF>
#include <QtCore/QVector>


class AubioDevice
{
public:
    AubioDevice();
};


class QtAudioDevice : public QIODevice
{
    Q_OBJECT
public:
    explicit QtAudioDevice(QObject *parent = nullptr);
    virtual ~QtAudioDevice() override;

protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

private:
    QVector<QPointF> m_buffer;
};
#endif // AUDIODEVICE_H
