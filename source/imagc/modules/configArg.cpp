/*
 * Cconfig.cpp
 *
 *  Created on: May 23, 2011
 *      Author: roj-idl71
 */

#include "configArg.h"

namespace std {

unsigned int n_elements(char* list){
	unsigned int i=0;

	char *pch;
	pch = strtok(list,",");
	while(pch != NULL){
		pch = strtok (NULL, ",");
		i++;
	}
	return i;
}

void getArgs(int argc, char *argv[], char *dpath, char *ppath, UINT *sYear, UINT *sDoy, UINT *sHour, UINT *sMin,
			int *set, UINT *eYear, UINT *eDoy, UINT *eHour, UINT *eMin,
			int *navg, int *nbavg, int *dc_mode, float *fact_avg,
			char *configpath, char *configfile,
			int *online_flag, int *plotting_flag, int *nthreads, char *label, float *snrTh,
			float &minHei, float &maxHei,
			UINT *nChannels, UINT *channels,
			int *test_calibration){

	//ARGUMENTS
    int c;
    int option_index = 0;
    int nCPUs;
    char tmppath[150];
	char cmd[] = "Usage: procImaging --dpath=<dpath> --ppath=<ppath> --startYear=<startYear> --startDoy=<startDoy>\n"
				 "\t\t  --startHour=<startHour> --startMin=<startMin> || --set=<set>\n"
				 "\t\t  --endYear=<endYear> --endDoy=<endDoy> --endHour=<endHour> --endMin=<endMin>\n"
				 "\t\t  --navg=<navg> --nbavg=<nbavg> --dc-mode=<dc-mode> --factor-avg=<factor-avg>\n"
				 "\t\t  --config-path=<config-path> --config-file=<config-file> --nthreads=<nthreads> --online --plotting\n\n"
				 "This program gets IMAGES from pdata files on <dpath> and save them on <ppath>\n\n"
				 "Required arguments:\n"
				 "--dpath\t\t\t: Data directory, directory where the pdata files are stored\n"
				 "--ppath\t\t\t: Processing directory, directory where the processed files will be saved\n"
				 "--startYear,startDoy\t: Year and day to process.\n"
				 "\nOptional arguments:\n"
				 "--endYear, endDoy\t: End date to process. Use these arguments to process a range of days \n"
				 "--startHour,startMin\t: Start time to process. By default is [00,00]\n"
				 "--endHour, endMin\t: End time to process. By default is [24,00]\n"
				 "--minHei, maxHei\t: minimum and maximum height. By default is [0,10000]\n"
				 "--set\t\t\t: Initial set. Setting set=-2 to begin with the first file of the day,-1 to the last file.\n"
				 "--config-file\t\t: Configuration file used to phase calibration. By default is 'auto'; in this mode\n"
				 "\t\t\t  it looks for the nearest configuration file, otherwise it is set to 'configure.txt'.\n"
				 "\t\t\t  The file format is configYYYYDDD.txt, where YYYY = year and DDD = doy.\n"
				 "--config-path\t\t: Configuration files must be located on this directory. By default is './config-files'\n"
				 "--navg\t\t\t: Incoherent integrations. By default is 1\n"
				 "--nbavg\t\t\t: Beam integrations. By default is 1\n"
				 "--dc-mode\t\t: DC remotion. By default is 0 (disabled)\n"
				 "--factor-avg\t\t: Integration factor. By default is 1\n"
				 "--online\t\t: Setting this option to online process, it overwrite 'set'=-1. By default is disabled\n"
				 "--plotting\t\t: Setting this option to display self spectra. By default is disabled\n"
				 "--nthreads\t\t: Number of threads to use [0-nCPUs]. By default is equal to the number of processors (nCPUs)\n"
				 "--label\t\t\t: sub-folder inside doy folder where the pdata has been saved. By default is IMAGING\n\n"
			 	 "--channels\t\t\t: select channels to be processed. Example: 0,2,6\n\n"
				 "--test_cal\t\t\t: empirical calibration mode. By default is false\n\n"
				 "Example: (If the first file is /data/d2010040/IMAGING/P2010040225.pdata)\n"
				 "\t >>procImaging --dpath=/data --ppath=/data/BIN --startYear=2010 --startDoy=40 --set=225(or set=-2) --plotting";


	*set = -100;
	*sYear = *sDoy = *eYear = *eDoy = 0;
	*sHour = 0; *sMin = 0;
	*eHour = 24; *eMin = 0;
	*navg = *nbavg = 1;
	*dc_mode = 0;
	*fact_avg = 1;
    *online_flag = *plotting_flag = 0;
    *snrTh = 0.3;
    nCPUs = sysconf( _SC_NPROCESSORS_ONLN ); //Getting number of processors available
    *nthreads = nCPUs;
    strcpy(configpath,CONFIGPATH);
    strcpy(configfile,"auto");
    strcpy(label,"\0");
    *nChannels = 0;
    *test_calibration=0;
    minHei = 0;
    maxHei = 10000;

	//Getting parameters of configuration
	//system("clear");

	static struct option long_options[] =
          {
            // These options set a flag.
            {"online",    no_argument,   online_flag, 1},
            {"plotting",  no_argument, plotting_flag, 1},
            // These options don't set a flag.
            //   We distinguish them by their indices.
            {"dpath",     required_argument, 0, 'p'},
            {"ppath",     required_argument, 0, 'r'},
            {"startYear", required_argument, 0, 'y'},
            {"startDoy",  required_argument, 0, 'd'},
            {"startHour", required_argument, 0, 'h'},
            {"startMin",  required_argument, 0, 'm'},
            {"set",       required_argument, 0, 's'},
            {"endYear",   required_argument, 0, 'z'},
			{"endDoy",    required_argument, 0, 'e'},
			{"endHour",   required_argument, 0, 'i'},
			{"endMin",    required_argument, 0, 'n'},
			{"navg",      required_argument, 0, 'a'},
			{"nbavg",     required_argument, 0, 'b'},
			{"dc-mode",   required_argument, 0, 'c'},
			{"factor-avg",required_argument, 0, 'x'},
			{"config-file", required_argument, 0, 'f'},
			{"config-path", required_argument, 0, 'g'},
			{"nthreads",  required_argument, 0, 't'},
			{"label",  required_argument, 0, 'l'},
			{"snrth",  required_argument, 0, 'j'},
			{"channels",  required_argument, 0, 'q'},
			{"test-cal",  required_argument, 0, 'k'},
			{"minHei",  required_argument, 0, 'o'},
			{"maxHei",  required_argument, 0, 'u'},
			{"procKey", required_argument, 0,'pk'},
            {0, 0, 0, 0}
          };
        //* getopt_long stores the option index here.

	textcolor(RESET, RED, WHITE);
    if (argc<4){
		printf("Too few arguments:\n");
		textcolor(RESET, BLUE, WHITE);
		printf("%s\n",cmd);
		textcolor(RESET, BLACK, WHITE);
		end_program(true);
    }

    while (1)
    {
    	c = getopt_long (argc, argv, "p:r:y:d:h:m:s:z:e:i:n:a:b:c:x:f:g:t:l:j:q:k:o:u:",
    			long_options, &option_index);

    	//* Detect the end of the options.
    	if (c == -1)
    		break;

    	switch (c){
    	case 0:
    		//* If this option set a flag, do nothing else now.
    		if (long_options[option_index].flag != 0)
    			break;
    		printf ("option %s", long_options[option_index].name);
    		if (optarg)
    			printf (" with arg %s", optarg);
    		printf ("\n");
    		break;
    	case 'p':
    		sprintf(dpath,"%s",optarg);
    		break;
    	case 'r':
    		strcpy(ppath,optarg);
    		break;
    	case 'y':
    		*sYear = atoi(optarg);
    		break;
    	case 'd':
    		*sDoy = atoi(optarg);
    		break;
    	case 'h':
    		*sHour = atoi(optarg);
    		break;
    	case 'm':
    		*sMin = atoi(optarg);
    		break;
    	case 's':
    		*set = atoi(optarg);
            break;
    	case 'z':
    		*eYear = atoi(optarg);
            break;
    	case 'e':
    		*eDoy = atoi(optarg);
            break;
    	case 'i':
    		*eHour = atoi(optarg);
            break;
    	case 'n':
    		*eMin = atoi(optarg);
            break;
    	case 'a':
    		*navg = atoi(optarg);
            break;
    	case 'b':
    		*nbavg = atoi(optarg);
            break;
    	case 'c':
        	*dc_mode = atoi(optarg);
            break;
    	case 'x':
    		*fact_avg = atof(optarg);
            break;
    	case 'f':
    		strcpy(configfile,optarg);
            break;
    	case 'g':
    		strcpy(configpath,optarg);
            break;
    	case 't':
    		*nthreads = atoi(optarg);
            break;
    	case 'l':
    		strcpy(label,optarg);
            break;
    	case 'j':
    		*snrTh = atof(optarg);
			break;
    	case 'q':
    		char __channelStr[20];
    		char __tmp[20];

    		strcpy(__channelStr,optarg);
    		strcpy(__tmp,__channelStr);

    		*nChannels = n_elements(__tmp);

    		FillUIntArray(__channelStr, channels, *nChannels);

            break;
    	case 'k':
    		*test_calibration = atoi(optarg);
            break;
    	case 'o':
    		minHei = atof(optarg);
			break;
    	case 'u':
    		maxHei = atof(optarg);
			break;
    	case '?':
    		//* getopt_long already printed an error message.
            break;
    	default:
    		abort ();
    	}
    }

	//*********************************************************
	//**************VERIFY PARMS ******************************
    DIR *dp;
    int finish = 0;


	if((dp = opendir(dpath)) == NULL){
		printf("Data directory doesn't exist: %s\n", dpath);
		finish = 1;
	}
	else
		closedir(dp);

	if ((dp = opendir(ppath)) == NULL){
		mkdir(ppath,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if((dp = opendir(ppath)) == NULL){
			printf("Directory to store processed data cannot be created: %s\n", ppath);
			finish = 1;
		}
		else{
			closedir(dp);
			sprintf(tmppath,"%s/%s/", ppath, BINPATH);

			if((dp = opendir(tmppath)) == NULL){
				printf("Bin directory to store processed data cannot be created: %s\n", tmppath);
				end_program(true);
			}
			else
				closedir(dp);
		}
	}
	else
		closedir(dp);

	if((dp = opendir(configpath)) == NULL){
		printf("This directory doesn't exist: %s\n", configpath);
		finish = 1;
	}
	else
		closedir(dp);

	if((*online_flag) && (*set== -100))
		*set = -1;

	if (*set==-100)
		*set=-2;

	if(strcmp(configfile,"NULL")==0){
		printf("Configure-file is not defined\n");
		finish = 1;
	}
	if((*sYear<1900) || (*sYear>3000)){
		printf("startYear value is out of range\n");
		finish = 1;
	}
	if((*sDoy<1) || (*sDoy>366)){
		printf("startDoy value is out of range\n");
		finish = 1;
	}
	if((*set<-2) || (*set>999)){
		printf("Number of set is out of range\n");
		finish = 1;
	}
	if((*navg<1) || (*navg>100)){
		printf("Average value is out of range\n");
		finish = 1;
	}
	if((*nbavg<1) || (*nbavg>32)){
		printf("Beam average value is out of range\n");
		finish = 1;
	}
	if((*dc_mode<0) || (*dc_mode>5)){
		printf("DC mode value is out of range\n");
		finish = 1;
	}
	if((*fact_avg<0.0) || (*fact_avg>10.0)){
		printf("Average scale factor is out of range\n");
		finish = 1;
	}
	if((*nthreads<1) || (*nthreads>nCPUs)){
		printf("Number of threads is out of range, it should be between [1-%d]\n",nCPUs);
		finish = 0;
	}

	if(*eYear==0)
		*eYear = *sYear;

	if(*eDoy==0)
		*eDoy = *sDoy;

	if((*eYear<1900) || (*eYear>3000)){
		printf("endYear value is out of range\n");
		finish = 1;
	}

	if((*eDoy<1) || (*eDoy>366)){
		printf("endDoy value is out of range\n");
		finish = 1;
	}

	if(*sHour>24){
		printf("startHour value is out of range\n");
		finish = 1;
	}

	if(*eHour>48){
		printf("endHour value is out of range\n");
		finish = 1;
	}

	if(*sMin>60){
		printf("startMin value is out of range\n");
		finish = 1;
	}

	if(*eMin>60){
		printf("endMin value is out of range\n");
		finish = 1;
	}

	if((*snrTh<0) || (*snrTh>60)){
			printf("endMin value is out of range\n");
			finish = 1;
		}

	for(unsigned int i=0; i<*nChannels; i++){
//		if (channels[i]<0){
//			printf("channel selected less than 1 = %d\n", channels[i]);
//			finish = 1;
//			break;
//		}
		if (channels[i]>20){
			printf("channel selected greater than 20 = %d\n", channels[i]);
			finish = 1;
			break;
		}
	}

	if((minHei<0) || (maxHei>10000)){
			printf("endMin value is out of range\n");
			finish = 1;
		}

	if (finish==1){
		textcolor(RESET, BLUE, WHITE);
		printf("%s\n",cmd);
		textcolor(RESET, BLACK, WHITE);
		end_program(true);
	}

	textcolor(RESET, BLACK, WHITE);
}

options* getArgs2(int argc, char *argv[]){

	//ARGUMENTS
    int c;
    int nCPUs;
    int option_index = 0;
    char tmppath[150];
	char cmd[] = "Usage: procImaging --dpath=<dpath> --ppath=<ppath> --startYear=<startYear> --startDoy=<startDoy>\n"
				 "\t\t  --startHour=<startHour> --startMin=<startMin> || --set=<set>\n"
				 "\t\t  --endYear=<endYear> --endDoy=<endDoy> --endHour=<endHour> --endMin=<endMin>\n"
				 "\t\t  --navg=<navg> --nbavg=<nbavg> --dc-mode=<dc-mode> --factor-avg=<factor-avg>\n"
				 "\t\t  --config-path=<config-path> --config-file=<config-file> --nthreads=<nthreads> --online --plotting\n\n"
				 "This program gets IMAGES from pdata files on <dpath> and save them on <ppath>\n\n"
				 "Required arguments:\n"
				 "--dpath\t\t\t: Data directory, directory where the pdata files are stored\n"
				 "--ppath\t\t\t: Processing directory, directory where the processed files will be saved\n"
				 "--startYear,startDoy\t: Year and day to process.\n"
				 "\nOptional arguments:\n"
				 "--endYear, endDoy\t: End date to process. Use these arguments to process a range of days \n"
				 "--startHour,startMin\t: Start time to process. By default is [00,00]\n"
				 "--endHour, endMin\t: End time to process. By default is [24,00]\n"
				 "--minHei, maxHei\t: minimum and maximum height. By default is [0,-1]\n"
				 "--set\t\t\t: Initial set. Setting set=-2 to begin with the first file of the day,-1 to the last file.\n"
				 "--config-file\t\t: Configuration file used to phase calibration. By default is 'auto'; in this mode\n"
				 "\t\t\t  it looks for the nearest configuration file, otherwise it is set to 'configure.txt'.\n"
				 "\t\t\t  The file format is configYYYYDDD.txt, where YYYY = year and DDD = doy.\n"
				 "--config-path\t\t: Configuration files must be located on this directory. By default is './config-files'\n"
				 "--navg\t\t\t: Incoherent integrations. By default is 1\n"
				 "--nbavg\t\t\t: Beam integrations. By default is 1\n"
				 "--dc-mode\t\t: DC remotion. By default is 0 (disabled)\n"
				 "--factor-avg\t\t: Integration factor. By default is 1\n"
				 "--online\t\t: Setting this option to online process, it overwrite 'set'=-1. By default is disabled\n"
				 "--nthreads\t\t: Number of threads to use [0-nCPUs]. By default is equal to the number of processors (nCPUs)\n"
				 "--label\t\t\t: sub-folder inside doy folder where the pdata has been saved. By default is IMAGING\n\n"
			 	 "--channels\t\t\t: select channels to be processed. Example: 0,2,6\n\n"
				 "--test-cal\t\t\t: empirical calibration mode. By default is false\n\n"
				 "--chan-index\t\t\t: this channel phase will be used in calibration. Phase from 0 to 2*pi\n\n"
			     "--autocal\t\t\t: empirical calibration mode. By default is false\n\n"
				 "Example: (If the first file is /data/d2010040/IMAGING/P2010040225.pdata)\n"
				 "\t >>procImaging --dpath=/data --ppath=/data/BIN --startYear=2010 --startDoy=40 --set=225(or set=-2) --plotting";

	struct options *opt;
	opt = new struct options;

	char __channelStr[20];
	char __tmp[20];
	char sequence[] = {DEFAULT_AUTOCAL_SEQUENCE};

	opt->set = -100;
	opt->startYear = opt->startDoy = opt->endYear = opt->endDoy = 0;
	opt->startHour = 0; opt->startMin = 0;
	opt->endHour = 24; opt->endMin = 0;
	opt->navg = opt->nbavg = 1;
	opt->dc_mode = 0;
	opt->shift = 0;
	opt->fact_avg = 1;
    opt->online_flag = opt->plotting_flag = 0;
    nCPUs = sysconf( _SC_NPROCESSORS_ONLN ); //Getting number of processors available
    opt->nthreads = nCPUs;
    strcpy(opt->config_path,CONFIGPATH);
    strcpy(opt->config_file,"auto");
    strcpy(opt->label,"\0");
    opt->snr_th = 0.3;
    opt->nChannels = 0;
    opt->test_cal=0;
    opt->cal_ena=0;
    opt->cal_phase_step=0.05;
    opt->minHei = 0;
    opt->maxHei = -1;
    opt->cal_chan_index = -1;

    opt->channels = new UINT[20];

    opt->autocal = 0;
    opt->autocal_max_nchannels = 8;
    opt->autocal_chan_sequence = new UINT[opt->autocal_max_nchannels];

    for (UINT i=0; i<opt->autocal_max_nchannels; i++)
    	opt->autocal_chan_sequence[i] = sequence[i];



	//Getting parameters of configuration
	//system("clear");

	static struct option long_options[] =
          {
			// These options set a flag.
			{"online",    no_argument,   &opt->online_flag, 1},
			{"plotting",  no_argument, &opt->plotting_flag, 1},
			// These options don't set a flag.
			//   We distinguish them by their indices.
			{"dpath",     required_argument, 0, 'a'},
			{"ppath",     required_argument, 0, 'b'},
			{"startYear", required_argument, 0, 'c'},
			{"startDoy",  required_argument, 0, 'd'},
			{"startHour", required_argument, 0, 'e'},
			{"startMin",  required_argument, 0, 'f'},
			{"endYear",   required_argument, 0, 'g'},
			{"endDoy",    required_argument, 0, 'h'},
			{"endHour",   required_argument, 0, 'i'},
			{"endMin",    required_argument, 0, 'j'},
			{"set",       required_argument, 0, 'k'},
			{"navg",      required_argument, 0, 'l'},
			{"nbavg",     required_argument, 0, 'm'},
			{"dc-mode",   required_argument, 0, 'n'},
			{"factor-avg",required_argument, 0, 'o'},
			{"config-file", required_argument, 0, 'p'},
			{"config-path", required_argument, 0, 'q'},
			{"nthreads",  required_argument, 0, 'r'},
			{"label",  required_argument, 0, 's'},
			{"snrth",  required_argument, 0, 't'},
			{"channels",  required_argument, 0, 'u'},
			{"test-cal",  required_argument, 0, 'v'},
			{"minHei",  required_argument, 0, 'w'},
			{"maxHei",  required_argument, 0, 'x'},
			{"chan-index",  required_argument, 0, 'y'},
			{"autocal",  required_argument, 0, 'z'},
			{"autocal_chan_sequence",  required_argument, 0, '1'},
            {0, 0, 0, 0}
          };
        //* getopt_long stores the option index here.

	textcolor(RESET, RED, WHITE);
    if (argc<4){
		printf("Too few arguments:\n");
		textcolor(RESET, BLUE, WHITE);
		printf("%s\n",cmd);
		textcolor(RESET, BLACK, WHITE);
		end_program(true);
    }

    while (1)
    {
    	c = getopt_long (argc, argv, "a:b:c:d:e:f:g:h:i:j:k:l:m:n:o:p:q:r:s:t:u:w:x:y:z:1:",
    			long_options, &option_index);

    	//* Detect the end of the options.
    	if (c == -1)
    		break;

    	switch (c){
    	case 0:
    		//* If this option set a flag, do nothing else now.
    		if (long_options[option_index].flag != 0)
    			break;
    		printf ("option %s", long_options[option_index].name);
    		if (optarg)
    			printf (" with arg %s", optarg);
    		printf ("\n");
    		break;
    	case 'a':
    		sprintf(opt->dpath,"%s",optarg);
    		break;
    	case 'b':
    		strcpy(opt->ppath,optarg);
    		break;
    	case 'c':
    		opt->startYear = atoi(optarg);
    		break;
    	case 'd':
    		opt->startDoy = atoi(optarg);
    		break;
    	case 'e':
    		opt->startHour = atoi(optarg);
    		break;
    	case 'f':
    		opt->startMin = atoi(optarg);
    		break;
    	case 'g':
    		opt->endYear = atoi(optarg);
            break;
    	case 'h':
    		opt->endDoy = atoi(optarg);
            break;
    	case 'i':
    		opt->endHour = atoi(optarg);
            break;
    	case 'j':
    		opt->endMin = atoi(optarg);
            break;
    	case 'k':
    		opt->set = atoi(optarg);
            break;
    	case 'l':
    		opt->navg = atoi(optarg);
            break;
    	case 'm':
    		opt->nbavg = atoi(optarg);
            break;
    	case 'n':
        	opt->dc_mode = atoi(optarg);
            break;
    	case 'o':
    		opt->fact_avg = atof(optarg);
            break;
    	case 'p':
    		strcpy(opt->config_file,optarg);
            break;
    	case 'q':
    		strcpy(opt->config_path,optarg);
            break;
    	case 'r':
    		opt->nthreads = atoi(optarg);
            break;
    	case 's':
    		strcpy(opt->label,optarg);
            break;
    	case 't':
    		opt->snr_th = atof(optarg);
			break;
    	case 'u':

    		strcpy(__channelStr,optarg);
    		strcpy(__tmp,__channelStr);

    		opt->nChannels = n_elements(__tmp);

    		FillUIntArray(__channelStr, opt->channels, opt->nChannels);

            break;
    	case 'v':
    		opt->test_cal = atoi(optarg);

    		if (opt->test_cal > 0){
    			opt->cal_ena = 1;
    			opt->cal_phase_step=0.05;
    		}

    		if (opt->test_cal==2){
				opt->cal_phase_step=0.01;
			}

            break;
    	case 'w':
    		opt->minHei = atof(optarg);
            break;
    	case 'x':
    		opt->maxHei = atof(optarg);
            break;
    	case 'y':
    		opt->cal_chan_index = atoi(optarg);
            break;
    	case 'z':
    		opt->autocal = atoi(optarg);

            break;
    	case '1':

    		strcpy(__channelStr,optarg);
    		strcpy(__tmp,__channelStr);

    		opt->autocal_max_nchannels = n_elements(__tmp);

    		FillUIntArray(__channelStr, opt->autocal_chan_sequence, opt->autocal_max_nchannels);

            break;
    	case '?':
    		//* getopt_long already printed an error message.
            break;
    	default:
    		abort ();
    	}
    }

	//*********************************************************
	//**************VERIFY PARMS ******************************
    DIR *dp;
    int finish = 0;

	if((dp = opendir(opt->dpath)) == NULL){
		printf("Data directory doesn't exist: %s\n", opt->dpath);
		finish = 1;
	}
	else
		closedir(dp);

	if ((dp = opendir(opt->ppath)) == NULL){
		mkdir(opt->ppath,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if((dp = opendir(opt->ppath)) == NULL){
			printf("Processed directory to store data cannot be created: %s\n", opt->ppath);
			finish = 1;
		}
		else{
			sprintf(tmppath,"%s/%s/", opt->ppath, BINPATH);
				if((dp = opendir(tmppath)) == NULL){
					printf("Bin directory to store processing data cannot be created: %s\n", tmppath);
					end_program(true);
				}
				closedir(dp);
		}
	}
	if (dp != NULL) closedir(dp);

	if((dp = opendir(opt->config_path)) == NULL){
		printf("This directory doesn't exist: %s\n", opt->config_path);
		finish = 1;
	}
	else
		closedir(dp);

	if((opt->online_flag) && (opt->set== -100))
		opt->set = -1;

	if (opt->set==-100)
		opt->set=-2;

	if(strcmp(opt->config_file,"NULL")==0){
		printf("Configure-file is not defined\n");
		finish = 1;
	}
	if((opt->startYear<1900) || (opt->startYear>3000)){
		printf("startYear value is out of range\n");
		finish = 1;
	}
	if((opt->startDoy<1) || (opt->startDoy>366)){
		printf("startDoy value is out of range\n");
		finish = 1;
	}
	if((opt->set<-2) || (opt->set>999)){
		printf("Number of set is out of range\n");
		finish = 1;
	}
	if((opt->navg<1) || (opt->navg>100)){
		printf("Average value is out of range\n");
		finish = 1;
	}
	if((opt->nbavg<1) || (opt->nbavg>32)){
		printf("Beam average value is out of range\n");
		finish = 1;
	}
	if((opt->dc_mode<0) || (opt->dc_mode>5)){
		printf("DC mode value is out of range\n");
		finish = 1;
	}
	if((opt->fact_avg<0.0) || (opt->fact_avg>10.0)){
		printf("Average scale factor is out of range\n");
		finish = 1;
	}
	if((opt->nthreads<1) ||  ( (opt->nthreads) > (nCPUs) ) ){
		printf("Number of threads is out of range, it should be between [1-%d]\n",nCPUs);
//		finish = 1;
	}

	if(opt->endYear==0)
		opt->endYear = opt->startYear;

	if(opt->endDoy==0)
		opt->endDoy = opt->startDoy;

	if(opt->endYear>3000){
		printf("endYear value is out of range\n");
		finish = 1;
	}

	if(opt->endDoy>366){
		printf("endDoy value is out of range\n");
		finish = 1;
	}

	if(opt->startHour>24){
		printf("startHour value is out of range\n");
		finish = 1;
	}

	if(opt->endHour>48){
		printf("endHour value is out of range\n");
		finish = 1;
	}

	if(opt->startMin>60){
		printf("startMin value is out of range\n");
		finish = 1;
	}

	if(opt->endMin>10000){
		printf("endMin value is out of range\n");
		finish = 1;
	}

	if((opt->minHei < 0) || (opt->maxHei > 10000)){
		printf("endMin value is out of range\n");
		finish = 1;
	}

	for(unsigned int i=0; i<opt->nChannels; i++){
		if (opt->channels[i]>20){
			printf("channel selected greater than 20, channel[%d]=%d\n", i, opt->channels[i]);
			finish = 1;
			break;
		}
	}

	if (finish==1){
		textcolor(RESET, BLUE, WHITE);
		printf("%s\n",cmd);
		textcolor(RESET, BLACK, WHITE);
		end_program(true);
	}
	textcolor(RESET, BLACK, WHITE);

	return opt;
}

options* getArgs3(int argc, char *argv[]){

	//ARGUMENTS
    int c;
    int nCPUs;
    int option_index = 0;
    char tmppath[150];
	char cmd[] = "Usage: procImaging --dpath=<dpath> --ppath=<ppath> --year=<startYear> --doy=<startDoy>\n"
				 "\t\t  --hour=<startHour> --minute=<startMinute> --hmin=<value> --hmax=<value>\n"
				 "\t\t  --eyear=<endYear> --edoy=<endDoy> --ehour=<endHour> --eminute=<endMin>\n"
				 "\t\t  --navg=<navg> --nbavg=<nbavg> --dc-mode=<dc-mode> --factor-avg=<factor-avg>\n"
				 "\t\t  --config-path=<config-path> --config-file=<config-file> --nthreads=<nthreads> --online --plotting\n\n"
				 "This program gets IMAGES from pdata files on <dpath> and save them on <ppath>\n\n"
				 "Required arguments:\n"
				 "--dpath\t\t\t: Data directory, directory where the pdata files are stored\n"
				 "--ppath\t\t\t: Processing directory, directory where the processed files will be saved\n"
				 "--start-year\t\t: Initial year to process.\n"
				 "--start-doy\t\t: Initial day of year to process.\n"
				 "\nOptional arguments:\n"
				 "--end-year\t\t: Final year to process. Use these arguments to process a range of days \n"
				 "--end-doy\t\t: Final day of year to process. Use these arguments to process a range of days \n"
				 "--start-hour\t\t: Initial hour to process. By default is 00\n"
				 "--start-minute\t\t: Initial minute to process. By default is 00\n"
				 "--end-hour\t\t: Final hour to process. By default is 24\n"
				 "--end-minute\t: fimal minute to process. By default is 00\n"
				 "--min-hei, --max-hei\t\t: minimum and maximum height. By default is [0,-1]\n"
				 "--set\t\t\t: Initial set. Setting set=-2 to begin with the first file of the day,-1 to the last file.\n"
				 "--config-file\t\t: Configuration file used to phase calibration. By default is 'auto'; in this mode\n"
				 "\t\t\t  it looks for the nearest configuration file, otherwise the file is set to 'configure.txt'.\n"
				 "\t\t\t  The config file format is configYYYYDDD.txt, where YYYY = year and DDD = doy.\n"
				 "--config-path\t\t: Configuration files must be located on this directory. By default is './config-files'\n"
				 "--navg\t\t\t: Incoherent integrations. By default is 1\n"
				 "--nbavg\t\t\t: Beam integrations. By default is 1\n"
				 "--dc-mode\t\t: DC remotion. By default is 0 (disabled)\n"
				 "--factor-avg\t\t: Integration factor. By default is 1\n"
				 "--online\t\t: Setting this option to online process, it overwrite 'set'=-1. By default is disabled\n"
				 "--nthreads\t\t: Number of threads to use [0-nCPUs]. By default is equal to the number of processors (nCPUs)\n"
				 "--label\t\t\t: sub-folder inside doy folder where the pdata has been saved. By default is IMAGING\n"
			 	 "--channels\t\t: select channels to be processed. Example: 0,2,6\n"
			 	 "\nOnly for calibrateImaging:\n"
				 "--cal-channel\t\t: this channel will be used to calibrate. Its phase will be evaluated from 0 to 2*pi\n"
				 "--cal-phase-step\t: channel phase will be incremented using this step\n"
				 "--cal-nsteps\t\t: number of phases used in calibration mode\n"
				 "--cal-auto-sequence\t: empirical calibration mode. By default is false\n"
				 "\nExample: (If the first file is /data/d2010040/IMAGING/P2010040225.pdata)\n"
				 "\n\t>>procImaging --dpath=/data --ppath=/data/BIN --start-year=2010 --start-doy=40 --start-hour=18 --start-minute=30";

	struct options *opt;
	opt = new struct options;

	char __channelStr[20];
	char __tmp[20];
	char sequence[] = {DEFAULT_AUTOCAL_SEQUENCE};

	sprintf(opt->dpath,"NON ARGUMENT");
	sprintf(opt->ppath,"NON ARGUMENT");
	opt->proc_key = 191;
	opt->set = -100;
	opt->startYear = opt->startDoy = opt->endYear = opt->endDoy = 0;
	opt->startHour = 0; opt->startMin = 0;
	opt->endHour = 24; opt->endMin = 0;
	opt->navg = opt->nbavg = 1;
	opt->dc_mode = 0;
	opt->shift = 0;
	opt->fact_avg = 1;
    opt->online_flag = opt->plotting_flag = 0;
    nCPUs = sysconf( _SC_NPROCESSORS_ONLN ); //Getting number of processors available
    opt->nthreads = nCPUs;
    strcpy(opt->config_path,CONFIGPATH);
    strcpy(opt->config_file,"auto");
    strcpy(opt->label,"\0");
    opt->snr_th = 0.5;
    opt->nChannels = 0;

    opt->minHei = -1;
    opt->maxHei = -1;

    opt->cal_ena = 0;
    opt->cal_auto = 0;
    opt->cal_phase_step = 0.1;
    opt->cal_channel = -1;
    opt->cal_nphases = -1;

    opt->channels = new UINT[20];

    opt->autocal = 0;
    opt->autocal_max_nchannels = MAX_NUMBER_OF_CHANNELS;
    opt->autocal_chan_sequence = new UINT[opt->autocal_max_nchannels];

    for (UINT i=0; i<opt->autocal_max_nchannels; i++)
    	opt->autocal_chan_sequence[i] = sequence[i];

	//Getting parameters of configuration
	//system("clear");

	static struct option long_options[] =
          {
			// These options set a flag.
			{"online",    no_argument,   &opt->online_flag, 1},
			{"cal-ena",  no_argument, &opt->cal_ena, 1},
			{"cal-auto",  no_argument, &opt->cal_auto, 1},
			// These options don't set a flag.
			//   We distinguish them by their indices.
			{"dpath",     required_argument, 0, 'a'},
			{"ppath",     required_argument, 0, 'b'},
			{"start-year", required_argument, 0, 'c'},
			{"start-doy",  required_argument, 0, 'd'},
			{"start-hour", required_argument, 0, 'e'},
			{"start-minute",  required_argument, 0, 'f'},
			{"end-year",   required_argument, 0, 'g'},
			{"end-doy",    required_argument, 0, 'h'},
			{"end-hour",   required_argument, 0, 'i'},
			{"end-minute",    required_argument, 0, 'j'},
			{"set",       required_argument, 0, 'k'},
			{"navg",      required_argument, 0, 'l'},
			{"nbavg",     required_argument, 0, 'm'},
			{"dc-mode",   required_argument, 0, 'n'},
			{"factor-avg",required_argument, 0, 'o'},
			{"config-file", required_argument, 0, 'p'},
			{"config-path", required_argument, 0, 'q'},
			{"nthreads",  required_argument, 0, 'r'},
			{"label",  required_argument, 0, 's'},
			{"snrth",  required_argument, 0, 't'},
			{"channels",  required_argument, 0, 'u'},
			{"min-hei",  required_argument, 0, 'v'},
			{"max-hei",  required_argument, 0, 'w'},
			{"cal-channel",  required_argument, 0, 'x'},
			{"cal-phase-step",  required_argument, 0, 'y'},
			{"cal-auto-sequence",  required_argument, 0, 'z'},
			{"cal-nsteps",  required_argument, 0, '1'},
			{"procKey", required_argument, 0, 'pk'},
            {0, 0, 0, 0}
          };
        //* getopt_long stores the option index here.

	textcolor(RESET, RED, WHITE);
    if (argc<4){
		printf("Too few arguments:\n");
		textcolor(RESET, BLUE, WHITE);
		printf("%s\n",cmd);
		textcolor(RESET, BLACK, WHITE);
		end_program(true);
    }

    while (1)
    {
    	c = getopt_long_only (argc, argv, "",
    			long_options, &option_index);

    	//* Detect the end of the options.
    	if (c == -1)
    		break;

    	switch (c){
    	case 0:
    		//* If this option set a flag, do nothing else now.
    		if (long_options[option_index].flag != 0)
    			break;
    		printf ("option %s", long_options[option_index].name);
    		if (optarg)
    			printf (" with arg %s", optarg);
    		printf ("\n");
    		break;
    	case 'a':
    		sprintf(opt->dpath,"%s",optarg);
    		break;
    	case 'b':
    		strcpy(opt->ppath,optarg);
    		break;
    	case 'c':
    		opt->startYear = atoi(optarg);
    		break;
    	case 'd':
    		opt->startDoy = atoi(optarg);
    		break;
    	case 'e':
    		opt->startHour = atoi(optarg);
    		break;
    	case 'f':
    		opt->startMin = atoi(optarg);
    		break;
    	case 'g':
    		opt->endYear = atoi(optarg);
            break;
    	case 'h':
    		opt->endDoy = atoi(optarg);
            break;
    	case 'i':
    		opt->endHour = atoi(optarg);
            break;
    	case 'j':
    		opt->endMin = atoi(optarg);
            break;
    	case 'k':
    		opt->set = atoi(optarg);
            break;
    	case 'l':
    		opt->navg = atoi(optarg);
            break;
    	case 'm':
    		opt->nbavg = atoi(optarg);
            break;
    	case 'n':
        	opt->dc_mode = atoi(optarg);
            break;
    	case 'o':
    		opt->fact_avg = atof(optarg);
            break;
    	case 'p':
    		strcpy(opt->config_file,optarg);
            break;
    	case 'q':
    		strcpy(opt->config_path,optarg);
            break;
    	case 'r':
    		opt->nthreads = atoi(optarg);
            break;
    	case 's':
    		strcpy(opt->label,optarg);
            break;
    	case 't':
    		opt->snr_th = atof(optarg);
			break;
    	case 'u':

    		strcpy(__channelStr,optarg);
    		strcpy(__tmp,__channelStr);

    		opt->nChannels = n_elements(__tmp);

    		FillUIntArray(__channelStr, opt->channels, opt->nChannels);

            break;
    	case 'v':
    		opt->minHei = atof(optarg);
            break;
    	case 'w':
    		opt->maxHei = atof(optarg);
            break;
    	case 'x':
    		opt->cal_channel = atoi(optarg);
            break;
    	case 'y':
			opt->cal_phase_step = atof(optarg);
			break;
    	case 'z':

    		strcpy(__channelStr,optarg);
    		strcpy(__tmp,__channelStr);

    		opt->autocal_max_nchannels = n_elements(__tmp);

    		FillUIntArray(__channelStr, opt->autocal_chan_sequence, opt->autocal_max_nchannels);

            break;
    	case '1':
			opt->cal_nphases = atoi(optarg);
			break;
    	case ':':
			//* getopt_long already printed an error message.
    		textcolor(RESET, BLACK, WHITE);
			end_program(true);
			break;
			case 'pk':
			opt->proc_key = atoi(optarg);
			break;
    	case '?':
    		//* getopt_long already printed an error message.
    		textcolor(RESET, BLACK, WHITE);
    		end_program(true);
            break;
    	default:
    		abort ();
    	}
    }

	//*********************************************************
	//**************VERIFY PARMS ******************************
    DIR *dp;
    int finish = 0;

	if((dp = opendir(opt->dpath)) == NULL){
		printf("Data directory doesn't exist: %s\n", opt->dpath);
		finish = 1;
	}
	else
		closedir(dp);

	if ((dp = opendir(opt->ppath)) == NULL){
		mkdir(opt->ppath,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if((dp = opendir(opt->ppath)) == NULL){
			printf("Processed directory to store data cannot be created: %s\n", opt->ppath);
			finish = 1;
		}
		else{
			sprintf(tmppath,"%s/%s/", opt->ppath, BINPATH);
				if((dp = opendir(tmppath)) == NULL){
					printf("Bin directory to store processing data cannot be created: %s\n", tmppath);
					end_program(true);
				}
				closedir(dp);
		}
	}
	if (dp != NULL) closedir(dp);

	if((opt->online_flag) && (opt->set== -100))
		opt->set = -1;

	if (opt->set==-100)
		opt->set=-2;

	if(strcmp(opt->config_file,"NULL")==0){
		printf("Configure-file is not defined\n");
		finish = 1;
	}
	if((opt->startYear<1900) || (opt->startYear>3000)){
		printf("startYear value is out of range\n");
		finish = 1;
	}
	if((opt->startDoy<1) || (opt->startDoy>366)){
		printf("startDoy value is out of range\n");
		finish = 1;
	}
	if((opt->set<-2) || (opt->set>999)){
		printf("Number of set is out of range\n");
		finish = 1;
	}
	if((opt->navg<1) || (opt->navg>100)){
		printf("Average value is out of range\n");
		finish = 1;
	}
	if((opt->nbavg<1) || (opt->nbavg>32)){
		printf("Beam average value is out of range\n");
		finish = 1;
	}
	if((opt->dc_mode<0) || (opt->dc_mode>5)){
		printf("DC mode value is out of range\n");
		finish = 1;
	}
	if((opt->fact_avg<0.0) || (opt->fact_avg>10.0)){
		printf("Average scale factor is out of range\n");
		finish = 1;
	}
	if((opt->nthreads<1) ||  ( (opt->nthreads) > (nCPUs) ) ){
		printf("Number of threads is out of range, it should be between [1-%d]\n",nCPUs);
//		finish = 1;
	}

	if(opt->endYear==0)
		opt->endYear = opt->startYear;

	if(opt->endDoy==0)
		opt->endDoy = opt->startDoy;

	if((opt->endYear<1900) || (opt->endYear>3000)){
		printf("endYear value is out of range\n");
		finish = 1;
	}

	if((opt->endDoy<1) || (opt->endDoy>366)){
		printf("endDoy value is out of range\n");
		finish = 1;
	}

	if(opt->startHour>24){
		printf("startHour value is out of range\n");
		finish = 1;
	}

	if(opt->endHour>48){
		printf("endHour value is out of range\n");
		finish = 1;
	}

	if(opt->startMin>60){
		printf("startMin value is out of range\n");
		finish = 1;
	}

	if(opt->endMin>10000){
		printf("endMin value is out of range\n");
		finish = 1;
	}

	if(opt->maxHei > 10000){
		printf("endMin value is out of range\n");
		finish = 1;
	}

	for(unsigned int i=0; i<opt->nChannels; i++){
		if (opt->channels[i]>20){
			printf("channel selected greater than 20, channel[%d]=%d\n", i, opt->channels[i]);
			finish = 1;
			break;
		}
	}

	if((opt->proc_key<1) || (opt->proc_key>500)){
		printf("process Key value is out of range\n");
		finish = 1;
	}


	if (finish==1){
		textcolor(RESET, BLUE, WHITE);
		printf("%s\n",cmd);
		textcolor(RESET, BLACK, WHITE);
		end_program(true);
	}
	textcolor(RESET, BLACK, WHITE);

	return opt;
}

}
