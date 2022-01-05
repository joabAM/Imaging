/*
 * CpdataUtil.h
 *
 *  Created on: Mar 2, 2010
 *      Author: murco
 */

#ifndef CPDATAUTIL_H_
#define CPDATAUTIL_H_

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include "memUtils.h"
#include "complex.h"
#include "cnoise.h"

#define MAXNUMCHAN 16
typedef unsigned int UINT;
typedef UINT CROSS [2];

//struct idData{
//	//DATE HEADER
//	float snr;
//    int i;
//    int j;
//};

class CpdataUtil{
	private:
		bool allocatedMem, allocatedNoiseMem, allocatedPhaseMem;
		UINT nBeamAvg, shift;
		UINT sizeBlockperChan;
	public:
		UINT ltime, htime, year, doy, hour, min, sec;
		CpdataUtil();
		~CpdataUtil();
		void setArrayParms(UINT, UINT*, UINT, CROSS*, UINT, UINT, float, float, UINT, UINT, UINT, UINT, UINT);
		void allocatedMemory();
		void cleanArrays();
		void avgData(float**, fcomplex**, fcomplex**, UINT);
		float* log10Array(float *, int);
		float* getNoise();
		float* getPhase(UINT, UINT, float*);
		void fixPhase(float*);
		float** getSelfPointer(){return pSelfSpect;};
		fcomplex** getCrossPointer(){return pCrossSpect;};
		fcomplex* getDcPointer(){return pDcPerChannel;};


		UINT nChannels, nPairs, nProfiles, nSamples, nFftPoints, nIncoherentInt, nCoherentInt;
		UINT  *channels;
		CROSS  *crossPairs;
		float hei0, deltaHei;
		fcomplex **pCrossSpect, **pDcSpect, *pDcPerChannel;
		float **pSelfSpect, *pNoise, *pPhase;
};

#endif /* CPDATAUTIL_H_ */
