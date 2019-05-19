#include "AlgorithManager.h"

#include <settings/config_reader.h>

AlgorithManager::AlgorithManager(uint_t winSize, uint_t hopSize)
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

void AlgorithManager::algLoop(std::list<std::shared_ptr<Sample> > *sampleBuffer)
{

}

void AlgorithManager::algStep(std::shared_ptr<Sample> sample)
{

}
