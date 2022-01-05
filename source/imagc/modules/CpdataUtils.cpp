/*
 * CpdataUtil.cpp
 *
 *  Created on: Mar 2, 2010
 *      Author: murco
 */

#include "CpdataUtils.h"

CpdataUtil::CpdataUtil() {
	// TODO Auto-generated constructor stub
	shift = 0;
	allocatedMem = false;
	allocatedNoiseMem = false;
	allocatedPhaseMem = false;
}

CpdataUtil::~CpdataUtil() {
	// TODO Auto-generated destructor stub
	if(allocatedMem == true)
	{
		del2dArrF(pSelfSpect,nChannels);
		del2dArrC(pCrossSpect,nPairs);
		del2dArrC(pDcSpect,nChannels);
		delete [] pDcPerChannel;
		delete [] pNoise;
		delete [] pPhase;
		allocatedMem = false;
		allocatedNoiseMem = false;
		allocatedPhaseMem = false;
	}
}

void CpdataUtil::setArrayParms(UINT nChan, UINT *CHANNELS, UINT nPair, CROSS *CROSS_PAIR,
		UINT nProf, UINT nSamp, float h0, float dh,
		UINT nIncohInt, UINT nCohInt,
		UINT nBeamAverage, UINT nPostAvg, UINT shiftFft){

	nChannels = nChan;
	nPairs = nPair;
	nProfiles = nProf;
	nSamples = nSamp;
	hei0 = h0;
	deltaHei = dh;
	nIncoherentInt = nIncohInt*nPostAvg;
	nCoherentInt = nCohInt;
	nBeamAvg = nBeamAverage;
	shift = shiftFft;
	channels = CHANNELS;
	crossPairs = CROSS_PAIR;

	nFftPoints = nProfiles/nBeamAvg;
	sizeBlockperChan = nSamples*nFftPoints;

	CpdataUtil::allocatedMemory();
}

void CpdataUtil::allocatedMemory(){

	pSelfSpect = new float*[nChannels];
	pCrossSpect = new fcomplex*[nPairs];
	pDcSpect = new fcomplex*[nChannels];
	pDcPerChannel = new fcomplex[nChannels];

	for(UINT i=0; i<nChannels; i++)
	{
		pDcSpect[i] = new fcomplex[nSamples];
		pSelfSpect[i] = new float[sizeBlockperChan];

		if (pSelfSpect[i] == NULL) exit(0);
		if (pDcSpect[i] == NULL) exit(0);
	}

	for(UINT i=0; i<nPairs; i++)
	{
		pCrossSpect[i] = new fcomplex[sizeBlockperChan];

		if (pCrossSpect[i] == NULL)	exit(0);
	}

	allocatedMem = true;
}

void CpdataUtil::cleanArrays(){

	if(allocatedMem == false) return;

	ltime = 0;

	for(UINT i=0; i<nChannels; i++)
	{
		memset(pSelfSpect[i],0,sizeBlockperChan*sizeof(float));
		memset(pDcSpect[i],0,nSamples*sizeof(fcomplex));
	}

	for(UINT i=0; i<nPairs; i++)
	{
		memset(pCrossSpect[i],0,sizeBlockperChan*sizeof(fcomplex));
	}
}

