//============================================================================
// Name        : calibrateChannels.cpp
// Author      : Miguel Urco
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#define BINPATH		""
#define	CONFIGFILE	"calibration.txt"
#define	CONFIGPATH	"./config-files"

#define XPDATA 1
#define YPDATA 3
#define XIMAGE 55
#define YIMAGE 3
#define SCREEN_HEI 38 //50
#define SCREEN_WIDTH 110

#define RESET		0
#define BRIGHT 		1
#define DIM			2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define PI			3.141592654

#include <stdio.h>
#include <stdlib.h>

#include "init.h"
#include "CpdataRead.h"
#include "CpdataUtils.h"
#include "Cimaging.h"
#include "cmdUtils.h"
#include "fileUtils.h"
#include "hdf5File.h"
#include "configArg.h"
#include "calibration.h"

using namespace std;

//Used as global variable inside Cimaging.cpp
struct imagingBuff *pImg;

class CalibrateImag {
	private:
//		int argc;
//		char * const *argv;
		CpdataRead *pdataReadObj;
		CpdataUtil *pdataUtilObj;
		Cimaging *imagingObj;

		//Calibration
		Ccalibration *calibrateObj;

		bool printFinalMsg;
		char version[10];
//		float beacoh;

		int __setupObjects(UINT, UINT);
		void __setNewImagingPath();
		void __showReadingInfo(float *pNoise, float *pEstimatedPhase, float *pPhase, float *pBeaconPhase, float beacoh);
		void __showProcessingInfo(time_t, time_t, char*);
		int __setupImagingObj(unsigned int *channels, unsigned int nChannels);

	public:
		//**********Arguments****************
		struct options *pOptions;

		//***********************************
		struct configParameters* pFileOptions;

		char ipath[120];
//		char iFile[30];

		int heiIndex[2], beaconIndex[2];

		int nAvgFixed;
//		float *pPhase;

		CalibrateImag(int argc, char *argv[]);
		~CalibrateImag();

		void setScriptState(int argc, char *argv[]);
		void setConfigFile(UINT, UINT);
		void createObjects();
		void resizeScreen();

		void run();
		void procData();
};

CalibrateImag::~CalibrateImag(){
//	delete [] pPhase;
}

CalibrateImag::CalibrateImag(int argc, char *argv[]) {
	this->setScriptState(argc, argv);
	//this->setConfigFile(pOptions->startYear, pOptions->startDoy);
	this->createObjects();
	this->resizeScreen();
	this->run();
}

void CalibrateImag::setScriptState(int argc, char *argv[]){

	strcpy(version, VERSION);
	printFinalMsg = true;

	pOptions = getArgs3(argc,argv);

}

void CalibrateImag::setConfigFile(UINT thisYear, UINT thisDoy){
	int sts=0;
	char thisConfigFile[100];

	//SETTING CONFIG FILE
	//Finding files: configYYYYDDD.txt or
	//Setting configuration file by default CONFIGFILE

	if (strcmp(pOptions->config_file,"auto") == 0)
	{
		//Setting configuration file by default	if pOptions->config_file doesn't exist
		if(sts==0) strcpy(thisConfigFile, CONFIGFILE);
	}
	else
		strcpy(thisConfigFile, pOptions->config_file);

	//Reading parameters from configuration file
	pFileOptions = readConfigFile(pOptions->config_path, thisConfigFile);

	if(pFileOptions == NULL){
		printf("\nCopy calibration.txt.template to calibration.txt if you need a new configuration file\n");
		end_program(printFinalMsg);
	}

	printf("HYDRA PHASES = ");
	for (unsigned int i=0; i<pFileOptions->nChannels; i++)
		printf("%4.2f, ", pFileOptions->pHydraPhase[i]);
	printf("\n\n");

}

void CalibrateImag::createObjects(){

	pdataReadObj = new class CpdataRead();
	pdataUtilObj = new class CpdataUtil();
	imagingObj = new class  Cimaging(pOptions->nthreads, 255);
	calibrateObj = new class Ccalibration();

//	pPhase = new float[20]; //20=maximum number of channels

}

void CalibrateImag::resizeScreen(){

	//**********Resizing the screen*************
	printf("\e[8;%d;%dt",SCREEN_HEI, SCREEN_WIDTH);
	system("clear");

}

