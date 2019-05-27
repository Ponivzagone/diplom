#include "NoteListBuilder.h"
#include <algorithm>



NoteListBuilder::NoteListBuilder(uint _sampleRate, uint _winSize, uint _hopSize)
    :  winSize(_winSize), hopSize(_hopSize), sampleRate(_sampleRate), timer(0.0)
{
    indexNote.reserve(100);
    offset = static_cast<double>(hopSize) / static_cast<double>(sampleRate);
}

NoteListBuilder::~NoteListBuilder()
{

}

void NoteListBuilder::buildPage(float _tempo)
{
    tempo = static_cast<double>(_tempo);

    double beatInSec = tempo / 60.0;
    double secInBeat = 60.0 / tempo;

    double startOffset = static_cast<double>(winSize) / static_cast<double>(sampleRate);
    startOffset -= offset;
    timer += startOffset;
    page.push_back(tact(secInBeat));


    for(auto & block : indexNote)
    {
        auto & lastTact = page.back();


         block_note * blockNote(new block_note());

        for(auto & noteIndex : block)
        {
            symbol::SPtr n;
            if(noteIndex != 0) {
                n = std::make_shared<note>(note( duration(offset + startOffset), noteIndex ));
            } else {
                n = std::make_shared<pause>(pause( duration(offset + startOffset) ));
            }

            blockNote->addNote(n);
        }

        lastTact.setSymbol(std::shared_ptr<symbol>(blockNote));

        timer += offset;

        if(lastTact.exitRange(timer)) {
            page.push_back(tact(secInBeat));
        }

        startOffset = 0.0;
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
    static uint tonMask[] = {4,6,8,9,11,1,3}; // C_Major
    if(std::binary_search(tonMask, tonMask + 7, index % 12)) return true;
    return false;
}
