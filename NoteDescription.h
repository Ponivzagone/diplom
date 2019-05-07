#ifndef NOTE_DESCRIPTION_H
#define NOTE_DESCRIPTION_H

#include <list>


class symbol {
public:
    virtual ~symbol() = 0;

};


class duration {
public:
    ~duration() {}

};

class sign_alter : public symbol {

public:
    ~sign_alter();

};


class pause : public symbol {
public:
    pause();
    ~pause();
private:
    duration _duration;

};


class note : public symbol {
public:
    note();
    ~note();

private:
    duration _duration;
    sign_alter _sign_alter;
};

class block_note : public note {
public:
    block_note();
    ~block_note();

private:
    std::list<note> _notes;
};


class tact {

public:
    tact();

private:

    std::list<symbol *> _notes;
};

#endif // NOTE_DESCRIPTION_H
