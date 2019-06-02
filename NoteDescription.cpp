
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

void pause::render(std::string & ss)
{
    ss.append( "r");
    std::stringstream s;
    s << duration::roundToNear(_duration).first;
    ss.append( s.str());
    ss.append( " ");
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

void note::render(std::string & ss)
{
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
    std::stringstream s;
    s << duration::roundToNear(_duration).first;
    ss.append( s.str());
    ss.append(" ");
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
    //ss.append("<");
    for(auto & sym: _notes)
    {
        sym->render(ss);
    }
   // ss.append(">");
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

                if(next)
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

                std::map<int, symbol::SPtr> indLost;
                bool check = false;
                if(prev)
                {
                    auto & PB = prev->_notes;
                    for(auto gav : _notes)
                    {
                        auto iii = std::find(PB.begin(), PB.end(), gav);
                        if(iii == PB.end())
                        {
                            indLost[gav->getIndex()] = gav;
                        }
                    }
                } else {
                    auto & NB = next->_notes;
                    for(auto itS : NB)
                    {
                        if(*it == itS )
                        {
                            double jopa = it->get()->getDuration();
                            itS.get()->setDuration(itS.get()->getDuration() + jopa);
                            it->get()->setDuration(0.0);
                        }
                    }
                    it = _notes.erase(it);
                    continue;
                }

                if(next)
                {
                    auto & NB = next->_notes;
                    for(auto gav : _notes)
                    {
                        auto iii = std::find(NB.begin(), NB.end(), gav);
                        if(iii != NB.end() && indLost.find(iii->get()->getIndex()) != indLost.end())
                        {
                            check = true;
                        }
                    }
                }

                for(auto & iM: indLost)
                {
                    double jopa = it->get()->getDuration();
                    if(check)
                    {
                        jopa = jopa / _notes.size();
                    }

                    iM.second->setDuration(iM.second->getDuration() + jopa);
                    it->get()->setDuration(0.0);
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


