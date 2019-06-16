#ifndef NOTE_DESCRIPTION_H
#define NOTE_DESCRIPTION_H

#include <list>
#include <memory>
#include <string>


enum StatusNote {
    Attack = 1,
    Sustain = 2,
    Mute = 3
};

class symbol {
public:
    typedef std::shared_ptr<symbol> SPtr;


    virtual ~symbol() = 0;
    virtual double render(std::string & ss) = 0;
    virtual double getDuration() const  = 0;
    virtual void setDuration(double _dur) = 0;

    virtual StatusNote getStatus() = 0;
    virtual void setStatus(StatusNote _stat) = 0;

    virtual ushort getIndex() const = 0;

    friend bool operator==(SPtr lhs, SPtr rhs);
    friend bool operator!=(SPtr lhs, SPtr rhs);

    friend bool operator<(SPtr lhs, SPtr rhs);

};



class duration {
     duration();
    ~duration();

     static std::pair<int, double> dur[6];
public:

    static std::pair<int, double> roundToNear(double time);

    static void initDurtion(double secInBeat, double minTime);

    static std::string render(double time);

};



class sign_alter {

public:
    ~sign_alter();
    void render();

};



class pause : public symbol {
public:
    pause();
    pause(double dur);

    virtual ~pause();
    virtual double render(std::string & ss);
    virtual double getDuration() const;
    virtual void setDuration(double _dur);
    virtual StatusNote getStatus() { return StatusNote::Mute; }

    virtual void setStatus(StatusNote _stat) {}

    virtual ushort getIndex() const;

    friend bool operator==(const pause & lhs, const pause & rhs);
    friend bool operator!=(const pause &lhs, const pause &rhs);
private:
    double _duration;

};




class note : public symbol {
public:
    note();
    note(ushort ind, double dur, StatusNote _stat);

    virtual ~note();
    virtual double render(std::string & ss);
    virtual double getDuration() const;
    virtual void setDuration(double _dur);

    virtual StatusNote getStatus() { return stat; }
    virtual void setStatus(StatusNote _stat) {
        if(_stat == StatusNote::Attack)
        {stat = _stat;}
    }


    void setLeag(ushort i);

    virtual ushort getIndex() const;

    friend bool operator==(const note & lhs, const note & rhs);
    friend bool operator!=(const note &lhs, const note &rhs);
private:

    bool inTonal();
    char * nameEncode();

    double _duration;
    ushort _index;
    sign_alter _sign_alter;

    StatusNote stat;

    bool leag;
};




class block_note {
public:
    block_note();
    ~block_note();
    void render(std::string &ss);

    void addNote(symbol::SPtr note);
    void merge(std::shared_ptr<block_note> el);

    void durationAlive(std::shared_ptr<block_note> prev, std::shared_ptr<block_note> next);

    ushort noteExits(std::shared_ptr<block_note> block);

    bool empthy();

    bool checkStat()
    {
        bool j = false;
        for(auto & n :_notes)
        {
            if(n.get()->getStatus() == StatusNote::Sustain) j = true;
        }
        return j;
    }


    friend bool operator==(const block_note & lhs, const block_note & rhs);

private:
    std::list<symbol::SPtr> _notes;
};



#define beatInTact 4

class tact {

public:
    tact(double _secInbeat, double _minTime);
    ~tact();

    void setSymbol(std::shared_ptr<block_note> sym);

    void reorgTact();

    bool exitRange(double & time);

    void render(std::string &ss);

private:
    std::list< std::shared_ptr<block_note> > _notes;

    double secInBeat;
    double sizeTact;
    double minTime;
};

#endif // NOTE_DESCRIPTION_H
