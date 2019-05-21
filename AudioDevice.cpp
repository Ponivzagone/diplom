#include "AudioDevice.h"
#include <qendian.h>
#include <QDebug>

QtAudioDevice::QtAudioDevice(const QAudioFormat &format, AudioInput *parent)
    : audioInput(parent), format(format), fillSize(0)
{
    buffer =  new quint32[audioInput->getWinSize()];

    winSize = audioInput->getWinSize();
    hopSize = audioInput->getHopSize();

    Q_ASSERT(format.sampleSize() % 8 == 0);
    channelBytes = format.sampleSize() / 8;
    sampleBytes = format.channelCount() * channelBytes;
}

QtAudioDevice::~QtAudioDevice()
{
    delete [] buffer;
}

void QtAudioDevice::start()
{
    open(QIODevice::WriteOnly);
}

void QtAudioDevice::stop()
{
    close();
}

qint64 QtAudioDevice::readData(char *data, qint64 maxSize)
{
    Q_UNUSED(data)
    Q_UNUSED(maxSize)
    return -1;
}
#include <iostream>

qint64 QtAudioDevice::writeData(const char *data, qint64 maxSize)
{

    Q_ASSERT(maxSize % sampleBytes == 0);
    const int numSamples = maxSize / sampleBytes;
    const unsigned char * ptr = reinterpret_cast<const unsigned char *>(data);
    std::shared_ptr<quint32> buffer(new quint32[numSamples], [](quint32 *p) {
        qDebug() << "buffer~shared_ptr" << endl;
        delete[] p;
    });

    for (int i = 0; i < numSamples; ++i) {
        quint32 sumChanel = 0;

        for (int j = 0; j < format.channelCount(); ++j) {
            quint32 value = 0;

            if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::UnSignedInt) {
                value = *reinterpret_cast<const quint8*>(ptr);
            } else if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::SignedInt) {
                value = qAbs(*reinterpret_cast<const qint8*>(ptr));
            } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::UnSignedInt) {
                if (format.byteOrder() == QAudioFormat::LittleEndian)
                    value = qFromLittleEndian<quint16>(ptr);
                else
                    value = qFromBigEndian<quint16>(ptr);
            } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::SignedInt) {
                if (format.byteOrder() == QAudioFormat::LittleEndian)
                    value = qAbs(qFromLittleEndian<qint16>(ptr));
                else
                    value = qAbs(qFromBigEndian<qint16>(ptr));
            } else if (format.sampleSize() == 32 && format.sampleType() == QAudioFormat::UnSignedInt) {
                if (format.byteOrder() == QAudioFormat::LittleEndian)
                    value = qFromLittleEndian<quint32>(ptr);
                else
                    value = qFromBigEndian<quint32>(ptr);
            } else if (format.sampleSize() == 32 && format.sampleType() == QAudioFormat::SignedInt) {
                if (format.byteOrder() == QAudioFormat::LittleEndian)
                    value = qAbs(qFromLittleEndian<qint32>(ptr));
                else
                    value = qAbs(qFromBigEndian<qint32>(ptr));
            } else if (format.sampleSize() == 32 && format.sampleType() == QAudioFormat::Float) {
                value = qAbs(*reinterpret_cast<const float*>(ptr) * 0x7fffffff); // assumes 0-1.0
            }

            value /= format.channelCount();
            sumChanel += value;
            ptr += channelBytes;
        }
        buffer.get()[i] = sumChanel;
    }
    audioInput->setSamples(buffer, numSamples);

    return maxSize;
}