void CpdataUtil::avgData(float** pSelf, fcomplex** pCross, fcomplex** pDc, UINT utime){
	time_t totalsec;
	tm *timeinfo;
	int indProf;
	float amp=1.0;

	if(allocatedMem == false) return;
	//*************************************************
	if(ltime == 0)
	{
		ltime = utime;
		totalsec  = (time_t)(ltime);
	    timeinfo = localtime(&totalsec);
	    year = timeinfo->tm_year+1900;
	    doy = timeinfo->tm_yday+1;
	    hour = timeinfo->tm_hour;
	    min = timeinfo->tm_min;
	    sec  = timeinfo->tm_sec;
	    htime = timeinfo->tm_hour + timeinfo->tm_min/60 + timeinfo->tm_sec/3600;
	}

	//Averaging self spectra by beam
	for(UINT i=0; i<nChannels; i++)
	{
		for(UINT j=0; j<nSamples; j++)
		{
			//nFftPoints es el numero de perfiles ya promediados: nFftPoints = nProfiles/nBeamAvg
			for(UINT k=0; k<nFftPoints; k++)
			{
				if (nBeamAvg == 1)
				{
					pSelfSpect[i][j*nFftPoints+k] += pSelf[i][j*nFftPoints+k];
				}
				else
				{
					//printf("Puntos acumulados en [%d] = ", j*nFftPoints+k);
					for(int l=-(int)nBeamAvg/2; l<(int)nBeamAvg/2+1; l++)
					{
						indProf = k*nBeamAvg+l;
						if(indProf<0) indProf+=nProfiles;
						if(indProf>=(int)nProfiles)	indProf-=(int)nProfiles;

						amp = 1.0;
						if((l==-(int)nBeamAvg/2 || l==(int)nBeamAvg/2))
							amp = 0.5;

						pSelfSpect[i][j*nFftPoints+k] += amp*pSelf[i][j*nProfiles+indProf];
					}
				}
			}
		}
	}

	//Averaging cross spectra by beam
	for(UINT i=0; i<nPairs; i++){
		for(UINT j=0; j<nSamples; j++){
			for(UINT k=0; k<nFftPoints; k++){
				if (nBeamAvg == 1)
				{
					pCrossSpect[i][j*nFftPoints+k] = Cadd(pCrossSpect[i][j*nFftPoints+k],RCmul(amp, pCross[i][j*nFftPoints+k]));
				}
				else{
					for(int l=-(int)nBeamAvg/2; l<(int)nBeamAvg/2+1; l++)
					{
						indProf = k*(int)nBeamAvg+l;
						if(indProf<0) indProf+=nProfiles;
						if(indProf>=(int)nProfiles)	indProf-=(int)nProfiles;

						amp = 1.0;
						if((l==-(int)nBeamAvg/2 || l==(int)nBeamAvg/2))
							amp = 0.5;

						pCrossSpect[i][j*nFftPoints+k] = Cadd(pCrossSpect[i][j*nFftPoints+k],RCmul(amp, pCross[i][j*nProfiles+indProf]));
					}
				}

			}
		}
	}

	//Averaging DC
	for(UINT i=0; i<nChannels; i++)
	{
		for(UINT j=0; j<nSamples; j++)
		{
			pDcSpect[i][j] = Cadd(pDcSpect[i][j],pDc[i][j]);
			pDcPerChannel[i] = Cadd(pDcPerChannel[i],pDc[i][j]);
		}
	}

	//Scale DC per channel
	for(UINT i=0; i<nChannels; i++)
	{
		pDcPerChannel[i] = RCdiv(pDcPerChannel[i],(float)nSamples);
	}

}
float* CpdataUtil::log10Array(float *pArray, int nx){

	float *pLog10Array = new float[nx];

	for(int i=0; i<nx; i++)
	{
		pLog10Array[i] = 10*log10(pArray[i]);
	}

	return pLog10Array;

}
float* CpdataUtil::getNoise(){
	//Allocate memory to noise
	if(allocatedNoiseMem == false)
	{
		pNoise = new float[nChannels];
		allocatedNoiseMem = true;
	}

	//Scale DC per channel
	for(UINT i=0; i<nChannels; i++)
	{
		int ncAvg = nBeamAvg*nIncoherentInt;
		pNoise[i] = enoise(pSelfSpect[i], sizeBlockperChan, ncAvg);
	}

	return(pNoise);
}
float* CpdataUtil::getPhase(UINT minHeight, UINT maxHeight, float* coh){

	fcomplex sumCrossSpect[MAXNUMCHAN];
	float sumSelfSpect[MAXNUMCHAN], bcoh[MAXNUMCHAN], bavgcoh, anorm;
	UINT indRange[2];

	if(allocatedPhaseMem == false)
	{
		pPhase = new float[nChannels];
		allocatedPhaseMem = true;
	}

	//Getting phase from beacon signal
	if(maxHeight <= minHeight){
		for(UINT i=0;i<(nChannels);i++)
			pPhase[i] = 0.0;

		*coh = 0.0;
		return(pPhase);
	}

	memset(sumCrossSpect,0,MAXNUMCHAN*sizeof(fcomplex));
	memset(sumSelfSpect,0,MAXNUMCHAN*sizeof(float));
	memset(pPhase,0,nChannels*sizeof(float));

	bavgcoh = 0.0;
	bcoh[0] = 0.0;

	indRange[0] = (minHeight - hei0)/deltaHei;
	indRange[1] = (maxHeight - hei0)/deltaHei;

	if(indRange[1]>nSamples)
	{
		printf("Limit of height range exceed, estimating phase is not possible\n");
		return(pPhase);
	}

	for(UINT i=0;i<(nChannels-1);i++)
	{

		for(UINT j=indRange[0];j<(indRange[1]+1);j++)
		{
			for(UINT k=0;k<nFftPoints;k++)
			{
				//Avoiding averaging DC when flip is used
				//if(k == (nFftPoints/2)*(1-shift)) continue;

				if(i==0) sumSelfSpect[0] += pSelfSpect[0][j*nFftPoints+k];
				sumSelfSpect[i+1] += pSelfSpect[i+1][j*nFftPoints+k];
				sumCrossSpect[i]=Cadd(sumCrossSpect[i],pCrossSpect[i][j*nFftPoints+k]);
				//printf("real=%f, imag=%f\n",pCrossSpect[i][j*nFftPoints+k].r,pCrossSpect[i][j*nFftPoints+k].i);

			}
		}

		pPhase[i+1] = -atan2(sumCrossSpect[i].i,sumCrossSpect[i].r);
		//printf("SumSelf(*) = [%10.4g %10.4g] \n", sumSelfSpect[0], sumSelfSpect[i+1]);
		anorm = sqrt(sumSelfSpect[0]*sumSelfSpect[i+1]);
		//printf("SumCross = [%10.4g %10.4g] Anorm = %10.4g \n", sumCrossSpect[i].i, sumCrossSpect[i].r,anorm);
		bcoh[i+1] = Cabs(RCdiv(sumCrossSpect[i],anorm));
		//printf("bcoh %f \n", bcoh[i+1]);
		bavgcoh += bcoh[i+1];
	}

	*coh = bavgcoh/((float)(nChannels-1));

	//printf("Averaged beacon coherence: %f \n",bavgcoh);
	/*if(bavgcoh <= 0.5)
	{
		//printf("Using previous beacon phase \n");
		//for(UINT i=0;i<nChannels;i++)
			//pPhase[i] = 0;
			//phase[i] = dphase[i]+pPhase[i];

	}*/

	/*
	printf("Instantaneous phases \n");
	for(UINT i=0;i<nchan;i++)
		printf("%7.2f,",pha[i]);
	printf("\n");
	*/

	return(pPhase);
}
void CpdataUtil::fixPhase(float* phase){
	fcomplex cpha;
	UINT i,j,k=0;

	/*int ind1[nPairs],ind2[nPairs];
	for(i=0;i<(nChannels-1);i++)
	{
		for(j=i+1;j<nChannels;j++)
		{
			ind1[k] = j;
			ind2[k] = i;
			k = k + 1;
		}
	}*/

	//Fixing data (phase correction)
	for(i=0;i<nPairs;i++)
	{
		if ( (phase[crossPairs[i][0]] == 0.0) && (phase[crossPairs[i][1]] == 0.0) )
			continue;

		for(j=0;j<nSamples;j++)
		{
			for(k=0;k<nFftPoints;k++)
			{
				//cpha = Cmul(Complex(cos(phase[ind2[i]]),sin(-phase[ind2[i]])),Complex(cos(phase[ind1[i]]),sin(phase[ind1[i]])));
				cpha = Cmul(Complex(cos(phase[crossPairs[i][0]]),sin(-phase[crossPairs[i][0]])),
						    Complex(cos(phase[crossPairs[i][1]]),sin(+phase[crossPairs[i][1]])));
				pCrossSpect[i][j*nFftPoints+k] = Cmul(pCrossSpect[i][j*nFftPoints+k],cpha);
			}
		}
	}
}