void CalibrateImag::run(){

	int sts=0;
	unsigned int thisYear, thisDoy;

	thisYear = pOptions->startYear;
	thisDoy = pOptions->startDoy;

	printf("Searching files ...\n");

	while(1){

		sts = pdataReadObj->setHeader(pOptions->dpath,
										thisYear, thisDoy,
										pOptions->set,
										pOptions->startHour, pOptions->endHour,
										pOptions->startMin, pOptions->endMin,
										pOptions->label,
										pOptions->online_flag);

		if (sts==1)
		{

			this->setConfigFile(thisYear, thisDoy);
			this->__setupObjects(thisYear, thisDoy);
			system("clear");
			this->procData();
		}

		thisDoy++;

		if(thisDoy > 366)
			{thisYear++; thisDoy=1;}

		if(thisYear > pOptions->endYear)
			break;

		if((thisYear == pOptions->endYear) && (thisDoy > pOptions->endDoy))
			break;

		pOptions->set=-2; //Read the first file of the next day

	}

	end_program(printFinalMsg);
}

void CalibrateImag::procData(){

	int fileStatus = 0;
	char iFile[30];

	float *pNoise, *pBeaconPhase;
	float beacoh;

	int m,n;
	float **image;
	float curr_phase, end_phase;
	float pPhase[pdataUtilObj->nChannels], pEstimatedPhase[pdataUtilObj->nChannels], pCurrentPhase[pdataUtilObj->nChannels];

	time_t startProcTime, endProcTime;

	////////////////////////////////
	////////////////////////////////
	//Getting and averaging data
	pdataUtilObj->cleanArrays();
	for(int i=0; i<pOptions->navg; i++)
	{
		fileStatus = pdataReadObj->readNextBlock();
		if(fileStatus == 0) break;

		pdataUtilObj->avgData(pdataReadObj->pSelfSpect,
							 pdataReadObj->pCrossSpect,
							 pdataReadObj->pDcSpect,
							 pdataReadObj->sHeader.ltime);
	}

	if(fileStatus == 0)	return;
	////////////////////////////////
	////////////////////////////////

	pNoise = pdataUtilObj->getNoise();

	pBeaconPhase = pdataUtilObj->getPhase(pFileOptions->fBeaconRange[0],pFileOptions->fBeaconRange[1], &beacoh);

	for (UINT i=0; i<pdataUtilObj->nChannels; i++)
		pPhase[i] = pFileOptions->pHydraPhase[i];


	for (UINT i=0; i<pdataUtilObj->nChannels; i++){
		//Initial phases
		pEstimatedPhase[i] = pPhase[i];
	}

	this->__setNewImagingPath();
	//Auto-calibration loop

	this->__setupImagingObj(calibrateObj->channels, calibrateObj->nChannels);
	system("clear");

	pdataUtilObj->sec = 0;
	pdataUtilObj->min = pdataReadObj->sHeader.min;
	curr_phase = pPhase[ calibrateObj->channel ];
	end_phase = curr_phase + calibrateObj->nphases*calibrateObj->phase_step;

	//Updating values to estimated phases
	for (UINT i=0; i<pdataUtilObj->nChannels; i++){
		pCurrentPhase[i] = pEstimatedPhase[i];
	}
	pCurrentPhase[calibrateObj->channel] = curr_phase;

	//Channel calibration loop
	do{

		//Phase correction
		pdataUtilObj->fixPhase(pPhase);

		this->__showReadingInfo(pNoise, pEstimatedPhase, pCurrentPhase, pBeaconPhase, beacoh);

		time(&startProcTime);
		image = imagingObj->getImaging(pdataUtilObj->pSelfSpect, pdataUtilObj->pCrossSpect, pNoise, pOptions->snr_th, nAvgFixed, &m, &n);
		time(&endProcTime);

		//Saving HDF5 file
		sprintf(iFile,"img%02d%02d%02d", pdataUtilObj->hour, pdataUtilObj->min, pdataUtilObj->sec);

		svArray2HDF5v2(ipath,iFile,image, imagingObj->nfft, imagingObj->nHeis, imagingObj->nx, imagingObj->ny);

		this->__showProcessingInfo(startProcTime, endProcTime, iFile);

		//EXIT IF KEY "q" IS PRESSED
//			if(pressKey("q")) end_program(printFinalMsg);

		calibrateObj->addPhasePower2(curr_phase, image, n, m);

		/////////////////////////////////////////////////////
		////Next iteration (next phase)

		//Do not add phase to channels again
		for (unsigned int i=0; i<calibrateObj->nChannels; i++)
			pPhase[calibrateObj->channels[i]] = 0;

		pPhase[ calibrateObj->channel ] = calibrateObj->phase_step;

		pCurrentPhase[calibrateObj->channel] += calibrateObj->phase_step;

		//Calibration mode
		pdataUtilObj->sec ++;
		if ( pdataUtilObj->sec > 59){
			pdataUtilObj->min ++;
			pdataUtilObj->sec = 0;
		}

		if ( pdataUtilObj->min > 59){
			pdataUtilObj->hour ++;
			pdataUtilObj->min = 0;
		}

		curr_phase += calibrateObj->phase_step;
	}while(curr_phase <= end_phase);

	//Setting channel phase to its right value
	pEstimatedPhase[ calibrateObj->channel ] = calibrateObj->estimatePhase();

	printf("\nPhase[%d] = %4.3f\n ", calibrateObj->channel, pEstimatedPhase[ calibrateObj->channel ]);

	end_program(printFinalMsg);

}

