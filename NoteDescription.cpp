#include "NoteDescription.h"

//symbol::symbol()
//{

//}

symbol::~symbol() {

}





sign_alter::~sign_alter() {

}

void sign_alter::render()
{

}





pause::pause() {

}

pause::pause(const duration & time)
    : _duration(time)
{

}

pause::~pause() {

}

void pause::render()
{

}





duration::duration()
{

}

duration::duration(double _time)
    : time(_time)
{

}

duration::~duration()
{

}




note::note()
{

}

note::note(const duration & time, ushort ind)
    : _duration(time), index(ind)
{

}

note::~note() {

}

void note::render()
{

}





block_note::block_note()
{

}

block_note::~block_note() {

}

void block_note::render()
{

}

void block_note::addNote(symbol::SPtr note)
{
    _notes.push_back(note);
}




tact::tact(double _secInbeat)
    : secInBeat(_secInbeat)
{
    sizeTact = secInBeat * beatInTact;
}

tact::~tact()
{

}

void tact::setSymbol(symbol::SPtr sym)
{
    _notes.push_back(sym);
}

bool tact::exitRange(double & time)
{
    if(sizeTact < time){ time -= sizeTact; return true;}
    return false;
}


