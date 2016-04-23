//
//  PeakProgramMeter.h
//  Vibrato2Plugin
//
//  Created by Liang Tang on 4/19/16.
//
//

#ifndef __Vibrato2Plugin__PeakProgramMeter__
#define __Vibrato2Plugin__PeakProgramMeter__

#include <stdio.h>
#include <math.h>
#include <algorithm>
#include "ErrorDef.h"

class PeakProgramMeter {
    
public:
    
    static Error_t createInstance (PeakProgramMeter*& pPPM);
    static Error_t destroyInstance (PeakProgramMeter*& pPPM);
    
    Error_t initInstance(float sampleRate, int windowSize, int numChannel);
    Error_t resetInstance();
    
    float getPeak(int channelIdx);
    Error_t ppmProcess (float **ppfInputBuffer, int numOfFrames);
    
private:
    PeakProgramMeter();
    ~PeakProgramMeter();
    
    int m_iNumChannels;
    int m_iWindowSize;
    int m_iHopSize;
    float m_fSampleRate;
    float m_kfAlphaAtt;
    float m_kfAlphaRelease;
    bool m_bIsInitialized;
    float** m_ppfFilterBuf;
    float** m_ppfPeak;
};

#endif /* defined(__Vibrato2Plugin__PeakProgramMeter__) */
