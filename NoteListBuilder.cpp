#include "NoteListBuilder.h"
#include <algorithm>

#include <settings/config_reader.h>


NoteListBuilder::NoteListBuilder(uint _sampleRate, uint _winSize, uint _hopSize)
    :  winSize(_winSize), hopSize(_hopSize), sampleRate(_sampleRate), currentSample(0), timer(0.0)
{
    indexNote.reserve(100);
    HzNote.reserve(88);
    noteHzInit();
    offset = static_cast<double>(hopSize) / static_cast<double>(sampleRate);
    stepSpectr = static_cast<double>(sampleRate) / static_cast<double>(winSize);
    NoteProbability = ConfigReader::instance().getValue<double>(CoreSettings::note_prob, 0.7);
    noteChange = winSize / hopSize;

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

        timer += offset;

        if(page.back().exitRange(timer)) {
            page.back().reorgTact();
            page.push_back(tact(secInBeat, offset));
        }

        auto & lastTact = page.back();

        block_note blockNote;

        for(auto & noteIndex : block)
        {
            symbol::SPtr n;
            if(noteIndex.first != 0) {
                n = std::make_shared<note>(note( noteIndex.first, offset, noteIndex.second ));
            } else {
                n = std::make_shared<pause>(pause( offset ));
            }

            blockNote.addNote(n);
        }

        lastTact.setSymbol(std::make_shared<block_note>(blockNote));

    }
    page.back().reorgTact();
}

#include <math.h>

void NoteListBuilder::noteHzInit()
{
    for (double i = 0.0; i < 88.0; ++i) {
           double hz = 440.0 * std::pow(2.0, (i - 48.0) / 12.0);
           HzNote.emplace_back(hz);
    }
}

std::pair<ushort, StatusNote> NoteListBuilder::initStatus(ushort index, float amp)
{
    for(auto & nn : statusN.n)
    {
       if(nn.first == index)
       {
            if(nn.second <  amp)
            {
                return std::pair<ushort, StatusNote>(index, StatusNote::Attack);
            }
            return std::pair<ushort, StatusNote>(index, StatusNote::Sustain);
       }
    }

    return std::pair<ushort, StatusNote>(index, StatusNote::Attack);
}

#include <iostream>


bool compareNote(const std::pair<ushort, StatusNote> & lhs, const std::pair<ushort, StatusNote> & rhs)
{
    return lhs.first < rhs.first;
}

void NoteListBuilder::selectionNotes(std::vector<double> & probability, AudioFFT * fft)
{
    ushort index = 0;

    indexNote.push_back(std::vector<std::pair<ushort, StatusNote > >());
    auto & blockNote = indexNote.back();

    float * screenSpectr = fft->getNorm();

    for(auto& note : probability)
    {
        ++index;
        if(note > NoteProbability)
        {

            double hz = HzNote.at(index - 1);
            int offset = hz / stepSpectr;

            std::pair<ushort, StatusNote > note = initStatus(index, screenSpectr[offset]);
            blockNote.emplace_back(note);
        }
    }
    statusN.zero();
    std::sort(blockNote.begin(), blockNote.end(), compareNote);
    if(blockNote.empty())
    {
        blockNote.push_back(std::pair<ushort, StatusNote >(0, StatusNote::Mute));
        return;
    }


    for(auto& note : blockNote)
    {
        double hz = HzNote.at(note.first - 1);
        int offset = hz / stepSpectr;
        statusN.setAmp(note.first, screenSpectr[offset]);
    }
}

void NoteListBuilder::render(std::string & ss)
{
    ss.append("{ ");
    for(auto & tact : page)
    {
        tact.render(ss);
    }
    ss.append(" }");
}
