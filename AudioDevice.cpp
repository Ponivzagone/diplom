#include "AudioDevice.h"
#include <qendian.h>
#include <QDebug>

QtAudioDevice::QtAudioDevice(const QAudioFormat &format, QtReader *parent)
    : audioInput(parent), format(format)
{
    Q_ASSERT(format.sampleSize() % 8 == 0);
    channelBytes = format.sampleSize() / 8;
    sampleBytes = format.channelCount() * channelBytes;
}

QtAudioDevice::~QtAudioDevice()
{
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

qint64 QtAudioDevice::writeData(const char *data, qint64 maxSize)
{

    Q_ASSERT(maxSize % sampleBytes == 0);
    const int numSamples = maxSize / sampleBytes;
    const unsigned char * ptr = reinterpret_cast<const unsigned char *>(data);
    std::shared_ptr<float> buffer(new float[numSamples], [](float *p) {
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
        buffer.get()[i] = static_cast<float>(sumChanel);
    }
    audioInput->setSamples(buffer, numSamples);

    return maxSize;
}


void AubioDevice::run()
{
    uint_t read = 0;
    fvec_t * in = new_fvec(audioInput->hopSize);
    audioInput->done = false;

    std::shared_ptr<float> tmp(new float[audioInput->hopSize], [](float * p){
        delete [] p;
    });
    int i = 1;
    do {
        std::cout << "r" << i++ << " " << std::endl;
        aubio_source_do(audioInput->source, in, &read);
        std::memcpy(tmp.get(), in->data, in->length * sizeof(float));


        audioInput->setSamples(tmp, audioInput->hopSize);

        audioInput->record_cond.wakeAll();


    } while( read == audioInput->hopSize && (audioInput->isRecord()) );

    del_fvec(in);
    audioInput->done = true;
    audioInput->record_cond.wakeAll();

    std::cout << "  norm stop  ";
}