int CalibrateImag::__setupObjects(UINT thisYear, UINT thisDoy){
	//SETTING ARGUMENTS OF PDATA FILE
	//If there are no data for this day it should continue with the next one

	float minHei, maxHei;

	//VERIFING CONFIG AND PDATA FILE
	if(pFileOptions->nChannels != pdataReadObj->sHeader.nChannels)
	{
		printf("\n Configuration file is not in agreement with the header file\n");
		end_program(printFinalMsg);
	}

	minHei = pdataReadObj->sHeader.h0;
	maxHei = pdataReadObj->sHeader.h0 + pdataReadObj->sHeader.nSamples*pdataReadObj->sHeader.dh;

	if (pOptions->minHei < 0)
		pOptions->minHei = pFileOptions->fProcRange[0];

	if (pOptions->maxHei < 0)
		pOptions->maxHei = pFileOptions->fProcRange[1];

	if (pOptions->minHei < minHei){
		printf("\nMinimum height should not be less than %6.2f\n", minHei);
		pOptions->minHei = minHei;
//		end_program(printFinalMsg);
	}

	if (pOptions->maxHei > maxHei){
		printf("\nMaximum height should not be greater than %6.2f\n", maxHei);
		pOptions->maxHei = maxHei;
//		end_program(printFinalMsg);
	}

	if (pFileOptions->fBeaconRange[0] < minHei){
		printf("\nMinimum beacon height should not be less than %6.2f\n", minHei);
		end_program(printFinalMsg);
	}

	if (pFileOptions->fBeaconRange[1] > maxHei){
		printf("\nMaximum beacon height should not be greater than %6.2f\n", maxHei);
		end_program(printFinalMsg);
	}

	heiIndex[0] = (int)((pOptions->minHei - pdataReadObj->sHeader.h0)/pdataReadObj->sHeader.dh);
	heiIndex[1] = (int)((pOptions->maxHei - pdataReadObj->sHeader.h0)/pdataReadObj->sHeader.dh);
	beaconIndex[0] = (int)((pFileOptions->fBeaconRange[0] - pdataReadObj->sHeader.h0)/pdataReadObj->sHeader.dh);
	beaconIndex[1] = (int)((pFileOptions->fBeaconRange[1] - pdataReadObj->sHeader.h0)/pdataReadObj->sHeader.dh);

	//SETTING PUTIL OBJECT
	pdataUtilObj->setArrayParms(pdataReadObj->sHeader.nChannels,
								pdataReadObj->sHeader.channels,
								pdataReadObj->sHeader.nPairs,
								pdataReadObj->sHeader.crossPairs,
								pdataReadObj->sHeader.nProfiles,
								pdataReadObj->sHeader.nSamples,
								pdataReadObj->sHeader.h0,
								pdataReadObj->sHeader.dh,
								pdataReadObj->sHeader.nIncoherentInt,
								pdataReadObj->sHeader.nCoherentInt,
								pOptions->nbavg,
								pOptions->navg,
								pOptions->shift);

	nAvgFixed =  pOptions->nbavg * pdataUtilObj->nIncoherentInt * pOptions->fact_avg;

	if (pOptions->nChannels == 0) {
		pOptions->nChannels = pdataReadObj->sHeader.nChannels;
		pOptions->channels = pdataReadObj->sHeader.channels;
	}

	if (pOptions->cal_channel  == -1)
		pOptions->cal_channel = pOptions->channels[pOptions->nChannels - 1];

	calibrateObj->init(pOptions->cal_auto,
				  	  pOptions->autocal_chan_sequence,
				  	  pOptions->autocal_max_nchannels,
				  	  1,
				  	  pOptions->cal_phase_step,
				  	  pOptions->cal_channel,
				  	  pOptions->nChannels,
				  	  pOptions->channels,
				  	  pOptions->cal_nphases,
				  	  pFileOptions->nx,
				  	  pFileOptions->ny,
				  	  pdataUtilObj->nFftPoints,
				  	  heiIndex[1] - heiIndex[0] + 1,
							pFileOptions->scalex,
							pFileOptions->scaley
				  	  );

	printf("DATA HEIGHT_RANGE     = %6.2f - %6.2f\n", minHei, maxHei);
	printf("SELECTED HEIGHT_RANGE = %6.2f - %6.2f\n", pOptions->minHei, pOptions->maxHei);
	printf("\n");

	return(1);
}

