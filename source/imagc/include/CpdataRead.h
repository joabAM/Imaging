/*
 * jroFile.h
 *
 *  Created on: Jan 18, 2010
 *      Author: murco
 */

#ifndef CPDATAREAD_H_
#define CPDATAREAD_H_
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <cmath>
#include "memUtils.h"
#include "fileUtils.h"
#include "complex.h"
#include "cnoise.h"

//RC PARAM (L5 AND L6 FUNCTION)
#define NONE		0
#define FLIP		1
#define CODE		2
#define SAMPLING	3
#define LIN6DIV256	4
#define SYNCRO		5
#define SAVE_CHANNELS_DC  32768

typedef unsigned short USHORT;
typedef unsigned int UINT;
typedef UINT CROSS [2];

struct header{
	//DATE HEADER
    USHORT year;
    USHORT doy;
    USHORT hour;
    USHORT min;
    USHORT sec;
	//SHORT HEADER
    UINT length;
    USHORT version;
    UINT currentBlock;
    UINT ltime;
    USHORT msec;
    UINT nBlocksRead;
    //SYSTEM HEADER
    UINT nSamples;
    UINT nProfiles;
    UINT nChannels;
    //RC HEADER
    UINT expType;
    float ipp;
    float txa;
    float txb;
	UINT nTaus;
	UINT codeType;
    float fClock;
    //WINDOW HEADER
	float h0;
	float dh;
	UINT nsa;
	//PROCESS HEADER
    UINT dataType;
    UINT sizeDataBlock;
    UINT nProfilesPerBlock;
    UINT nBlocksPerFile;
    UINT nWindows;
    UINT processFlag;
    UINT nCoherentInt;
    UINT nIncoherentInt;
	UINT nFftPoints;
    UINT totalSpectra;
    UINT nPairs;
    UINT channels[20];
    CROSS crossPairs[50];
    UINT blockDelay;
    //tm *timeinfo;
};

//Defining shortHeader struct
struct shortHeader{
	unsigned int length;
	unsigned short version;
	unsigned int currentBlock;
	unsigned int utime;
	unsigned short msec;
	short timeZone;
	short dst;
	unsigned int countError;
};

struct sysHeader{
	unsigned int length;
	unsigned int nSamples;
	unsigned int nProfiles;
	unsigned int nChannels;
	unsigned int adcResolution;
	unsigned int pcibWidthBus;
};

struct rcHeader{
	unsigned int length;
	unsigned int expType;
	unsigned int nTx;
	float ipp;
	float txa;
	float txb;
	unsigned int nWindowsRC;
	unsigned int nTaus;
	unsigned int codeType;
	unsigned int line6;
	unsigned int line5;
	float fClock;
	unsigned int prePulseBefore;
	unsigned int prePulseAfter;
	char  ippRange[20];
	char  txaRange[20];
	char  txbRange[20];
};

struct winHeader{
	float h0;
	float dh;
	unsigned int nsa;
};

struct procHeader{
	unsigned int length;
	unsigned int dataType;
	unsigned int sizeDataBlock;
	unsigned int nProfilesPerBlock;
	unsigned int nBlocksPerFile;
	unsigned int nWindowsProc;
	unsigned int processFlag;
	unsigned int nCoherentInt;
	unsigned int nIncoherentInt;
	unsigned int totalSpectra;
};

class CpdataRead{
private:
	bool readDC;
	bool online;
	bool fileOpened, firstBlock, allocatedMem;
	int yy, dd, ss;
	int timeout;
	FILE *fp;
	int readShortHeader(void);
	int readLongHeader(void);
	int readHeader(void);
	int setFilename(void);
	void setLastFile(void);
	void allocatedMemory();
	//Variables for processing
	int nAvg, nBeamAvg;
	UINT sizeFftperChan, sizeBlockperChan;

public:
	char path[100], filename[30], label[30], fullfilename[170];
	int nTotalBlocksRead;
	unsigned int startTime, endTime;
	struct header sHeader;
	fcomplex **pCrossSpect, **pDcSpect, *pDcPerChannel;
	float **pSelfSpect, *pNoise;
	bool newDoy;
	CpdataRead(void);
	CpdataRead(char*, int, int, int, char*);
	~CpdataRead();

	int getNoise();
	int setAverageData(int, int);
	int setInitialPath(char*, int, int, int, char*);
	int setInitialPath(char*, int, int, int, unsigned int, unsigned int, char*);
	int setDate(char*, int, int, int, int, int, int, int, char*);
	int setDate(char*, int, int, int, int, int, int, int, char*, bool);
	int setHeader(char*, int, int, int, int, int, int, int, char*, bool);
	int readNextBlock(void);
	int readNextFile(int);
	int readNextBlock2(void);
	int removeDC(void);
	//Variables for processing
};

#endif /* CPDATAREAD_H_ */
