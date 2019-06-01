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
    timer = 0.0;

    double beatInSec = tempo / 60.0;
    double secInBeat = 60.0 / tempo;

    page.push_back(tact(secInBeat, offset));


    for(auto & block : indexNote)
    {
        auto & lastTact = page.back();


        block_note blockNote;

        for(auto & noteIndex : block)
        {
            symbol::SPtr n;
            if(noteIndex != 0) {
                n = std::make_shared<note>(note( noteIndex, offset ));
            } else {
                n = std::make_shared<pause>(pause( offset ));
            }

            blockNote.addNote(n);
        }

        lastTact.setSymbol(std::make_shared<block_note>(blockNote));

        timer += offset;

        if(lastTact.exitRange(timer)) {
            page.back().reorgTact();
            page.push_back(tact(secInBeat, offset));
        }

    }
    page.back().reorgTact();
}

#include <iostream>

void NoteListBuilder::selectionNotes(std::vector<double> & probability)
{
    ushort index = 0;
    indexNote.push_back(std::vector<ushort>());
    auto & blockNote = indexNote.back();
    for(auto& note : probability)
    {
        ++index;
        if(note > NOTEPROBABILITY)
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

void NoteListBuilder::render(std::string & ss)
{
    ss.append("\\relative c'{ ");
    for(auto & tact : page)
    {
        tact.render(ss);
    }
    ss.append(" }");
}
