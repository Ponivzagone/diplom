#ifndef NOTELISTBUILDER_H
#define NOTELISTBUILDER_H
#include <list>
#include <vector>
#include "NoteDescription.h"


class NoteListBuilder
{
public:
    NoteListBuilder(uint _sampleRate, uint _winSize, uint _hopSize);
    ~NoteListBuilder();


    void buildPage(float _tempo);
    void selectionNotes(std::vector<double> & probability);

    void render();

private:

    bool inTonal(uint index);

    std::list<tact> page;
    std::vector<std::vector<ushort> > indexNote;

    uint winSize;
    uint hopSize;
    uint sampleRate;
    double tempo;


    double timer;
    double offset;

};

#endif // NOTELISTBUILDER_H
