#ifndef NOTELISTBUILDER_H
#define NOTELISTBUILDER_H
#include <list>
#include <vector>
#include "NoteDescription.h"
#include "AudioFFT.h"
#include <sstream>
#include <tuple>
#include <list>

#define NOTEPROBABILITY 0.7




struct NoteChangeStatus
{
    NoteChangeStatus()
    {
        n.reserve(89);
        for(int i = 0; i < 89; ++i)
        {
            n.emplace_back(std::make_pair<uint, float>(i, 0.0));
        }
    }

    ~NoteChangeStatus() {}


    void zero()
    {
        for(int i = 0; i < 89; ++i)
        {
            n.at(i).second = 0.0;
        }
    }

    void setAmp(int ind, float amp)
    {
        n.at(ind).second = amp;
    }



    std::vector<std::pair<uint, float> > n; // текущий семпл, с чем сравнить, чего щазз
};


class NoteListBuilder
{
public:
    NoteListBuilder(uint _sampleRate, uint _winSize, uint _hopSize);
    ~NoteListBuilder();


    void buildPage(float _tempo);
    void selectionNotes(std::vector<double> & probability, AudioFFT * fft);

    void render(std::string &ss);

private:

    void noteHzInit();

    std::pair<ushort, StatusNote> initStatus(ushort index, float amp);

    std::vector<double> HzNote;

    std::vector<std::pair<ushort, double > > prevNote;


    double NoteProbability;

    std::list<tact> page;
    std::vector<std::vector<std::pair<ushort, StatusNote > > > indexNote;

    uint winSize;
    uint hopSize;
    uint sampleRate;

    uint noteChange;
    uint currentSample;

    double stepSpectr;

    double tempo;


    double timer;
    double offset;

    NoteChangeStatus statusN;

};

#endif // NOTELISTBUILDER_H
