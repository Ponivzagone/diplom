/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "widget.h"
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtWidgets/QVBoxLayout>
#include <QGridLayout>
#include <QPixmap>
#include <QtCharts/QValueAxis>
#include <algorithm>
#include "xyseriesiodevice.h"

QT_CHARTS_USE_NAMESPACE

Widget::Widget(QWidget *parent)
    : QWidget(parent),
      m_device(0),
      m_chart(0),
      spectrum_chart(0),
      m_series(0),
      m_audioInput(0)
{
    QGridLayout *pianoRoll = new QGridLayout;
    playerIconPath.append("/srv/Diplom/app/courseWork/audio/images/white.png");
    playerIconPath.append("/srv/Diplom/app/courseWork/audio/images/red.png");
    playerIconPath.append("/srv/Diplom/app/courseWork/audio/images/black.png");
    playerIconPath.append("/srv/Diplom/app/courseWork/audio/images/red2.png");
    int j = 2;
    for(int i = 0; i < 88; i++)
    {
        if(i > 26 && i < 64 )
        {
            keys.append(new QLabel);
            if((i <= (7 + 12 * j)) && (i >= (3 + 12 * j)))
                if(i % 2 == 0)
                   keys[i - 27]->setPixmap(QPixmap(playerIconPath[2]));
                else
                   keys[i - 27]->setPixmap(QPixmap(playerIconPath[0]));
            if(i % 12 == 8) j++;
            if((i <= (2 + 12 * j)) && (i >= (-4 + 12 * j)))
                if(i % 2 == 0)
                    keys[i - 27]->setPixmap(QPixmap(playerIconPath[0]));
                else
                    keys[i - 27]->setPixmap(QPixmap(playerIconPath[2]));
            pianoRoll->addWidget(keys[i - 27],0, i - 27);
        }
    }

    m_chart = new QChart;
    QChartView *chartView = new QChartView(m_chart);
    chartView->setMinimumSize(800, 400);
    m_series = new QLineSeries;
    m_chart->addSeries(m_series);
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 1024);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Samples");
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 255);
    axisY->setTitleText("Audio level");
    m_chart->setAxisX(axisX, m_series);
    m_chart->setAxisY(axisY, m_series);
    m_chart->legend()->hide();
    m_chart->setTitle("Data from the microphone");

    outputResult = new QTextEdit(tr("Result:"));

    spectrum_chart = new QChart;
    QChartView * spectrumView = new QChartView(spectrum_chart);
    spectrumView->setMinimumSize(800, 400);
    s_series = new QLineSeries;
    spectrum_chart->addSeries(s_series);
    QValueAxis *s_X = new QValueAxis;
    s_X->setRange(0, 4000);
    s_X->setLabelFormat("%g");
    s_X->setTitleText("hz");
    QValueAxis *s_Y = new QValueAxis;
    s_Y->setRange(0, 1000);
    s_Y->setTitleText("Amplitude");
    spectrum_chart->setAxisX(s_X, s_series);
    spectrum_chart->setAxisY(s_Y, s_series);
    spectrum_chart->legend()->hide();
    spectrum_chart->setTitle("Data spectrum from the microphone");


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(chartView);
   // mainLayout->addWidget(outputResult);
    mainLayout->addLayout(pianoRoll);
    mainLayout->addWidget(spectrumView);
    setLayout(mainLayout);

    QAudioFormat formatAudio;
    formatAudio.setSampleRate(8000);
    formatAudio.setChannelCount(1);
    formatAudio.setSampleSize(8);
    formatAudio.setCodec("audio/pcm");
    formatAudio.setByteOrder(QAudioFormat::LittleEndian);
    formatAudio.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo inputDevices = QAudioDeviceInfo::defaultInputDevice();
    m_audioInput = new QAudioInput(inputDevices,formatAudio, this);

    m_device = new XYSeriesIODevice(m_series, s_series, this);

    m_device->open(QIODevice::WriteOnly);

    m_audioInput->start(m_device);

    connect(m_device, &XYSeriesIODevice::update, this, [this](std::vector<double> result){
        for(unsigned i = 0; i < result.size(); ++i)
        {
            if(result[i] > 0.72)
            {
                idNotes.push_back(i);
            }
        }
        this->detectSoundingNotes(idNotes);
        idNotes.clear();
    });
    connect(m_device, &XYSeriesIODevice::setDefaultColor, this, [this](int color){
       this->setColorKeys(0, color);
    });
}

Widget::~Widget()
{
    m_audioInput->stop();
    m_device->close();
}

