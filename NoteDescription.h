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
    ~duration() {}

};

class sign_alter : public symbol {

public:
    virtual ~sign_alter();
    virtual void render();

};


class pause : public symbol {
public:
    pause();
    virtual ~pause();
    virtual void render();
private:
    duration _duration;

};


class note : public symbol {
public:
    note();
    virtual ~note();
    virtual void render();
private:
    duration _duration;
    sign_alter _sign_alter;
};

class block_note : public note {
public:
    block_note();
    virtual ~block_note();
    virtual void render();
private:
    std::list<note> _notes;
};


class tact {

public:
    tact();

    ~tact();
private:
    std::list<symbol::SPtr> _notes;


};

#endif // NOTE_DESCRIPTION_H
