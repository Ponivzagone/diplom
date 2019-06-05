
#include "NoteDescription.h"
#include <iterator>
#include <sstream>
#include <limits>
#include <algorithm>

bool equals(const double & A, const double & B)
{
    return (std::abs(A - B) < std::numeric_limits<double>::epsilon());
}


symbol::~symbol() {

}


bool operator==(symbol::SPtr lhs, symbol::SPtr rhs)
{
    if(lhs->getIndex() != rhs->getIndex()) { return false; }
    //if( !equals( lhs->getDuration(), rhs->getDuration() ) ) { return false; }
    return true;
}

bool operator!=(symbol::SPtr lhs, symbol::SPtr rhs)
{
    if(lhs == rhs) { return false; }
    return true;
}

bool operator<(symbol::SPtr lhs, symbol::SPtr rhs)
{
    if(lhs->getIndex() < rhs->getIndex()) { return true; }
    return false;
}


bool comp(symbol::SPtr lhs, symbol::SPtr rhs)
{
    if(lhs->getIndex() < rhs->getIndex()) { return true; }
    return false;
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
    for(int i = 0; i < 4; ++i)
    {
        if(  std::abs(time - dur[i].second)
             <
           ( dur[i + 1].second - dur[i].second ) / LENGTHTONEXTDUR )
        {
            return dur[i];
        }
    }
    return dur[4];
}

void duration::initDurtion(double tactSize, double minTime)
{
    int k = 0;
    dur[k++] = std::make_pair(0, minTime);
    //dur[k++] = std::make_pair(16, tactSize / 16);
    dur[k++] = std::make_pair(8, tactSize / 8);
    dur[k++] = std::make_pair(4, tactSize / 4);
    dur[k++] = std::make_pair(2, tactSize / 2);
    dur[k++] = std::make_pair(1, tactSize);
}