void Widget::detectSoundingNotes(std::vector<unsigned> &input)
{
        this->setColorKeys(0, 0);
        outputResult->clear();
        outputResult->setText(outputResult->toPlainText() + tr("notes:"));
        for(unsigned i = 0; i < input.size(); ++i)
        {
            switch (input[i])
            {
                case 39:
                    outputResult->setText(outputResult->toPlainText() + tr(" C "));
                    this->setColorKeys(input[i], 1);
                break;
                case 40:
                    outputResult->setText(outputResult->toPlainText() + tr(" C# "));
                    this->setColorKeys(input[i], 1);
                break;
                case 41:
                    outputResult->setText(outputResult->toPlainText() + tr(" D "));
                    this->setColorKeys(input[i], 1);
                break;
                case 42:
                    outputResult->setText(outputResult->toPlainText() + tr(" D# "));
                    this->setColorKeys(input[i], 1);
                break;
                case 43:
                    outputResult->setText(outputResult->toPlainText() + tr(" E "));
                    this->setColorKeys(input[i], 1);
                break;
                case 44:
                    outputResult->setText(outputResult->toPlainText() + tr(" F "));
                    this->setColorKeys(input[i], 1);
                break;
                case 45:
                    outputResult->setText(outputResult->toPlainText() + tr(" F# "));
                    this->setColorKeys(input[i], 1);
                break;
                case 46:
                    outputResult->setText(outputResult->toPlainText() + tr(" G "));
                    this->setColorKeys(input[i], 1);
                break;
                case 47:
                    outputResult->setText(outputResult->toPlainText() + tr(" G# "));
                    this->setColorKeys(input[i], 1);
                break;
                case 48:
                    outputResult->setText(outputResult->toPlainText() + tr(" A "));
                    this->setColorKeys(input[i], 1);
                break;
                case 49:
                    outputResult->setText(outputResult->toPlainText() + tr(" A# "));
                    this->setColorKeys(input[i], 1);
                break;
                case 50:
                    outputResult->setText(outputResult->toPlainText() + tr(" B "));
                    this->setColorKeys(input[i], 1);
                break;
                case 51:
                    outputResult->setText(outputResult->toPlainText() + tr(" C2 "));
                    this->setColorKeys(input[i], 1);
                break;
                case 52:
                    outputResult->setText(outputResult->toPlainText() + tr(" C#2 "));
                break;
                case 53:
                    outputResult->setText(outputResult->toPlainText() + tr(" D2 "));
                break;
                case 54:
                    outputResult->setText(outputResult->toPlainText() + tr(" D#2 "));
                break;
                case 55:
                    outputResult->setText(outputResult->toPlainText() + tr(" E2 "));
                break;
                case 56:
                    outputResult->setText(outputResult->toPlainText() + tr(" F2 "));
                break;
                case 57:
                    outputResult->setText(outputResult->toPlainText() + tr(" F#2 "));
                break;
                default:
                break;
            }
        }
}

void Widget::setColorKeys(int position, bool color)
{
    if(color)
    {
        for(int j = 2; j < 8; j++)
        {
                if((position <= (7 + 12 * j)) && (position >= (3 + 12 * j)))
                    if(position % 2 == 0)
                        keys[position -27]->setPixmap(QPixmap(playerIconPath[1]));
                    else
                        keys[position -27]->setPixmap(QPixmap(playerIconPath[3]));
                if((position <= (2 + 12 * j)) && (position >= (-4 + 12 * j)))
                    if(position % 2 == 0)
                        keys[position -27]->setPixmap(QPixmap(playerIconPath[3]));
                    else
                        keys[position -27]->setPixmap(QPixmap(playerIconPath[1]));
        }
    }
    if(!color)
    {
        int j = 2;
        for(int i = 0; i < 88; i++)
        {
            if(i > 26 && i < 64 )
            {
                if((i <= (7 + 12 * j)) && (i >= (3 + 12 * j)))
                    if(i % 2 == 0)
                        keys[i-27]->setPixmap(QPixmap(playerIconPath[2]));
                    else
                        keys[i-27]->setPixmap(QPixmap(playerIconPath[0]));
                if(i % 12 == 8) j++;
                if((i <= (2 + 12 * j)) && (i >= (-4 + 12 * j)))
                    if(i % 2 == 0)
                        keys[i-27]->setPixmap(QPixmap(playerIconPath[0]));
                    else
                        keys[i-27]->setPixmap(QPixmap(playerIconPath[2]));
            }
        }
    }
}

