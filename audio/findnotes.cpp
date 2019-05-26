#include "FindNotes.h"

findnotes::findnotes()
{
    for(int i = 0; i < 1024; i++)
    {
        notesOld.push_back(0);
    }

    fout.open("/home/genri/Net/database/noteDataBaseC2.txt");
}

findnotes::~findnotes()
{
     fout.close();
}


void findnotes::find(double * amplitude, int range)
{
    bool chek = false;
    double attenuation = 400;
    for(int i = 1; i < range / 2; ++i)
    {
        if(amplitude[i] > attenuation)
        {
            chek = true;
            break;
        }
    }
    if(chek)
    {
        fout << "in:";
        for(int i = 0; i < range / 2; ++i)
        {
            fout << " " << amplitude[i];
        }
        fout << std::endl;
        fout << "out:";
        for(int i = 1; i <= 88; ++i)
        {
            if(i == 52)fout << " 1";
            else fout << " 0";
        }
        fout << std::endl;
    }
}
