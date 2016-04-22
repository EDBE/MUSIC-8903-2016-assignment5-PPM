#include "MUSI8903Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>
#include <iostream>

#include "UnitTest++.h"

#include "Synthesis.h"
#include "Vector.h"

#include "Vibrato.h"
//#include "ErrorDef.h"
#include "PeakProgramMeter.h"

SUITE(Vibrato)
{
    struct VibratoData
    {
        VibratoData()  :
            m_pVibrato(0),
            m_ppfInputData(0),
            m_ppfOutputData(0),
            m_kiDataLength(35131),
            m_fMaxModWidth(.5F),
            m_iBlockLength(512),
            m_iNumChannels(1),
            m_fSampleRate(44100),
            m_fModWidth(.1F),
            m_fModFreq(1.F),
        
            m_iNumOfBlocks(m_kiDataLength /m_iBlockLength + 1),
            m_pPPM(0),
            m_ppfPeakValue(0),
            m_ppfReference(0)
        {
//            CVibrato::createInstance(m_pVibrato);
//            m_pVibrato->initInstance(m_fMaxModWidth,m_fSampleRate,m_iNumChannels);
            
            PeakProgramMeter::createInstance(m_pPPM);
            m_pPPM->initInstance(m_fSampleRate, m_iBlockLength, m_iNumChannels);

            m_ppfInputData = new float*[m_iNumChannels];
            m_ppfOutputData= new float*[m_iNumChannels];
            m_ppfInputTmp  = new float*[m_iNumChannels];
            m_ppfOutputTmp = new float*[m_iNumChannels];
            m_ppfPeakValue = new float*[m_iNumChannels];
            m_ppfReference = new float*[m_iNumChannels];
            
            for (int i = 0; i < m_iNumChannels; i++)
            {
                m_ppfInputData[i] = new float [m_kiDataLength];
                CSynthesis::generateSine(m_ppfInputData[i], 800, m_fSampleRate, m_kiDataLength, .6F, 0);
                
                m_ppfOutputData[i] = new float [m_kiDataLength];
                CVector::setZero(m_ppfOutputData[i], m_kiDataLength);
                
                m_ppfPeakValue[i] = new float [m_iNumOfBlocks];
                CVector::setZero(m_ppfPeakValue[i], m_iNumOfBlocks);  //There was a bug here too. it was just m_ppfPeakValue
                
                m_ppfReference[i] = new float [m_iNumOfBlocks];
                CVector::setZero(m_ppfReference[i], m_iNumOfBlocks); //likewise
            }
            
        }

        ~VibratoData() 
        {
            for (int i = 0; i < m_iNumChannels; i++)
            {
                delete [] m_ppfOutputData[i];
                delete [] m_ppfInputData[i];
                delete [] m_ppfPeakValue[i];
                delete [] m_ppfReference[i];
            }
            delete [] m_ppfOutputTmp;
            delete [] m_ppfInputTmp;
            delete [] m_ppfOutputData;
            delete [] m_ppfInputData;
            delete [] m_ppfPeakValue;
            delete [] m_ppfReference;

            CVibrato::destroyInstance(m_pVibrato);
            
            PeakProgramMeter::destroyInstance(m_pPPM);
        }

        void process ()
        {
            int iNumFramesRemaining = m_kiDataLength;
            int counter = 0;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);

                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c] = &m_ppfInputData[c][m_kiDataLength - iNumFramesRemaining];
                    m_ppfOutputTmp[c] = &m_ppfOutputData[c][m_kiDataLength - iNumFramesRemaining];
                }
//                m_pVibrato->process(m_ppfInputTmp, m_ppfOutputTmp, iNumFrames);
                m_pPPM->ppmProcess(m_ppfInputTmp, m_iBlockLength);
                for (int i = 0; i < m_iNumChannels; i++) {
                    m_ppfPeakValue[i][counter] = m_pPPM->getPeak(i);
                }

                counter += 1;
                iNumFramesRemaining -= iNumFrames;
            }
        }

        CVibrato *m_pVibrato;
        float **m_ppfInputData,
            **m_ppfOutputData,
            **m_ppfInputTmp,
            **m_ppfOutputTmp;
        int m_kiDataLength;
        float m_fMaxModWidth;
        int m_iBlockLength;
        int m_iNumChannels;
        float m_fSampleRate;
        float m_fModWidth,
            m_fModFreq;
        int m_iNumOfBlocks;
        
        //Test for PPM
        PeakProgramMeter* m_pPPM;
        float** m_ppfPeakValue;
        float** m_ppfReference;

    };
    
//    TEST_FIXTURE(VibratoData, VibModWidthZero)
//    {
//        m_pVibrato->setParam(CVibrato::kParamModFreqInHz, 20);
//        m_pVibrato->setParam(CVibrato::kParamModWidthInS, 0);
//
//        process();
//
//        int iDelay = CUtil::float2int<int>(m_fMaxModWidth*m_fSampleRate+1);
//        for (int c = 0; c < m_iNumChannels; c++)
//            CHECK_ARRAY_CLOSE(m_ppfInputData[c], &m_ppfOutputData[c][iDelay], m_kiDataLength-iDelay, 1e-3F);
//    }

