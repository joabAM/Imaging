/*
 * calibration.h
 *
 *  Created on: April 17, 2015
 *      Author: Miguel Urco
 */

#ifndef CALIBRATION_H_
#define CALIBRATION_H_

#define PI			3.141592654

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <math.h>



using namespace std;

typedef unsigned int UINT;

class Ccalibration{
	private:
		bool initialized;
		UINT* chan_sequence;


		float __getPower(float** image, int nx, int ny);
		float __getDesv(float** image, int nx, int ny);

	public:
		UINT max_nchannels;

		//Autocalibration
		int cal_auto;
		UINT* channels;
		UINT nChannels;
		UINT channel;
		UINT nx, ny, nffts, nheis;
		float max_x, max_y;
		int power_len;
		bool filterPower;
		float *powerList;
		float *phaseList;
		float *desvList;
		float calPhaseList[8]={0};

		float const chanParameters [8][4] ={ {0.0, 0.0, 0.0, 0.0},
											{0.2, 0.2, 64, 16}, //2
											{0.1, 0.1, 64, 32}, //1
											{0.1, 0.1, 96, 16}, 
											{0.1, 0.05, 128, 16},
											{0.1, 0.05, 128, 16},//5
											{0.1, 0.05, 128, 16},
											{0.1, 0.02, 128, 16},
											};


		bool *errorList;

		float estimated_phase;
		float *estimated_phase_list;

		//Calibration
		int enabled;
		float phase_step;
		int nphases;

		Ccalibration();
		~Ccalibration();
		void init(int __cal_auto, UINT* __chan_sequence, UINT __max_nchannels,
				int __enable, float __phase_step, int __channel,
				UINT __nchannels, UINT* __channels, int __nphases,
				UINT __nx, UINT __ny,
				UINT __nFFTs, UINT __nHeis, float Xmax, float Ymax);
		void nextSequence();
		int isTheLastChannel();
		void addPhasePower(float phase, float **image, int nx, int ny, bool show);
	  	// void addPhasePower2(float phase, float **image, int nx, int ny,int nFFTPoints, int nHeis);
		float getOptFunction(float** array, int nFFTPoints, int nHeis, int nx, int ny);
		float getSharpness(float** src, int nFFTPoints, int nHeis, int nx, int ny);
		void meanFilter(float src[],float dst[], int windowsize, int N) ;
		
		float estimatePhase();
		float getDispIndx(float **image, int n, int m, int pos);
		void printEstimatedPhases();
		int getPhaseIndex(int filterPoints);
		bool isTrust();
};



#endif /* CALIBRATION_H_ */
