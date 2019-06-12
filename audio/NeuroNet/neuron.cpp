#include "neuron.h"
#include <math.h>

double Neuron::eta = 0.15; // net learning rate
double Neuron::alpha = 0.5; // momentum

 Neuron::Neuron(unsigned numOutputs, unsigned myIndex)
{
	for (unsigned c = 0; c < numOutputs; ++c) {
		outputWeights.push_back(Connection());
		outputWeights.back().weight = randomWeight();
	}

    m_myIndex = myIndex;
 }

 Neuron::Neuron(unsigned numOutputs, unsigned myIndex, std::vector<double> &weight)
 {
     for (unsigned c = 0; c < numOutputs; ++c) {
         outputWeights.push_back(Connection());
         outputWeights.back().weight = weight[c];
     }

     m_myIndex = myIndex;
 }

void Neuron::updateInputWeights(Layer &prevLayer)
{
	for (unsigned n = 0; n < prevLayer.size(); ++n)
	{
        Neuron &neuron = prevLayer[n];
		double oldDeltaWeight = neuron.outputWeights[m_myIndex].deltaWeight;

		double newDeltaWeight = eta * neuron.getOutputVal() * gradient + alpha * oldDeltaWeight;
		neuron.outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
		neuron.outputWeights[m_myIndex].weight += newDeltaWeight;
    }
}

void Neuron::getWeight(std::vector<double> &weig)
{
    unsigned lenght = this->outputWeights.size();
    for(unsigned i = 0; i < lenght; ++i)
    {
        weig.push_back(this->outputWeights[i].weight);
    }
}

double Neuron::sumDOW(const Layer &nextLayer) const
{
	double sum = 0.0;

	for (unsigned n = 0; n < nextLayer.size() - 1; ++n)
	{
		sum += outputWeights[n].weight * nextLayer[n].gradient;
	}

	return sum;
}

void Neuron::calcHiddenGradients(const Layer &nextLayer)
{
	double dow = sumDOW(nextLayer);
    gradient = dow * Neuron::activationFunctionDerivative(outputVal);
}
void Neuron::calcOutputGradients(double targetVals)
{
	double delta = targetVals - outputVal;
    gradient = delta * Neuron::activationFunctionDerivative(outputVal);
}

double Neuron::activationFunction(double x)
{
	//output range [-1.0..1.0]
	return tanh(x);
}

double Neuron::activationFunctionDerivative(double x)
{
	return 1.0 - x * x;
}

void Neuron::feedForward(const Layer &prevLayer)
{
	double sum = 0.0;

	for (unsigned n = 0; n < prevLayer.size(); ++n)
	{
		sum += prevLayer[n].getOutputVal() *
			prevLayer[n].outputWeights[m_myIndex].weight;
	}
    sum = Neuron::activationFunction(sum);
    if(sum == 1)
        outputVal = 0.999;
    if(sum  == -1)
        outputVal = -0.999;
    else
        outputVal = sum;
}
