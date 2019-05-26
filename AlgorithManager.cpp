#include "AlgorithManager.h"

#include <settings/config_reader.h>
#include "AudioInput.h"
#include <iostream>


AlgorithManager::AlgorithManager(uint_t _winSize, uint_t _hopSize, uint_t _sampleRate)
    : winSize(_winSize)
{
    spectrumeAnalyze.reset(new AubioFFT(winSize));
    beatTracker.reset(new AubioTempo(
                            winSize,
                            _hopSize,
                            _sampleRate
                          ));
    noteDetector.reset(new Recognition());
}

AlgorithManager::~AlgorithManager()
{

}

void AlgorithManager::algLoop(std::list<std::shared_ptr<Sample> > & sampleBuffer)
{
    for(auto& sample : sampleBuffer)
    {
        sample->setEmpthy();
        algStep(sample);

        //TODO: analyse duration note
    }
}

void AlgorithManager::algStep(std::shared_ptr<Sample> & sample)
{
    spectrumeAnalyze->fftDo(sample);
    beatTracker->detecting(sample);

    std::vector<double> input;
    std::vector<double> result;
    double max = 0.0;

    input.reserve(winSize / 2);
    float * fft = spectrumeAnalyze->getNorm();

    for(int i = 0; i < winSize / 2; ++i)
    {
        input.push_back(std::move(static_cast<double>(fft[i])));
        max = std::max(max, input.back());
    }


    noteDetector->startRec(input, result, max);

    for(int i = 0; i < result.size(); ++i)
    {
        std::cout << result.at(i) << " " ;
    }
    std::cout << std::endl;
}