std::string duration::render(double time)
{
    std::stringstream ss;
    for(int i = 0; i < 6; ++i)
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

double pause::render(std::string & ss)
{
    if(!duration::roundToNear(_duration).first)
    {
        return 0.0;
    }

    ss.append( "r");
    return duration::roundToNear(_duration).first;
}

double pause::getDuration() const
{
    return _duration;
}

void pause::setDuration(double _dur)
{
    _duration = _dur;
}

ushort pause::getIndex() const
{
    return 0;
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

double note::render(std::string & ss)
{
    if(!duration::roundToNear(_duration).first)
    {
        return 0.0;
    }
    ss.append(this->nameEncode());
    if(_index > 39)
    {
        ss.append( "'");
    }
    if(_index > 51)
    {
        ss.append( "'");
    }
    if(!this->inTonal())
    {
        ss.append("es");
    }
    ss.append(" ");
    return duration::roundToNear(_duration).first;
}

bool note::inTonal()
{
    static uint tonMask[] = {1,3,4,6,8,9,11}; // C_Major
    if(std::binary_search(tonMask, tonMask + 7, _index % 12)) return true;
    return false;
}

char * note::nameEncode()
{
    int i = _index % 12;
    char * a;
    switch(i)
    {
        case 0:
            a = "g";
        break;
        case 1:
            a = "a";
        break;
        case 2:
            a = "a";
        break;
        case 3:
            a = "b";
        break;
        case 4:
            a = "c";
        break;
        case 5:
            a = "c";
        break;
        case 6:
            a = "d";
        break;
        case 7:
            a = "d";
        break;
        case 8:
            a = "e";
        break;
        case 9:
            a = "f";
        break;
        case 10:
            a = "f";
        break;
        case 11:
            a = "g";
        break;
    }

    return a;
}

double note::getDuration() const
{
    return _duration;
}

ushort note::getIndex() const
{
    return _index;
}

void note::setDuration(double _dur)
{
    _duration = _dur;
}

void note::setLeag(ushort i)
{
    leag = i;
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

void block_note::render(std::string &ss)
{
    int dur = 0;
    int first;
    if(_notes.size() != 1)
        ss.append("<");
    for(auto & sym: _notes)
    {
        first = duration::roundToNear(sym->getDuration()).first;
        if(dur == 0) { dur = first; }
        if(first != 0 && dur == first) { sym->render(ss); }
    }
    if(_notes.size() != 1)
        ss.append(">");
    if(dur != 0 )
        {
        std::stringstream s;
        s << dur;
        ss.append(s.str());
        ss.append(" ");
    }
}

void block_note::addNote(symbol::SPtr note)
{
    _notes.push_back(note);
}

#include <iostream>

void block_note::merge(std::shared_ptr<block_note> el)
{
    auto it = _notes.begin();
    std::list<symbol::SPtr> & elL = el.get()->_notes;
    for(auto itE = elL.begin(); itE != elL.end(); ++itE)
    {
        //std::cout << "NEXT IND:" << it->get()->getIndex() << " DUR:" << it->get()->getDuration() << std::endl;
        //std::cout << "MERGE IND:" << itE->get()->getIndex() << " DUR:" << itE->get()->getDuration() << std::endl;

        it->get()->setDuration(it->get()->getDuration() + itE->get()->getDuration());
        it = std::next(it, 1);
       // std::cout << " END MERGE!" << std::endl;
    }
    return;
}

#include <iterator>
#include <map>


void block_note::durationAlive(std::shared_ptr<block_note> prev, std::shared_ptr<block_note> next)
{
    auto itE = _notes.end();


    // Отвратный алгоритммм
    if(prev && next)
    {
        for(auto it = _notes.begin(); it != itE; )
        {
            if( !(duration::roundToNear(it->get()->getDuration()).first) )
            {

                if(!it->get()->getIndex()) //ты огрызок паузы тебя надо засунуть во все что спереди или сзади // одно типный код ван лавв (придурок)
                {

                    double dur = it->get()->getDuration() / 2;
                    it->get()->setDuration(0.0);
                    auto & PB = prev->_notes;
                    for(auto itS : PB)
                    {
                        itS->setDuration(itS->getDuration() + dur );
                    }


                    auto & NB = next->_notes;
                    for(auto itS : NB)
                    {
                        itS->setDuration(itS->getDuration() + dur );

                    }
                    it = _notes.erase(it);
                    continue;
                }
            }
            ++it;
        }



        std::multimap<int, symbol::SPtr > notFind;
        short check = 0;
        for (auto it = _notes.begin(); it != itE;it++)
        {
            if( !(duration::roundToNear(it->get()->getDuration()).first) )
            {
                {
                    auto & PB = prev->_notes;
                    auto f = std::find(PB.begin(), PB.end(), *it);
                    if(f != PB.end()) { notFind.insert(std::pair<int, symbol::SPtr>(f->get()->getIndex(),*f)); check |= 1; }
                }

                {
                    auto & NB = next->_notes;
                    auto f = std::find(NB.begin(), NB.end(), *it);
                    if(f != NB.end()) {notFind.insert(std::pair<int, symbol::SPtr>(f->get()->getIndex(),*f)); check |= 2; }
                }
            }
        }

        if(check & 1 && check & 2)
        {
            for (auto it = _notes.begin(); it != itE;it++)
            {

                auto f = notFind.equal_range(it->get()->getIndex());
                for (auto i = f.first; i != f.second; ++i)
                {
                    double dur = it->get()->getDuration() / 2;
                    i->second.get()->setDuration(i->second.get()->getDuration() + dur );
                }
                it->get()->setDuration(0.0);
            }
        } else if(check & 1 || check & 2)
        {
            for (auto it = _notes.begin(); it != itE;it++)
            {

                auto f = notFind.equal_range(it->get()->getIndex());
                for (auto i = f.first; i != f.second; ++i)
                {
                    double dur = it->get()->getDuration();
                    i->second.get()->setDuration(i->second.get()->getDuration() + dur );
                }
                it->get()->setDuration(0.0);
            }
        }

    }
    else {
        for(auto it = _notes.begin(); it != itE; )
        {
            if( !(duration::roundToNear(it->get()->getDuration()).first) )
            {
                //если ты огрызок то тебя надо к чему то присунуть назад желательно если нету них то вперед его время пихать

                if(!it->get()->getIndex()) //ты огрызок паузы тебя надо засунуть во все что спереди или сзади // одно типный код ван лавв (придурок)
                {
                    if(prev)
                    {
                        auto & PB = prev->_notes;
                        for(auto itS : PB)
                        {
                            itS->setDuration(itS->getDuration() + it->get()->getDuration());
                        }
                        it->get()->setDuration(0.0);
                    }
                    else
                    {
                        auto & NB = next->_notes;
                        for(auto itS : NB)
                        {
                            itS->setDuration(itS->getDuration() + it->get()->getDuration());

                        }
                        it->get()->setDuration(0.0);
                    }
                    it = _notes.erase(it);
                    continue;
                }
                else //ты огрызок ноты тебя надо засунуть к такой же ноте либо спереди либо сзади
                {

                    // когда на вход блоки и справа и слева передаются надо отделюную ветку прописать
                    // найти для предыдущего то чего  не хватает  а если там больше ?? помоему все идет по пизде
                    // как откусить от срединного блока по половине в каждую сторону  при условии что есть хоть по одной ноте  не обзательно одинаковой что были с обоих сторон повторяющимися
                    // если нету то отдай все в предыдущее

                    if(prev)
                    {
                        auto & NB = prev->_notes;
                        for(auto itS : NB)
                        {
                            if(*it == itS )
                            {
                                itS.get()->setDuration(itS.get()->getDuration() +  it->get()->getDuration());
                                it->get()->setDuration(0.0);
                            }
                        }
                        it = _notes.erase(it);
                        continue;
                    }
                    else
                    {
                        auto & NB = next->_notes;
                        for(auto itS : NB)
                        {
                            if(*it == itS )
                            {
                                itS.get()->setDuration(itS.get()->getDuration() + it->get()->getDuration());
                                it->get()->setDuration(0.0);
                            }
                        }
                        it = _notes.erase(it);
                        continue;
                    }

                }
            }
            else // ты не огрызок ты личностьтебя надо либо залиговать с нужной нотой либо кайфую ты пауза
                 // Хотяя хмм а если ты одиночная нота пока оставлю на подумать  тут надо с силой удара разбираться типа
                 // был ли всплеск либо нет(! надо спектр хранить нужной ноты в этот момент времени и смотреть на предыдущий
                 // снимок амплитуда упала значит падаем либо где завести поле амплитуда растет или нет если нет то смело лигуем а растет значит оставляем
                 // об этом чуть позжа
            {
    //            if(it->get()->getIndex())
    //            {
    //                if(prev)
    //                {
    //                    auto & PB = prev->_notes;
    //                    for(auto itS : PB)
    //                    {
    //                        if(*it == itS)
    //                        {
    //                            note * nn =  dynamic_cast<note *>(itS.get());
    //                            nn->setLeag(1);
    //                        }
    //                    }

    //                }
    //                else
    //                {
    //                    auto & NB = next->_notes;
    //                    for(auto itS : NB)
    //                    {
    //                        if(*it == itS)
    //                        {

    //                        }
    //                    }
    //                }
    //            }
            }
            ++it;
        }
    }


    for (auto it = _notes.begin(); it != _notes.end();) {
        if(it->get()->getDuration() == 0.0)
        {
            it  = _notes.erase(it);
        }
        else {
            ++it;
        }
    }
}

ushort block_note::noteExits(std::shared_ptr<block_note> block)
{

}

bool block_note::empthy()
{
    return _notes.empty();
}

bool operator==(const block_note &lhs, const block_note &rhs)
{

    //TODO: вынеси сравнение ноты и пазуы в символ и сделай виртуальный метод получения indexa у паузы 0 у ноты индекс


    if(lhs._notes.size() != rhs._notes.size()) return false;

    auto iL = lhs._notes.begin();
    auto eL = lhs._notes.end();
    auto iR = rhs._notes.begin();
    auto eR = rhs._notes.end();
    for( ; iL != eL , iR != eR; ++iL, ++iR )
    {
        if(*iL != *iR) { return false; }
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

#include <iostream>

void tact::reorgTact()
{
    if(_notes.empty()) { return; }

    duration::initDurtion(sizeTact, minTime);

   // std::cout << "SIZE:" << _notes.size() << std::endl;

    auto end = _notes.end();
    for(auto it = _notes.begin(); it != end;)
    {
        auto itN = std::next(it, 1);
        if(itN == end) { break; }

        if(*(it->get()) == *(itN->get()))
        {   
            itN->get()->merge(*it);
            it = _notes.erase(it);
        } else {
            ++it;
        }
     //   std::cout << "SIZE:" << _notes.size() << std::endl;
    }

    for(auto it = _notes.begin(); it != end; )
    {

        auto itN = std::next(it, 1);
        auto itP = std::prev(it, 1);

        if(itP == end && itN != end)
        {
            it->get()->durationAlive(nullptr, *itN);
            if(it->get()->empthy()) { it = _notes.erase(it); continue; }
            ++it; continue;
        }
        if(itN == end && itP != end)
        {
            it->get()->durationAlive(*itP, nullptr);
            if(it->get()->empthy()) { it = _notes.erase(it); continue; }
            ++it; continue;
        }

        if(itP == end && itN == end) { break; }

        it->get()->durationAlive(*itP, *itN);

        if(it->get()->empthy()) { it = _notes.erase(it); continue; }
        ++it;

    }
}

bool tact::exitRange(double & time)
{
    if(sizeTact < time)
    {
        time -= sizeTact;
        return true;
    }
    return false;
}

#include <iostream>

void tact::render(std::string &ss)
{
    duration::initDurtion(sizeTact, minTime);

    for(auto & note : _notes )
    {
        note->render(ss);
    }
    ss.append( " | ");
}


