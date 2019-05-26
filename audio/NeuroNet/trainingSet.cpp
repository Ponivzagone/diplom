#include "trainingSet.h"
#include <QDebug>

trainingSet::trainingSet(const std::string filename)
{
    try {
        trainingDataFile.open(filename.c_str(), std::ifstream::in);
    }
    catch (...) {
      qDebug() << "Exception opening/reading/closing file\n";
    }
}

void trainingSet::getTopology(std::vector<unsigned> &topology)
{
	std::string line;
	std::string label;

	std::getline(trainingDataFile, line);
	std::stringstream ss(line);
	ss >> label;
	if (this->isEOF() || label.compare("topology:") != 0)
	{
		abort();
	}

	while (!ss.eof())
	{
		unsigned n;
		ss >> n;
		topology.push_back(n);
	}
	return;
}

unsigned trainingSet::getNextInputs(std::vector<double> &inputVals)
{
	inputVals.clear();

	std::string line;
	std::getline(trainingDataFile, line);
	std::stringstream ss(line);

	std::string label;
	ss >> label;
	if (label.compare("in:") == 0) {
		double oneValue;
		while (ss >> oneValue) {
			inputVals.push_back(oneValue);
		}
	}

	return inputVals.size();
}

unsigned trainingSet::getTargetOutputs(std::vector<double> &targetOutputVals)
{
	targetOutputVals.clear();

	std::string line;
	std::getline(trainingDataFile, line);
	std::stringstream ss(line);

	std::string label;
	ss >> label;
	if (label.compare("out:") == 0) {
		double oneValue;
		while (ss >> oneValue) {
			targetOutputVals.push_back(oneValue);
		}
	}

    return targetOutputVals.size();
}

bool trainingSet::getWeight(std::vector<double> &readWeight)
{
    std::string line;
    std::getline(trainingDataFile, line);
    std::stringstream ss(line);

    std::string label;
    ss >> label;
    if(label.compare("weight:") == 0)
    {
        double oneValue;
        while (ss >> oneValue) {
            readWeight.push_back(oneValue);
        }
        return true;
    }

    return false;
}

trainingSet::~trainingSet()
{
    trainingDataFile.close();
}
