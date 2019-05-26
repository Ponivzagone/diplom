#ifndef RECOGNITION_H
#define RECOGNITION_H
#include "trainingSet.h"
#include "neuron.h"
#include "net.h"

class Recognition
{
public:
    Recognition();
    void startRec(std::vector<double> &inputVals, std::vector<double> &resultvals, double max);
    ~Recognition();
private:
    Net *net;
    std::vector<unsigned> topology;
};
#endif // RECOGNITION_H
