//
//  PeakProgramMeter.cpp
//  Vibrato2Plugin
//
//  Created by Music Technology on 4/19/16.
//
//

#include "PeakProgramMeter.h"

PeakProgramMeter::PeakProgramMeter() {
    m_bIsInitialized = false;
    m_fSampleRate = 44100;
    m_iNumChannels = 0;
}

PeakProgramMeter::~PeakProgramMeter() {
    this -> resetInstance();
}

Error_t PeakProgramMeter::createInstance (PeakProgramMeter*& pPPM) {
    pPPM = new PeakProgramMeter();
    if (!pPPM) {
        return kUnknownError;
    }
    return kNoError;
}

Error_t PeakProgramMeter::destroyInstance(PeakProgramMeter*& pPPM) {
    if (!pPPM) {
        return kUnknownError;
    }
    pPPM -> resetInstance();
    delete pPPM;
    pPPM = 0;
    return kNoError;
}

Error_t PeakProgramMeter::initInstance(float sampleRate = 44100,int windowSize = 4096,  int numChannel = 1) {
    //set parameters
    m_fSampleRate    = sampleRate;
    m_iNumChannels   = numChannel;
    m_iWindowSize    = windowSize;
    m_fFilterBuf     = 0.f;
    m_kfAlphaAtt     = 1.f - (exp(-2.2f / (m_fSampleRate*.01f)));
    m_kfAlphaRelease = 1.f - (exp(-2.2f / (m_fSampleRate*1.5f)));
    if (windowSize % 2 == 0) {
        m_iHopSize     = windowSize / 2;
    } else {
        return kFunctionInvalidArgsError;
    }
    m_bIsInitialized = true;
    
    return kNoError;
}

Error_t PeakProgramMeter::resetInstance() {
    m_iNumChannels = 0;
    m_bIsInitialized = false;
    
    return kNoError;
}


//Error_t PeakProgramMeter::process (const float **ppfInputBuffer, float **ppfOutputBuffer, int numOfFrames) {
//    
//    if (!ppfInputBuffer || !ppfOutputBuffer) {
//        return kFunctionInvalidArgsError;
//    }
//    int numBlocks = ceilf((float)numOfFrames / m_iWindowSize);
//    
//    for (int i = 0; i < numBlocks; i++) {
//        for (int j = 0; j < m_iNumChannels; j++) {
//            int i_start = i * m_iHopSize;
//            int i_stop  = std::min(numOfFrames, i_start + m_iWindowSize);
//            for (int k = i_start; k < i_stop; k++) {
////                ppmProcess(ppfInputBuffer[i][k], ppfOutputBuffer[i][k], numOfFrames, ppfOutputBuffer[j][m_iHopSize]);
//            }
//        }
//    }
//    
//    return kNoError;
//}

Error_t PeakProgramMeter::ppmProcess(const float **ppfInputBuffer, float peakOfCurrentBlk, int numOfFrames) {
    float filterBuf;
    float ppmOut = 0.f;
    for (int i = 0; i < numOfFrames; i++) {
        for (int j = 0 ; j < m_iNumChannels; j++) {
            filterBuf = m_fFilterBuf;
            if (filterBuf > fabsf(ppfInputBuffer[j][i])) {
                ppmOut = (1 - m_kfAlphaRelease) * filterBuf;
            } else {
                ppmOut = m_kfAlphaAtt * fabsf(ppfInputBuffer[j][i]) + (1 - m_kfAlphaAtt) * filterBuf;
            }
            m_fFilterBuf = ppmOut;
            if (ppmOut > peakOfCurrentBlk) {
                peakOfCurrentBlk = ppmOut;
            }
        }
    }
    peakOfCurrentBlk = 20 * log10f(peakOfCurrentBlk);
    return kNoError;
}