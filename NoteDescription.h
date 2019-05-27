#ifndef NOTE_DESCRIPTION_H
#define NOTE_DESCRIPTION_H

#include <list>
#include <memory>

class symbol {
public:
    typedef std::shared_ptr<symbol> SPtr;
    virtual ~symbol() = 0;
    virtual void render() = 0;

};


class duration {
public:
     duration();
     duration(double _time);
    ~duration();
private:
    double time;
};

class sign_alter : public symbol {

public:
    virtual ~sign_alter();
    virtual void render();

};


class pause : public symbol {
public:
    pause();
    pause(const duration & time);
    virtual ~pause();
    virtual void render();
private:
    duration _duration;

};


class note : public symbol {
public:
    note();
    note(const duration & time, ushort ind);
    virtual ~note();
    virtual void render();
private:
    duration _duration;
    ushort index;
    sign_alter _sign_alter;
};

class block_note : public symbol {
public:
    block_note();
    virtual ~block_note();
    virtual void render();

    void addNote(symbol::SPtr note);
private:
    std::list<symbol::SPtr> _notes;
};



#define beatInTact 4

class tact {

public:
    tact(double _secInbeat);
    ~tact();

    void setSymbol(symbol::SPtr sym);



    bool exitRange(double & time);

private:
    std::list<symbol::SPtr> _notes;

    double secInBeat;
    double sizeTact;
};

#endif // NOTE_DESCRIPTION_H
