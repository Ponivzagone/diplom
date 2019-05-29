#ifndef NOTE_DESCRIPTION_H
#define NOTE_DESCRIPTION_H

#include <list>
#include <memory>

class symbol {
public:
    typedef std::shared_ptr<symbol> SPtr;
    virtual ~symbol() = 0;
    virtual void render() = 0;
    virtual double getDuration() = 0;
    virtual double setDuration(double _dur) = 0;

};



class duration {
     duration();
    ~duration();

     static std::pair<int, double> dur[7];
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
    virtual void render();
    virtual double getDuration() const;
    virtual double setDuration(double _dur);

    friend bool operator==(const pause & lhs, const pause & rhs);
    friend bool operator!=(const pause &lhs, const pause &rhs);
private:
    double _duration;

};




class note : public symbol {
public:
    note();
    note(ushort ind, double dur);
    virtual ~note();
    virtual void render();
    virtual double getDuration() const;
    virtual double setDuration(double _dur);

    friend bool operator==(const note & lhs, const note & rhs);
    friend bool operator!=(const note &lhs, const note &rhs);
private:
    double _duration;
    ushort _index;
    sign_alter _sign_alter;

    bool leag;
};




class block_note {
public:
    block_note();
    ~block_note();
    void render();

    void addNote(symbol::SPtr note);
    void merge(std::shared_ptr<block_note> el);

    void durationAlive(std::shared_ptr<block_note> prev, std::shared_ptr<block_note> next);

    ushort noteExits(std::shared_ptr<block_note> block);


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

private:
    std::list< std::shared_ptr<block_note> > _notes;

    double secInBeat;
    double sizeTact;
    double minTime;
};

#endif // NOTE_DESCRIPTION_H
