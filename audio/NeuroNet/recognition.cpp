#include "recognition.h"
#include "trainingSet.h"
#include "neuron.h"
#include "net.h"


Recognition::Recognition()
{
    trainingSet trainingData("/srv/Diplom/app/courseWork/dataSetNet/empty.txt");
    trainingData.getTopology(topology);
    net = new Net(topology);
}

void Recognition::startRec(std::vector<double> &inputVals, std::vector<double> &resultVals, double max)
{
    unsigned lenght = inputVals.size();
    for(unsigned i = 0; i < lenght; ++i)
    {
        inputVals[i] = inputVals[i] / max;
    }
    net->feedForward(inputVals);
    net->getResults(resultVals);

}

Recognition::~Recognition()
{
    delete net;
}
