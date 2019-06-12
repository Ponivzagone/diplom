#include "trainingSet.h"
#include "neuron.h"
#include "net.h"

typedef std::vector<double> neuroW;

double Net::recentAverageSmoothingFactor = 100.0; // Number of training samples to average over

Net::Net(const std::vector<unsigned> &topology)
{
	unsigned numLayers = topology.size();
    std::vector<neuroW> readWeight;

    trainingSet loadWeight("/srv/dev/diplom/dataSetNet/weight.txt");
    for (unsigned layerNum = 0; layerNum < numLayers; ++layerNum)
    {
		layers.push_back(Layer());
		unsigned numOutputs = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1];
        for (unsigned neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum)
        {
            readWeight.push_back(neuroW());
            if(loadWeight.getWeight(readWeight[neuronNum]))
                layers.back().push_back(Neuron(numOutputs, neuronNum, readWeight[neuronNum]));
            else
                layers.back().push_back(Neuron(numOutputs, neuronNum));
        }
		layers.back().back().setOutputVal(1.0);
	}
}


void Net::getResults(std::vector<double> &resultVals) const
{

	for (unsigned n = 0; n < layers.back().size() - 1; ++n)
	{
		resultVals.push_back(layers.back()[n].getOutputVal());
    }
}

void Net::backProp(const std::vector<double> &targetVals)
{

	Layer &outputLayer = layers.back();
	error = 0.0;

	for (unsigned n = 0; n < outputLayer.size() - 1; ++n)
	{
		double delta = targetVals[n] - outputLayer[n].getOutputVal();
		error += delta * delta;
	}
	error /= outputLayer.size() - 1; 
	error = sqrt(error); 


	recentAverageError =
		(recentAverageError * recentAverageSmoothingFactor + error)
		/ (recentAverageSmoothingFactor + 1.0);


	for (unsigned n = 0; n < outputLayer.size() - 1; ++n)
	{
		outputLayer[n].calcOutputGradients(targetVals[n]);
	}

	for (unsigned layerNum = layers.size() - 2; layerNum > 0; --layerNum)
	{
		Layer &hiddenLayer = layers[layerNum];
		Layer &nextLayer = layers[layerNum + 1];

		for (unsigned n = 0; n < hiddenLayer.size(); ++n)
		{
			hiddenLayer[n].calcHiddenGradients(nextLayer);
		}
	}


	for (unsigned layerNum = layers.size() - 1; layerNum > 0; --layerNum)
	{
		Layer &layer = layers[layerNum];
		Layer &prevLayer = layers[layerNum - 1];

		for (unsigned n = 0; n < layer.size() - 1; ++n)
		{
			layer[n].updateInputWeights(prevLayer);
		}
	}
}

void Net::getneuroWeight()
{
    std::vector<neuroW> readWeight;

    for(unsigned layerNum = 0; layerNum < layers.size() - 1; ++layerNum)
    {
        Layer &layer = layers[layerNum];
        for(unsigned n = 0; n < layer.size(); ++n)
        {
            readWeight.push_back(neuroW());
            layer[n].getWeight(readWeight.back());
        }
    }
    std::ofstream fout;
    fout.open("/srv/dev/diplom/dataSetNet/weight.txt");

    for(unsigned i = 0; i < readWeight.size(); ++i)
    {
        fout <<"weight:";
        for(unsigned j = 0; j < readWeight[i].size(); ++j)
        {
            fout << " " << readWeight[i][j];
        }
        fout << std::endl;
    }
    fout.close();
}

void Net::feedForward(const std::vector<double> &inputVals)
{
	assert(inputVals.size() == layers[0].size() - 1);

	for (unsigned i = 0; i < inputVals.size(); ++i) {
		layers[0][i].setOutputVal(inputVals[i]);
	}

	for (unsigned layerNum = 1; layerNum < layers.size(); ++layerNum) {
		Layer &prevLayer = layers[layerNum - 1];
		for (unsigned n = 0; n < layers[layerNum].size() - 1; ++n) {
			layers[layerNum][n].feedForward(prevLayer);
		}
	}
}