//    TEST_FIXTURE(VibratoData, VibDc)
//    {
//        m_pVibrato->setParam(CVibrato::kParamModFreqInHz, 2);
//        m_pVibrato->setParam(CVibrato::kParamModWidthInS, .1F);
//        for (int c = 0; c < m_iNumChannels; c++)
//        {
//            CSynthesis::generateDc(m_ppfInputData[c], m_kiDataLength, (c+1)*.1F);
//        }
//
//        process();
//
//        int iDelay = CUtil::float2int<int>(m_fMaxModWidth*m_fSampleRate+1);
//        for (int c = 0; c < m_iNumChannels; c++)
//            CHECK_ARRAY_CLOSE(m_ppfInputData[c], &m_ppfOutputData[c][iDelay], m_kiDataLength-iDelay, 1e-3F);
//    }

//    TEST_FIXTURE(VibratoData, VibVaryingBlocksize)
//    {
//        m_pVibrato->setParam(CVibrato::kParamModFreqInHz, 2);
//        m_pVibrato->setParam(CVibrato::kParamModWidthInS, .1F);
//
//        process();
//
//        m_pVibrato->resetInstance();
//        m_pVibrato->initInstance(m_fMaxModWidth,m_fSampleRate,m_iNumChannels);
//        m_pVibrato->setParam(CVibrato::kParamModFreqInHz, 2);
//        m_pVibrato->setParam(CVibrato::kParamModWidthInS, .1F);
//        {
//            int iNumFramesRemaining = m_kiDataLength;
//            while (iNumFramesRemaining > 0)
//            {
//
//                int iNumFrames = std::min(iNumFramesRemaining, rand()/RAND_MAX*17000);
//
//                for (int c = 0; c < m_iNumChannels; c++)
//                {
//                    m_ppfInputTmp[c] = &m_ppfInputData[c][m_kiDataLength - iNumFramesRemaining];
//                }
//                m_pVibrato->process(m_ppfInputTmp, m_ppfInputTmp, iNumFrames);
//
//                iNumFramesRemaining -= iNumFrames;
//            }
//        }
//
//        for (int c = 0; c < m_iNumChannels; c++)
//            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_kiDataLength, 1e-3);
//    }

//    TEST_FIXTURE(VibratoData, VibParamRange)
//    {
//        CHECK_EQUAL(Error_t::kFunctionInvalidArgsError, m_pVibrato->setParam(CVibrato::kParamModFreqInHz, -1));
//        CHECK_EQUAL(Error_t::kFunctionInvalidArgsError, m_pVibrato->setParam(CVibrato::kParamModWidthInS, -0.001F));
//
//        CHECK_EQUAL(Error_t::kFunctionInvalidArgsError, m_pVibrato->setParam(CVibrato::kParamModFreqInHz, m_fSampleRate));
//        CHECK_EQUAL(Error_t::kFunctionInvalidArgsError, m_pVibrato->setParam(CVibrato::kParamModWidthInS, m_fMaxModWidth+.1F));
//    }

//    TEST_FIXTURE(VibratoData, VibZeroInput)
//    {
//        m_pVibrato->setParam(CVibrato::kParamModFreqInHz, 20);
//        m_pVibrato->setParam(CVibrato::kParamModWidthInS, 0);
//        
//        for (int c = 0; c < m_iNumChannels; c++)
//            CVector::setZero(m_ppfInputData[c], m_kiDataLength);
//
//        process();
//
//        int iDelay = CUtil::float2int<int>(m_fMaxModWidth*m_fSampleRate+1);
//        for (int c = 0; c < m_iNumChannels; c++)
//            CHECK_ARRAY_CLOSE(m_ppfInputData[c], &m_ppfOutputData[c][iDelay], m_kiDataLength-iDelay, 1e-3F);
//    }
    
    //First Test: zero input; Expect zero output
    TEST_FIXTURE(VibratoData, zeroInputPPM) {
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            m_ppfInputData[i] = new float [m_kiDataLength];
            CVector::setZero(m_ppfInputData[i], m_kiDataLength);
        }

        process();
        
        for (int c = 0; c < m_iNumChannels; c++) {
            CHECK_ARRAY_CLOSE(m_ppfReference[c], m_ppfPeakValue[c], m_iNumOfBlocks, 0);
        }


    }
//
//    //Second Test: DC input; Expect DC output
//    TEST_FIXTURE(VibratoData, dcInputPPM) {
//        for (int i = 0; i < m_iNumChannels; i++)
//        {
//            m_ppfInputData[i] = new float [m_kiDataLength];
//            CSynthesis::generateDc(m_ppfInputData[i], m_kiDataLength, .6f);
//            
//            m_ppfOutputData[i] = new float [m_kiDataLength];
//            CSynthesis::generateDc(m_ppfOutputData[i], m_kiDataLength, .6f);
//        }
//        process();
//        for (int c = 0; c < m_iNumChannels; c++)
//            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_kiDataLength, 1e-3F);
//    }
    
    //Third Test: Sine wave; Expect the same output as the one generated by Matlab

    
    
    
    
}

#endif //WITH_TESTS