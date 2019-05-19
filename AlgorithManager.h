#ifndef ALGORITHMANAGER_H
#define ALGORITHMANAGER_H

#include <QScopedPointer>
#include <list>
#include <memory>
#include <aubio/aubio.h>


#include "AudioFFT.h"
#include "AudioTempo.h"

struct Sample;

class AlgorithManager
{
public:
    AlgorithManager() = delete;
    AlgorithManager(uint_t winSize, uint_t hopSize);
    ~AlgorithManager();

    void algLoop( std::list< std::shared_ptr< Sample > > * sampleBuffer );

    void algStep( std::shared_ptr< Sample > sample );





private:





    QScopedPointer<AudioFFT> spectrumeAnalyze;
    QScopedPointer<TempoDetecting> beatTracker;





};

#endif // ALGORITHMANAGER_H
