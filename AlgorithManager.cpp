#include "AlgorithManager.h"

#include <settings/config_reader.h>
#include "AudioInput.h"
#include <iostream>

AlgorithManager::AlgorithManager(uint_t _winSize, uint_t hopSize)
    : winSize(_winSize)
{
    spectrumeAnalyze.reset(new AubioFFT(winSize));
    beatTracker.reset(new AubioTempo(
                            winSize,
                            hopSize,
                            ConfigReader::instance().getValue<uint>(CoreSettings::sample_rate, 8000)
                          )); // Thread SAVE??? if change setting in UI at runtime algo

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
    float * jopa = spectrumeAnalyze->getNorm();
    for(int i = 0; i < winSize; i += 100)
    {
        std::cout << jopa[i] << "   ";
    }
    std::cout << std::endl;

}
