//
//  PeakProgramMeter.cpp
//  Vibrato2Plugin
//
//  Created by Liang Tang on 4/19/16.
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
    delete pPPM;
    pPPM = 0;

    return kNoError;
}

Error_t PeakProgramMeter::initInstance(float sampleRate = 44100,int windowSize = 4096,  int numChannel = 1) {
    //set parameters
    m_fSampleRate    = sampleRate;
    m_iNumChannels   = numChannel;
    m_iWindowSize    = windowSize;
    m_ppfFilterBuf   = new float*[numChannel];
    m_ppfPeak        = new float*[numChannel];
    for (int i = 0; i < m_iNumChannels; i++) {
        m_ppfFilterBuf[i] = new float [1];
        m_ppfPeak[i]      = new float [1];
    }
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
    
    for (int i = 0; i < m_iNumChannels; i++) {
        delete [] m_ppfFilterBuf[i];
        delete [] m_ppfPeak[i];
    }
    delete [] m_ppfFilterBuf;
    delete [] m_ppfPeak;
    
    return kNoError;
}

Error_t PeakProgramMeter::ppmProcess(float **ppfInputBuffer, int numOfFrames) {
    float filterBuf = 0.f;
    float ppmOut = 0.f;
    float tempPeak = 0.f;
    for (int i = 0 ; i < m_iNumChannels; i++) {
        for (int j = 0 ; j < numOfFrames; j++) {
            filterBuf = m_ppfFilterBuf[i][0];
            if (filterBuf > fabsf(ppfInputBuffer[i][j])) {
                ppmOut = (1 - m_kfAlphaRelease) * filterBuf;
            } else {
                ppmOut = m_kfAlphaAtt * fabsf(ppfInputBuffer[i][j]) + (1 - m_kfAlphaAtt) * filterBuf;
            }
            m_ppfFilterBuf[i][0] = ppmOut;
            if (ppmOut > tempPeak) {
                tempPeak = ppmOut;
            }
        }
        m_ppfPeak[i][0] = tempPeak;
    }
    
    return kNoError;
}

float PeakProgramMeter::getPeak(int channelIdx) {
    float peak = 0.f;
    peak = m_ppfPeak[channelIdx][0];
    return peak;
}