int CalibrateImag::__setupImagingObj(unsigned int *channels, unsigned int nChannels){

	int flagOk=0;

	//SETTING IMAGING OBJECT
	flagOk = imagingObj->setParameters(pFileOptions->nx,
									 pFileOptions->ny,
									 pdataUtilObj->nChannels,
									 pdataUtilObj->channels,
									 pdataUtilObj->nPairs,
									 pdataUtilObj->crossPairs,
									 pdataUtilObj->nFftPoints,
									 heiIndex,
									 nChannels,
									 channels);

	if (flagOk==0) end_program(printFinalMsg);

	imagingObj->getSpreadFunc(pFileOptions->pRx,
							 pFileOptions->pRy,
							 pFileOptions->pRz,
							 pFileOptions->scalex,
							 pFileOptions->scaley,
							 pFileOptions->rotangle,
							 pFileOptions->offset,
							 pFileOptions->wavelength);

	return(1);
}

void CalibrateImag::__setNewImagingPath(){
	DIR *dp;
	char ch, del_cmd[133];

	//SETTING THE WORK DIRECTORY TO STORE IMAGING DATA
	sprintf(ipath,"%s/%s/", pOptions->ppath, BINPATH);

	if ((dp = opendir(ipath)) == NULL){
		mkdir(ipath,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if((dp = opendir(ipath)) == NULL){
			printf("Directory to store processing data cannot be created: %s\n", ipath);
			end_program(printFinalMsg);;
		}
	}
	closedir(dp);


	sprintf(ipath,"%s/%s/BIN%04d%03d/", pOptions->ppath, BINPATH, pdataReadObj->sHeader.year, pdataReadObj->sHeader.doy);

	if ((dp = opendir(ipath)) == NULL){
		mkdir(ipath,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if((dp = opendir(ipath)) == NULL){
			printf("Directory to store processing data cannot be created: %s\n", ipath);
			end_program(printFinalMsg);;
		}
	}
	else{
		printf("Do you want to delete files in %s (y/n): ", ipath);
		ch = getchar();
		if (ch == 121){
			sprintf(del_cmd,"exec rm -r %s/*", ipath);
			printf("Deleting files ...\n%s\n", del_cmd);

			system(del_cmd);
		}
	}
	closedir(dp);

	//WRITING HEADER FILE

	svHeader2HDF5v2(ipath, pdataReadObj->sHeader.year, pdataReadObj->sHeader.doy,
				 pdataReadObj->sHeader.hour, pdataReadObj->sHeader.min,pdataReadObj->sHeader.sec,
				 pdataReadObj->sHeader.h0, pdataReadObj->sHeader.dh, pdataReadObj->sHeader.nSamples,
				 pdataReadObj->sHeader.ipp,
				 pdataUtilObj->nCoherentInt,
				 pdataUtilObj->nIncoherentInt,
				 pFileOptions->nx, pFileOptions->ny, pFileOptions->scalex, pFileOptions->scaley,
				 pFileOptions->rotangle, pFileOptions->offset, pFileOptions->wavelength,
				 heiIndex, beaconIndex,
				 pdataReadObj->sHeader.nProfilesPerBlock/pOptions->nbavg,
				 pOptions->nChannels, pFileOptions->pHydraPhase, pFileOptions->pDeltaPhase);

}


void CalibrateImag::__showReadingInfo(float *pNoise, float *pInitialPhase, float *pCurrPhase, float *pBeaconPhase, float beacoh){

	int xi, yi;
	tm *timeinfo;
	time_t startDataTime, endDataTime;

	////////////////////////////////////////////////////////////////
	/////////////DISPLAY PDATA INFO ON SCREEN///////////////////////

	gotoxy(1,1);
	printf("PROCIMAGING v%s: ", version);

	xi = XPDATA; yi = YPDATA;
	gotoxy(xi,yi++);
	printf("READING INFO\n");

	gotoxy(xi,yi++);
	printf("Data path    : %s\n", pOptions->dpath);

	gotoxy(xi,yi++);
	printf("Pdata file   : %s\n", pdataReadObj->filename);

	gotoxy(xi,yi++);
	startDataTime  = (time_t)(pdataUtilObj->ltime);
	timeinfo = localtime(&startDataTime);
	printf("Start Time   : %s\n", asctime(timeinfo));

	gotoxy(xi,yi++);
	endDataTime  = (time_t)(pdataReadObj->sHeader.ltime);
	endDataTime += pdataReadObj->sHeader.blockDelay;
	timeinfo = localtime(&endDataTime);
	printf("End Time     : %s\n", asctime(timeinfo));

	gotoxy(xi,yi++);
	printf("Channels     : ");
	for (UINT i=0; i<calibrateObj->nChannels; i++)
		printf("%02d, ", calibrateObj->channels[i]);
	printf("\n");

	gotoxy(xi,yi++);
	printf("Channel to calibrate : %02d\n", calibrateObj->channel);

	gotoxy(xi,yi++);
	printf("Phase step           : %4.3f\n", calibrateObj->phase_step);
	yi++;

	gotoxy(XPDATA+2,yi);
	printf("NOISE:");
	gotoxy(XPDATA+20,yi);
	printf("INITIAL PHASE:");
	gotoxy(XPDATA+40,yi);
	printf("CURRENT PHASE:");
	gotoxy(XPDATA+60,yi);
	printf("BEACON PHASE: (coh=%f)", beacoh);

	yi++;
	//printf("************************\n");
	for(UINT j=0; j<pdataUtilObj->nChannels; j++)
	{
		gotoxy(XPDATA+2,yi+j);
		printf("[%d]  = %5.2fdB",j , 10*log10(pNoise[j]));
		gotoxy(XPDATA+20,yi+j);
		printf("[%d]  = % 5.3f",j , pInitialPhase[j]);
		gotoxy(XPDATA+40,yi+j);
		printf("[%d]  = % 5.3f",j , pCurrPhase[j]);
		gotoxy(XPDATA+60,yi+j);
		printf("[%d]  = % 5.3f",j , pBeaconPhase[j]);
	}

	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////
}

void CalibrateImag::__showProcessingInfo(time_t startTime, time_t endTime, char *iFile){

	int xi, yi;
	tm *timeinfo;
	///////////DISPLAY PROCESS INFO ON SCREEN////////////////

	xi = XIMAGE; yi = YIMAGE;
	gotoxy(xi,yi++);
	printf("PROCESSING INFO\n");

	gotoxy(xi,yi++);
	printf("Processing path  : %s\n", pOptions->ppath);

	gotoxy(xi,yi++);
	printf("Generated file   : %s.h5\n", iFile);

	gotoxy(xi,yi++);
	timeinfo = localtime(&startTime);
	printf("Start proc. time : %s\n", asctime(timeinfo));

	gotoxy(xi,yi++);
	timeinfo = localtime(&endTime);
	printf("End proc. time   : %s\n", asctime(timeinfo));

	gotoxy(xi,yi++);
	printf("Processing time  : %-3li sec\n", endTime - startTime);

	gotoxy(XIMAGE, 22);
	printf("Press 'q' to quit\n");

	gotoxy(XPDATA,pdataUtilObj->nChannels+26);

	//EXIT IF KEY "q" IS PRESSED
	if(pressKey("q")) end_program(printFinalMsg);

	gotoxy(XPDATA,pdataUtilObj->nChannels+23);
	////////////////////////////////////////////////
	///////////////////////////////////////////////

}

int main(int argc, char *argv[]){

	CalibrateImag calibrateImagObj(argc, argv);

	return(0);

}