//idData* CpdataUtil::filterBySnr(snrth){
//
//	//allow memory for pIdData
//	pIdData = new struct idData[nfft*nHeis];
//
//	ySize = nFftPoints*nSamples;
//
//	//Averaged noise
//	avgNoise = 0;
//	for(int c=0;c<nchan;c++)
//		avgNoise += pNoise[c];
//
//	avgNoise /= (float)nChannels;
//
//	//Signal with snr higher than snrThr
//	ndata = 0;
//	for(i=0;i<nFftPoints;i++)
//	{
//		for(j=indHei[0];j<=indHei[1];j++)
//		{
//			avgSignal = 0;
//			iy = j*nfft+i;
//			for(k=0;k<nchan;k++)
//				avgSignal += pSelfSpect[k][iy] - noise[k];
//
//			avgSignal /= (float)nchan;
//
//			if(avgSignal < 0.0)
//				avgSignal = 1.0e-10;
//
//			//Estimating S/N
//			snr = avgSignal/avgNoise;
//
//			if(snr > snrThr){
//				pIdData[ndata].i = i;
//				pIdData[ndata].j = j;
//				pIdData[ndata].snr = snr;
//				ndata++;
//			}
//		}
//	}
//}

//void getArgs(int argc, char *argv[], char *dpath, char *ppath, UINT *sYear, UINT *sDoy, UINT *sHour, UINT *sMin,
//			int *set, UINT *eYear, UINT *eDoy, UINT *eHour, UINT *eMin,
//			int *navg, int *nbavg, int *dc_mode, float *fact_avg,
//			char *configpath, char *configfile,
//			int *online_flag, int *plotting_flag, int *nthreads, char *label){
//
//	//ARGUMENTS
//    int c;
//    int option_index = 0;
//    int nCPUs;
//    char tmppath[100];
//	char cmd[] = "Usage: procImaging --dpath=<dpath> --ppath=<ppath> --startYear=<startYear> --startDoy=<startDoy>\n"
//				 "\t\t  --startHour=<startHour> --startMin=<startMin> || --set=<set>\n"
//				 "\t\t  --endYear=<endYear> --endDoy=<endDoy> --endHour=<endHour> --endMin=<endMin>\n"
//				 "\t\t  --navg=<navg> --nbavg=<nbavg> --dc-mode=<dc-mode> --factor-avg=<factor-avg>\n"
//				 "\t\t  --config-path=<config-path> --config-file=<config-file> --nthreads=<nthreads> --online --plotting\n\n"
//				 "This program gets IMAGES from pdata files on <dpath> and save them on <ppath>\n\n"
//				 "Required arguments:\n"
//				 "--dpath\t\t\t: Data directory, directory where the pdata files are stored\n"
//				 "--ppath\t\t\t: Processing directory, directory where the processed files will be saved\n"
//				 "--startYear,startDoy\t: Year and day to process.\n"
//				 "\nOptional arguments:\n"
//				 "--endYear, endDoy\t: End date to process. Use these arguments to process a range of days \n"
//				 "--startHour,startMin\t: Start time to process. By default is [00,00]\n"
//				 "--endHour, endMin\t: End time to process. By default is [24,00]\n"
//				 "--set\t\t\t: Initial set. Setting set=-2 to begin with the first file of the day,-1 to the last file.\n"
//				 "--config-file\t\t: Configuration file used to phase calibration. By default is 'auto'; in this mode\n"
//				 "\t\t\t  it looks for the nearest configuration file, otherwise it is set to 'configure.txt'.\n"
//				 "\t\t\t  The file format is configYYYYDDD.txt, where YYYY = year and DDD = doy.\n"
//				 "--config-path\t\t: Configuration files must be located on this directory. By default is './config-files'\n"
//				 "--navg\t\t\t: Incoherent integrations. By default is 1\n"
//				 "--nbavg\t\t\t: Beam integrations. By default is 1\n"
//				 "--dc-mode\t\t: DC remotion. By default is 0 (disabled)\n"
//				 "--factor-avg\t\t: Integration factor. By default is 1\n"
//				 "--online\t\t: Setting this option to online process, it overwrite 'set'=-1. By default is disabled\n"
//				 "--plotting\t\t: Setting this option to display self spectra. By default is disabled\n"
//				 "--nthreads\t\t: Number of threads to use [0-nCPUs]. By default is equal to the number of processors (nCPUs)\n"
//				 "--label\t\t\t: sub-folder inside doy folder where the pdata has been saved. By default is IMAGING\n\n"
//				 "Example: (If the first file is /data/d2010040/IMAGING/P2010040225.pdata)\n"
//				 "\t >>procImaging --dpath=/data --ppath=/data/BIN --startYear=2010 --startDoy=40 --set=225(or set=-2) --plotting";
//
//
//	*set = -100;
//	*sYear = *sDoy = *eYear = *eDoy = 0;
//	*sHour = 0; *sMin = 0;
//	*eHour = 24; *eMin = 0;
//	*navg = *nbavg = 1;
//	*dc_mode = 0;
//	*fact_avg = 1;
//    *online_flag = *plotting_flag = 0;
//    nCPUs = sysconf( _SC_NPROCESSORS_ONLN ); //Getting number of processors available
//    *nthreads = nCPUs;
//    strcpy(configpath,CONFIGPATH);
//    strcpy(configfile,"auto");
//    strcpy(label,"IMAGING");
//
//	//Getting parameters of configuration
//	//system("clear");
//
//	static struct option long_options[] =
//          {
//            // These options set a flag.
//            {"online",    no_argument,   online_flag, 1},
//            {"plotting",  no_argument, plotting_flag, 1},
//            // These options don't set a flag.
//            //   We distinguish them by their indices.
//            {"dpath",     required_argument, 0, 'p'},
//            {"ppath",     required_argument, 0, 'r'},
//            {"startYear", required_argument, 0, 'y'},
//            {"startDoy",  required_argument, 0, 'd'},
//            {"startHour", required_argument, 0, 'h'},
//            {"startMin",  required_argument, 0, 'm'},
//            {"set",       required_argument, 0, 's'},
//            {"endYear",   required_argument, 0, 'z'},
//			{"endDoy",    required_argument, 0, 'e'},
//			{"endHour",   required_argument, 0, 'i'},
//			{"endMin",    required_argument, 0, 'n'},
//			{"navg",      required_argument, 0, 'a'},
//			{"nbavg",     required_argument, 0, 'b'},
//			{"dc-mode",   required_argument, 0, 'c'},
//			{"factor-avg",required_argument, 0, 'x'},
//			{"config-file", required_argument, 0, 'f'},
//			{"config-path", required_argument, 0, 'g'},
//			{"nthreads",  required_argument, 0, 't'},
//			{"label",  required_argument, 0, 'l'},
//            {0, 0, 0, 0}
//          };
//        //* getopt_long stores the option index here.
//
//	textcolor(RESET, RED, WHITE);
//    if (argc<4){
//		printf("Too few arguments:\n");
//		textcolor(RESET, BLUE, WHITE);
//		printf("%s\n",cmd);
//		textcolor(RESET, BLACK, WHITE);
//		end_program(true);
//    }
//
//    while (1)
//    {
//    	c = getopt_long (argc, argv, "p:r:y:d:h:m:s:z:e:i:n:a:b:c:x:f:",
//    			long_options, &option_index);
//
//    	//* Detect the end of the options.
//    	if (c == -1)
//    		break;
//
//    	switch (c){
//    	case 0:
//    		//* If this option set a flag, do nothing else now.
//    		if (long_options[option_index].flag != 0)
//    			break;
//    		printf ("option %s", long_options[option_index].name);
//    		if (optarg)
//    			printf (" with arg %s", optarg);
//    		printf ("\n");
//    		break;
//    	case 'p':
//    		sprintf(dpath,"%s",optarg);
//    		break;
//    	case 'r':
//    		strcpy(ppath,optarg);
//    		break;
//    	case 'y':
//    		*sYear = atoi(optarg);
//    		break;
//    	case 'd':
//    		*sDoy = atoi(optarg);
//    		break;
//    	case 'h':
//    		*sHour = atoi(optarg);
//    		break;
//    	case 'm':
//    		*sMin = atoi(optarg);
//    		break;
//    	case 's':
//    		*set = atoi(optarg);
//            break;
//    	case 'z':
//    		*eYear = atoi(optarg);
//            break;
//    	case 'e':
//    		*eDoy = atoi(optarg);
//            break;
//    	case 'i':
//    		*eHour = atoi(optarg);
//            break;
//    	case 'n':
//    		*eMin = atoi(optarg);
//            break;
//    	case 'a':
//    		*navg = atoi(optarg);
//            break;
//    	case 'b':
//    		*nbavg = atoi(optarg);
//            break;
//    	case 'c':
//        	*dc_mode = atoi(optarg);
//            break;
//    	case 'x':
//    		*fact_avg = atof(optarg);
//            break;
//    	case 'f':
//    		strcpy(configfile,optarg);
//            break;
//    	case 'g':
//    		strcpy(configpath,optarg);
//            break;
//    	case 't':
//    		*nthreads = atoi(optarg);
//            break;
//    	case 'l':
//    		strcpy(label,optarg);
//            break;
//    	case '?':
//    		//* getopt_long already printed an error message.
//            break;
//    	default:
//    		abort ();
//    	}
//    }
//
//	//*********************************************************
//	//**************VERIFY PARMS ******************************
//    DIR *dp;
//    int finish = 0;
//
//	if((dp = opendir(dpath)) == NULL){
//		printf("Data directory doesn't exist: %s\n", dpath);
//		finish = 1;
//	}
//	else
//		closedir(dp);
//
//	if ((dp = opendir(ppath)) == NULL){
//		mkdir(ppath,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
//		if((dp = opendir(ppath)) == NULL){
//			printf("Processed directory to store data cannot be created: %s\n", ppath);
//			finish = 1;
//		}
//		else{
//			sprintf(tmppath,"%s/%s/", ppath, BINPATH);
//				if((dp = opendir(tmppath)) == NULL){
//					printf("Bin directory to store processing data cannot be created: %s\n", tmppath);
//					end_program(true);
//				}
//				closedir(dp);
//		}
//	}
//	if (dp != NULL) closedir(dp);
//
//	if((dp = opendir(configpath)) == NULL){
//		printf("This directory doesn't exist: %s\n", configpath);
//		finish = 1;
//	}
//	else
//		closedir(dp);
//
//	if((*online_flag) && (*set== -100))
//		*set = -1;
//
//	if (*set==-100)
//		*set=-2;
//
//	if(strcmp(configfile,"NULL")==0){
//		printf("Configure-file is not defined\n");
//		finish = 1;
//	}
//	if((*sYear<1900) || (*sYear>3000)){
//		printf("startYear value is out of range\n");
//		finish = 1;
//	}
//	if((*sDoy<1) || (*sDoy>366)){
//		printf("startDoy value is out of range\n");
//		finish = 1;
//	}
//	if((*set<-2) || (*set>999)){
//		printf("Number of set is out of range\n");
//		finish = 1;
//	}
//	if((*navg<1) || (*navg>100)){
//		printf("Average value is out of range\n");
//		finish = 1;
//	}
//	if((*nbavg<1) || (*nbavg>32)){
//		printf("Beam average value is out of range\n");
//		finish = 1;
//	}
//	if((*dc_mode<0) || (*dc_mode>5)){
//		printf("DC mode value is out of range\n");
//		finish = 1;
//	}
//	if((*fact_avg<0.0) || (*fact_avg>10.0)){
//		printf("Average scale factor is out of range\n");
//		finish = 1;
//	}
//	if((*nthreads<1) || (*nthreads>nCPUs)){
//		printf("Number of threads is out of range, it should be between [1-%d]\n",nCPUs);
//		finish = 1;
//	}
//
//	if(*eYear==0)
//		*eYear = *sYear;
//
//	if(*eDoy==0)
//		*eDoy = *sDoy;
//
//	if((*eYear<1900) || (*eYear>3000)){
//		printf("endYear value is out of range\n");
//		finish = 1;
//	}
//
//	if((*eDoy<1) || (*eDoy>366)){
//		printf("endDoy value is out of range\n");
//		finish = 1;
//	}
//
//	if((*sHour<0) || (*sHour>24)){
//		printf("startHour value is out of range\n");
//		finish = 1;
//	}
//
//	if((*eHour<0) || (*eHour>48)){
//		printf("endHour value is out of range\n");
//		finish = 1;
//	}
//
//	if((*sMin<0) || (*sMin>60)){
//		printf("startMin value is out of range\n");
//		finish = 1;
//	}
//
//	if((*eMin<0) || (*eMin>60)){
//		printf("endMin value is out of range\n");
//		finish = 1;
//	}
//
//	if (finish==1){
//		textcolor(RESET, BLUE, WHITE);
//		printf("%s\n",cmd);
//		textcolor(RESET, BLACK, WHITE);
//		end_program(true);
//	}
//	textcolor(RESET, BLACK, WHITE);
//}
