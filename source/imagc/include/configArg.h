/*
 * Cconfig.h
 *
 *  Created on: May 23, 2011
 *      Author: roj-idl71
 */

#ifndef CONFIGARG_H_
#define CONFIGARG_H_

#define BINPATH		""
#ifndef CONFIGFILE
#define	CONFIGFILE	"configure.txt"
#define	CONFIGPATH	"./config-files"
#endif

#ifndef MAX_NUMBER_OF_CHANNELS
#define MAX_NUMBER_OF_CHANNELS	16
#endif

//#define DEFAULT_AUTOCAL_SEQUENCE	3,4,1,2,0,6,5,7
//#define DEFAULT_AUTOCAL_SEQUENCE	0,2,1,3,4,6,5,7
#define DEFAULT_AUTOCAL_SEQUENCE	0,1,2,3,4,6,5,7
#define RESET		0
#define BRIGHT 		1
#define DIM			2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define BLACK 		0
#define RED			1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7

#include <iostream>
#include <stdio.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include "cmdUtils.h"
#include "fileUtils.h"

typedef unsigned int UINT;

namespace std{

struct options{
	int proc_key;
	int nx, ny;
	int sh;
	float dx, dy;
	//DATE HEADER
	char dpath[100];
	char ppath[100];
	unsigned int startYear;
	unsigned int startDoy;
	unsigned int startHour;
	unsigned int startMin;
	unsigned int endYear;
	unsigned int endDoy;
	unsigned int endHour;
	unsigned int endMin;
	float minHei;
	float maxHei;
	int set;
	int navg;
	int nbavg;
	int dc_mode;
	int shift;
	float fact_avg;
	char config_path[100];
	char config_file[100];
	int online_flag;
	int plotting_flag;
	int nthreads;
	char label[20];
	float snr_th;
	UINT nChannels;
	UINT* channels;

	int test_cal;
	int cal_chan_index;	//Channel used in calibration mode
	int autocal;

	int cal_ena;
	int cal_auto;
	int cal_channel;
	float cal_phase_step;
	int cal_nphases;

	UINT *autocal_chan_sequence;
	UINT autocal_max_nchannels;
};

unsigned int n_elements(char* list);

void getArgs(int argc, char *argv[], char *dpath, char *ppath,
				UINT *sYear, UINT *sDoy, UINT *sHour, UINT *sMin, int *set,
				UINT *eYear, UINT *eDoy, UINT *eHour, UINT *eMin,
				int *navg, int *nbavg, int *dc_mode, float *fact_avg,
				char *configpath, char *configfile,
				int *online_flag, int *plotting_flag, int *nthreads, char *label,
				float *snrTh,
				float &minHei, float &maxHei,
				UINT *nChannels=0, UINT *channels=NULL,
				int *test_calibration=0);


options* getArgs2(int argc, char *argv[]);

options* getArgs3(int argc, char *argv[]);

}

#endif /* CONFIGARG_H_ */
