#include "NoteDescription.h"
#include <sstream>
#include <iterator>

symbol::~symbol() {

}





sign_alter::~sign_alter() {

}

void sign_alter::render()
{

}


std::pair<int,double> duration::dur[];


duration::duration()
{

}
duration::~duration()
{

}

std::pair<int, double> duration::roundToNear(double time)
{
    if(time < dur[0].second) return dur[0];
    for(int i = 0; i < 6; ++i)
    {
        if(  std::abs(time - dur[i].second)
             <
           ( dur[i + 1].second - dur[i].second ) / 2 )
        {
            return dur[i];
        }
    }
    return dur[6];
}

void duration::initDurtion(double tactSize, double minTime)
{
    int k = 0;
    dur[k++] = std::make_pair(0, minTime);
    dur[k++] = std::make_pair(32, tactSize / 32);
    dur[k++] = std::make_pair(16, tactSize / 16);
    dur[k++] = std::make_pair(4, tactSize / 4);
    dur[k++] = std::make_pair(8, tactSize / 8);
    dur[k++] = std::make_pair(2, tactSize / 2);
    dur[k++] = std::make_pair(1, tactSize);
}

#include <limits>
bool equals(const double & A, const double & B)
{
    return (std::abs(A - B) < std::numeric_limits<double>::epsilon());
}

std::string duration::render(double time)
{
    std::stringstream ss;
    for(int i = 0; i < 7; ++i)
    {
        if( equals( time, dur[i].second ) )
        {
            ss << dur[i].first;
            return ss.str();
        }
    }
}



pause::pause() {

}

pause::pause(double dur)
    : _duration(dur)
{

}

pause::~pause() {

}

void pause::render()
{

}

double pause::getDuration() const
{
    return _duration;
}

double pause::setDuration(double _dur)
{
    _duration = _dur;
}

bool operator==(const pause &lhs, const pause &rhs)
{
    if( !equals( lhs._duration, rhs._duration ) ) { return false; }
    return true;
}

bool operator!=(const pause &lhs, const pause &rhs)
{
    if(lhs == rhs) { return false; }
    return true;
}



note::note()
{

}

note::note(ushort ind, double dur)
    : _duration(dur), _index(ind)
{

}

note::~note() {

}

void note::render()
{

}

double note::getDuration() const
{
    return _duration;
}

double note::setDuration(double _dur)
{
    _duration = _dur;
}

bool operator==(const note &lhs, const note &rhs)
{
    if(lhs._index != rhs._index) { return false; }
    if( !equals( lhs._duration, rhs._duration ) ) { return false; }
    return true;
}

bool operator!=(const note &lhs, const note &rhs)
{
    if(lhs == rhs) { return false; }
    return true;
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

void block_note::merge(std::shared_ptr<block_note> el)
{
    auto it = _notes.begin();
    std::list<symbol::SPtr> & elL = el.get()->_notes;
    for(auto itE = elL.begin(); itE != elL.end(); ++itE)
    {

    }
    return;
}

bool operator==(const block_note &lhs, const block_note &rhs)
{

    if(lhs._notes.size() != rhs._notes.size()) return false;

    auto iL = lhs._notes.begin();
    auto eL = lhs._notes.end();
    auto iR = rhs._notes.begin();
    auto eR = rhs._notes.end();
    for( ;iL != eL , iR != eR; ++iL, ++iR )
    {

        {
            auto * pl = dynamic_cast<note * >(iL->get());
            auto * pr = dynamic_cast<note * >(iR->get());

            if(pl && pr)
            {
                if(*pl != *pr) { return false; }
                continue;
            }
        }
        {
            auto * pl = dynamic_cast<pause * >(iL->get());
            auto * pr = dynamic_cast<pause * >(iR->get());

            if(pl && pr)
            {
                if(*pl != *pr) { return false; }
                continue;
            }
        }
        return false;
    }
    return true;
}




tact::tact(double _secInbeat, double _minTime)
    : secInBeat(_secInbeat), minTime(_minTime)
{
    sizeTact = secInBeat * beatInTact;
}

tact::~tact()
{

}

void tact::setSymbol(std::shared_ptr<block_note> sym)
{
    _notes.push_back(sym);
}

void tact::reorgTact()
{
    if(_notes.empty()) { return; }

    duration::initDurtion(sizeTact, minTime);

    auto end = _notes.end();
    for(auto it = _notes.begin(); it != end;)
    {
        auto itN = std::next(it, 1);
        if(itN == end) { continue; }

        if(*it == *itN)
        {
            itN->get()->merge(*it);
            it = _notes.erase(it);
        } else {
            ++it;
        }


    }
}

bool tact::exitRange(double & time)
{
    if(sizeTact < time){ time -= sizeTact; return true;}
    return false;
}


