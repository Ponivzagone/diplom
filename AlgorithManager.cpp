#include "AlgorithManager.h"

#include <settings/config_reader.h>
#include "AudioInput.h"
#include <iostream>
#include <map>

AlgorithManager::AlgorithManager(uint_t _winSize, uint_t _hopSize, uint_t _sampleRate)
    : winSize(_winSize)
{
    spectrumeAnalyze.reset(new AubioFFT(winSize));
    beatTracker.reset(new AubioTempo(
                            winSize,
                            _hopSize,
                            _sampleRate
                          ));

    trainingSet trainingData("/srv/Diplom/app/courseWork/dataSetNet/empty.txt");
    std::vector<unsigned> topology;
    trainingData.getTopology(topology);
    net.reset(new Net(topology));
    pageBuilder.reset(new NoteListBuilder(_sampleRate, _winSize, _hopSize));
}

AlgorithManager::~AlgorithManager()
{

}

void AlgorithManager::algLoop(std::list<std::shared_ptr<Sample> > & sampleBuffer)
{

    std::vector<float>  tempoRanged;
    std::vector<double> results;
    results.reserve(88);
    tempoRanged.reserve(100);

    for(auto& sample : sampleBuffer)
    {
        sample->setEmpthy();
        algStep(sample);

        tempoRanged.push_back(beatTracker->getTempo());

        net->getResults(results);
        pageBuilder->selectionNotes(results);


    }

    std::nth_element(tempoRanged.begin(), tempoRanged.begin() + tempoRanged.size()/2, tempoRanged.end());
    std::cout << "The median tempo is " << tempoRanged[tempoRanged.size()/2] << '\n';

    pageBuilder->buildPage( tempoRanged.at( tempoRanged.size() / 2 ) );
    pageBuilder->render();
}

void AlgorithManager::algStep(std::shared_ptr<Sample> & sample)
{
    beatTracker->detecting(sample);

    spectrumeAnalyze->fftDo(sample);
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

    unsigned lenght = input.size();
    for(unsigned i = 0; i < lenght; ++i)
    {
        input[i] = input[i] / max;
    }
    net->feedForward(input);

}

