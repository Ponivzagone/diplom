#ifndef FINDNOTES_H
#define FINDNOTES_H

#include <QVector>
#include <fstream>

class findnotes
{
public:
    findnotes();
    ~findnotes();
    void find(double *, int);
private:
    QVector<bool> notesOld;
    std::ofstream fout;
};

#endif // FINDNOTES_H
