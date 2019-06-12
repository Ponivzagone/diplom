#include "xyseriesiodevice.h"
#include <QtCharts/QXYSeries>

const double PI = 3.14159265358979323846264338;
XYSeriesIODevice::XYSeriesIODevice(QXYSeries * series, QXYSeries *s_series, QObject *parent) :
    QIODevice(parent),
    m_series(series),
    spect_series(s_series)
{
    fft_Complex_array = new std::complex<double>[1024];
    start = new Recognition();
    for(int i = 0; i < 512; ++i)
        fft_amplitude_array.push_back(0.0);
    for(int i = 0; i < 1024; ++i)
    {
        heming.push_back(0.0);
        heming[i] = 0.54 - 0.46 * cos((2.0 * 3.1415 * i)/(1024 - 1));
    }
    count = 0;
    max = 0.0;
}

XYSeriesIODevice::~XYSeriesIODevice()
{
    delete[] fft_Complex_array;
    delete start;
}

qint64 XYSeriesIODevice::readData(char * data, qint64 maxSize)
{
    Q_UNUSED(data)
    Q_UNUSED(maxSize)
    return -1;
}

void Rearrange(std::complex<double> *samples, int length)
{
    int *rearrange;
    rearrange = new int[length];

    rearrange[0] = 0;
    for(int limit = 1, bit = length / 2; limit < length; limit <<= 1, bit>>=1 )
     for(int i = 0; i < limit; i++)
        rearrange[i + limit] = rearrange[i] + bit;

    for(int i = 0; i < length; i++) {
     if (rearrange[i] == i) rearrange[i] = 0;
     else rearrange[ rearrange[i] ] = 0;
    }

   std::complex<double> t;
   for (int i = 0; i < length; ++i)
      if (rearrange[i]) {
         t = samples[i];
         samples[i] = samples[ rearrange[i] ];
         samples[ rearrange[i] ] = t;
      }
   delete[] rearrange;
}

void ForwardFft(std::complex<double> *samples, int length )
{
   Rearrange(samples,length);
   for(int halfSize = 1; halfSize < length; halfSize *= 2)
   {

      std::complex<double> phaseShiftStep = std::polar(1.0, -PI / halfSize);
      std::complex<double> currentPhaseShift(1, 0);

      for(int fftStep = 0; fftStep < halfSize; ++fftStep)
      {

         for(int i = fftStep; i < length; i += 2 * halfSize)
         {
            std::complex<double> t = currentPhaseShift * samples[i + halfSize];
            samples[i + halfSize] = samples[i] - t;
            samples[i] += t;
         }

         currentPhaseShift *= phaseShiftStep;
      }
   }
}

qint64 XYSeriesIODevice::writeData(const char * data, qint64 maxSize)
{
    qint64 range = 1024;
    QVector<QPointF> oldPoints = m_series->pointsVector();
    QVector<QPointF> points;
    QVector<QPointF> spectrumePoints;
    double localMax = 0.0;
    if (oldPoints.count() < range)
    {
        points = m_series->pointsVector();
    }
    else
    {
        for (int i = maxSize; i < oldPoints.count(); ++i)
        {
            points.append(QPointF(i - maxSize, oldPoints.at(i).y()));
        }
    }

    qint64 size = points.count();
    for (int i = 0; i < maxSize; ++i)
    {
        points.append(QPointF(i + size, (quint8)data[i]));
    }
    if(points.length() > range)
    {
        for(int i = 0; i < range; ++i)
        {

            fft_Complex_array[i] = std::complex<double>(points.at(i).y());
        }
        ForwardFft(fft_Complex_array, range);
        double halfRange = range/ 2;
        for(int i = 20; i < halfRange; ++i)
        {
            fft_amplitude_array[i] = std::abs(fft_Complex_array[i] /* * heming[i]*/);

            if(localMax < fft_amplitude_array[i])
                localMax = fft_amplitude_array[i];
            spectrumePoints.append(QPointF((double)i / range * 8000 / 1.0, fft_amplitude_array[i]));
        }
        if(max > localMax)
        {
            if(max > 500)
            {
                    count += maxSize;
                    max = 0.0;
                    count = 0;
                    start->startRec(fft_amplitude_array, result, localMax);
                    emit update(result);
            }
            else
                emit setDefaultColor(0);
        }
        else
        {

            max = localMax;
        }
    }
    m_series->replace(points);
    spect_series->replace(spectrumePoints);
    return maxSize;
}
