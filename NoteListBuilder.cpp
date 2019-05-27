#include "NoteListBuilder.h"
#include <algorithm>



NoteListBuilder::NoteListBuilder(uint _sampleRate, uint _winSize, uint _hopSize)
    :  winSize(_winSize), hopSize(_hopSize), sampleRate(_sampleRate), timer(0.0)
{
    indexNote.reserve(100);
    offset = hopSize / sampleRate;
}

NoteListBuilder::~NoteListBuilder()
{

}

void NoteListBuilder::buildPage(float _tempo)
{
    tempo = static_cast<double>(_tempo);

    double beatInSec = tempo / 60.0;
    double startOffset = winSize / sampleRate;



    for(auto & block : indexNote)
    {

    }
}

void NoteListBuilder::selectionNotes(std::vector<double> & probability)
{
    ushort index = 0;
    indexNote.push_back(std::vector<ushort>());
    auto & blockNote = indexNote.back();
    for(auto& note : probability)
    {
        ++index;
        if(note > 0.7)
        {
            blockNote.push_back(index);
        }
    }
    std::sort(blockNote.begin(), blockNote.end(), std::less<ushort>());
    if(blockNote.empty())
    {
        blockNote.push_back(0);
    }
}

void NoteListBuilder::render()
{

}

bool NoteListBuilder::inTonal(uint index)
{
    static uint tonMask[] = {4,6,8,9,11,1,3};
    if(std::binary_search(tonMask, tonMask + 7, index % 12)) return true;
    return false;
}
