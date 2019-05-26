#pragma once
#include <vector>
#include <fstream>
#include "neuron.h"

class Net
{
public:
    Net(const std::vector<unsigned> &topology);
	void feedForward(const std::vector<double> &inputVals);
	void backProp(const std::vector<double> &targetVals);
	void getResults(std::vector<double> &resultVals) const;
	double getRecentAverageError() const { return recentAverageError; }
    void getneuroWeight();
private:
	std::vector<Layer> layers;//layers[layerNumber][NeuronNumber]
	double error;
	double recentAverageError;
	static double recentAverageSmoothingFactor;
};
