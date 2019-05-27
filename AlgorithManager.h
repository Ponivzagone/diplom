#ifndef ALGORITHMANAGER_H
#define ALGORITHMANAGER_H

#include <QScopedPointer>
#include <list>
#include <NeuroNet/net.h>
#include <NeuroNet/trainingSet.h>
#include <NeuroNet/recognition.h>
#include <memory>
#include <aubio/aubio.h>


#include "AudioFFT.h"
#include "AudioTempo.h"
#include "NoteListBuilder.h"

struct Sample;



class AlgorithManager
{
public:
    AlgorithManager() = delete;
    AlgorithManager(uint _winSize, uint _hopSize, uint _sampleRate);
    ~AlgorithManager();

    void algLoop( std::list< std::shared_ptr< Sample > > & sampleBuffer );

    void algStep( std::shared_ptr< Sample > & sample );





private:


    uint winSize;


    QScopedPointer<AudioFFT> spectrumeAnalyze;
    QScopedPointer<TempoDetecting> beatTracker;
    QScopedPointer<Net> net;
    QScopedPointer<NoteListBuilder> pageBuilder;





};

#endif // ALGORITHMANAGER_H
