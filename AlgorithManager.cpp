#include "AlgorithManager.h"

#include <settings/config_reader.h>
#include "AudioInput.h"
#include <iostream>
#include <map>

AlgorithManager::AlgorithManager(uint_t _winSize, uint_t _hopSize, uint_t _sampleRate)
    : winSize(_winSize), sampleRate(_sampleRate), hopSize(_hopSize)
{
    spectrumeAnalyze.reset(new AubioFFT(winSize));
    beatTracker.reset(new AubioTempo(
                            winSize,
                            _hopSize,
                            _sampleRate
                          ));

    trainingSet trainingData("/srv/dev/diplom/dataSetNet/empty.txt");
    std::vector<unsigned> topology;
    trainingData.getTopology(topology);
    net.reset(new Net(topology));
}

AlgorithManager::~AlgorithManager()
{

}



std::string AlgorithManager::algLoop(std::list<std::shared_ptr<Sample> > & sampleBuffer)
{
    QScopedPointer<NoteListBuilder> pageBuilder(new NoteListBuilder(sampleRate, winSize, hopSize));
    std::vector<float>  tempoRanged;
    std::vector<double> results;
    results.reserve(88);
    tempoRanged.reserve(100);

    for(auto& sample : sampleBuffer)
    {
        sample->setEmpthy();
        int code = algStep(sample);

        tempoRanged.push_back(beatTracker->getTempo());

        results.clear();
        if(!code) { net->getResults(results); }

        pageBuilder->selectionNotes(results, spectrumeAnalyze.get());


    }

    std::nth_element(tempoRanged.begin(), tempoRanged.begin() + tempoRanged.size()/2, tempoRanged.end());
    std::cout << "The median tempo is " << tempoRanged[tempoRanged.size()/2] << '\n';

    pageBuilder->buildPage( tempoRanged.at( tempoRanged.size() / 2 ) );
    std::string ss;
    pageBuilder->render(ss);
    return ss;
}

int AlgorithManager::algStep(std::shared_ptr<Sample> & sample)
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

    if(max < 5.0) { return 1; }

    unsigned lenght = input.size();
    for(unsigned i = 0; i < lenght; ++i)
    {
        input[i] = input[i] / max;
    }
    net->feedForward(input);
    return 0;
}

