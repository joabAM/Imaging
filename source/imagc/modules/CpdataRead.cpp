/*
 * jroFile.cpp
 *
 *  Created on: Jan 18, 2010
 *      Author: murco
 */
#include "CpdataRead.h"

CpdataRead::CpdataRead(){

	readDC=false;
	online=false;
	fileOpened = false;
	allocatedMem = false;
	timeout = 0;
	nAvg = 1;
	nBeamAvg = 1;
	newDoy = true;
	sHeader.sizeDataBlock = 100; //100 bytes minimum
}

CpdataRead::CpdataRead(char *dataPath, int year, int doy, int set, char *expLabel){

	readDC=false;
	online=false;
	fileOpened = false;
	allocatedMem = false;
	timeout = 0;
	nAvg = 1;
	nBeamAvg = 1;
	newDoy = true;
	sHeader.sizeDataBlock = 100; //100 bytes minimum
	this->setDate(dataPath, year, doy, set, 0, 24, 0, 0, expLabel);
}

CpdataRead::~CpdataRead(){
	if(allocatedMem == true){
		del2dArrF(pSelfSpect,sHeader.nChannels);
		del2dArrC(pCrossSpect,sHeader.nPairs);
		del2dArrC(pDcSpect,sHeader.nChannels);
		delete [] pDcPerChannel;
		delete [] pNoise;
		allocatedMem = false;
	}
}

int CpdataRead::setDate(char *dataPath, int year, int doy, int set, int startHour, int endHour, int startMin, int endMin, char *expLabel){
	int sts, nskip=0;
	time_t seconds;
	struct tm time_str;
	unsigned int thisTime;

	if(allocatedMem == true){
		del2dArrF(pSelfSpect,sHeader.nChannels);
		del2dArrC(pCrossSpect,sHeader.nPairs);
		del2dArrC(pDcSpect,sHeader.nChannels);
		delete [] pDcPerChannel;
		delete [] pNoise;
		allocatedMem = false;
	}

	strcpy(path,dataPath);
	strcpy(label,expLabel);
	yy = year;
	dd = doy;
	ss = set;

	if (ss==-1){
		online=true;
		timeout=30;
	}


	nTotalBlocksRead = 0;

	time_str.tm_year = yy - 1900;
	time_str.tm_mon = 0;
	time_str.tm_mday = dd;
	time_str.tm_hour = startHour;
	time_str.tm_min = startMin;
	time_str.tm_sec = 0;
	time_str.tm_isdst = -1;
	seconds = mktime(&time_str);
	startTime = (unsigned int)(seconds);

	time_str.tm_year = yy - 1900;
	time_str.tm_mon = 0;
	time_str.tm_mday = dd;
	time_str.tm_hour = endHour;
	time_str.tm_min = endMin;
	time_str.tm_sec = 0;
	time_str.tm_isdst = -1;
	seconds = mktime(&time_str);
	endTime = (unsigned int)(seconds);

	//Find out the right file between startTime and endTime
	while(1){
		sts = this->setFilename();
		if(online) break; //Online process
		if(sHeader.ltime>=startTime) break;

		//Si el directorio de datos no existe o si mno hay archivos en el directorio
		if((sts==0) or (sts==-1)){
			newDoy = true;
			dd += 1;
			ss=-2;		//Empezar a leer desde el primer archivo
			if(dd>366) {yy++; dd=1;}
			time_str.tm_year = yy - 1900;
			time_str.tm_mon = 0;
			time_str.tm_mday = dd;
			time_str.tm_hour = 0;
			time_str.tm_min = 0;
			time_str.tm_isdst = -1;
			seconds = mktime(&time_str);
			thisTime = (unsigned int)(seconds);
			if(thisTime>=endTime) break;
			continue;
		}

		//Si el archivo buscado no existe, slatar al siguiente
		if(sts==-2){
			if (nskip<3){
				nskip += 1;
				continue;
			}
			break;
		}

		ss++;

	}

	if(sHeader.ltime>endTime){
		printf("Data is out of time range\n");
		return(0);
	}

	if(sts!=1){
		printf("There are no more files in the selected time range\n");
		return(0);
	}

	this->allocatedMemory();
	return(1);
}

int CpdataRead::setHeader(char *dataPath, int year, int doy, int set, int startHour, int endHour, int startMin, int endMin, char *expLabel, bool readOnline){

	return this->setDate(dataPath, year, doy, set, startHour, endHour, startMin, endMin, expLabel, readOnline);
}
int CpdataRead::setDate(char *dataPath, int year, int doy, int set, int startHour, int endHour, int startMin, int endMin, char *expLabel, bool readOnline){
	int sts, nskip=0;
	time_t seconds;
	struct tm time_str;
	unsigned int thisTime;

	if(allocatedMem == true){
		del2dArrF(pSelfSpect,sHeader.nChannels);
		del2dArrC(pCrossSpect,sHeader.nPairs);
		del2dArrC(pDcSpect,sHeader.nChannels);
		delete [] pDcPerChannel;
		delete [] pNoise;
		allocatedMem = false;
	}

	strcpy(path,dataPath);
	strcpy(label,expLabel);
	yy = year;
	dd = doy;
	ss = set;

	if (readOnline==true){
		online=true;
		timeout=60;
	}


	nTotalBlocksRead = 0;

	time_str.tm_year = yy - 1900;
	time_str.tm_mon = 0;
	time_str.tm_mday = dd;
	time_str.tm_hour = startHour;
	time_str.tm_min = startMin;
	time_str.tm_sec = 0;
	time_str.tm_isdst = -1;
	seconds = mktime(&time_str);
	startTime = (unsigned int)(seconds);

	time_str.tm_year = yy - 1900;
	time_str.tm_mon = 0;
	time_str.tm_mday = dd;
	time_str.tm_hour = endHour;
	time_str.tm_min = endMin;
	time_str.tm_sec = 0;
	time_str.tm_isdst = -1;
	seconds = mktime(&time_str);
	endTime = (unsigned int)(seconds);

	//Find out the right file between startTime and endTime
	while(1){
		sts = this->setFilename();
		if(online) break; //Online process
		if(sHeader.ltime>=startTime) break;

		//Si el directorio de datos no existe o si no hay archivos en el directorio pasar al siguiente dia
		if((sts==0) or (sts==-1)){
			newDoy = true;
			dd += 1;
			ss=-2;		//Empezar a leer desde el primer archivo
			if(dd>366) {yy++; dd=1;}
			time_str.tm_year = yy - 1900;
			time_str.tm_mon = 0;
			time_str.tm_mday = dd;
			time_str.tm_hour = 0;
			time_str.tm_min = 0;
			time_str.tm_isdst = -1;
			seconds = mktime(&time_str);
			thisTime = (unsigned int)(seconds);
			if(thisTime>=endTime) break;
			continue;
		}

		//Si el archivo buscado no existe, saltar al siguiente
		if(sts==-2){
			if (nskip>999)
				break;
		}

		ss++;
		nskip += 1;

	}

	if(sHeader.ltime>endTime){
		printf("Data is out of time range\n");
		return(0);
	}

	if(sts!=1){
		printf("There are no more files in the selected time range\n");
		return(0);
	}

	this->allocatedMemory();
	return(1);
}

void CpdataRead::allocatedMemory(){
	bool sts=true;

	sizeBlockperChan = sHeader.nSamples*sHeader.nFftPoints;


	pSelfSpect = new float*[sHeader.nChannels];
	pCrossSpect = new fcomplex*[sHeader.nPairs];
	pDcSpect = new fcomplex*[sHeader.nChannels];
	pDcPerChannel = new fcomplex[sHeader.nChannels];
	pNoise = new float[sHeader.nChannels];

	for(UINT i=0; i<sHeader.nChannels; i++){
		pDcSpect[i] = new fcomplex[sHeader.nSamples];
		pSelfSpect[i] = new float[sizeBlockperChan];

		if (pSelfSpect[i] == NULL) {sts=false;}
		if (pDcSpect[i] == NULL) {sts=false;	}
	}

	for(UINT i=0; i<sHeader.nPairs; i++){
		pCrossSpect[i] = new fcomplex[sizeBlockperChan];

		if (pCrossSpect[i] == NULL) {sts=false;}
	}

	allocatedMem = true;
	if (sts==false){
		printf("Error allocating memory\n");
		exit(0);
	}
}

int CpdataRead::setAverageData(int nAverages, int nBeamAverages){

	nAvg = nAverages;
	nBeamAvg = nBeamAverages;
	return(0);
}

int CpdataRead::readHeader(void){
	//read short and long header
	sHeader.nBlocksRead = 0;
	firstBlock = true;
	if (!this->readShortHeader()){return(0);}
	if (!this->readLongHeader()){return(0);}

	return(1);
}

int CpdataRead::readShortHeader(void){

	struct shortHeader pShortHeader;
	long currPos, sizeFile;
//	bool msg=true;

	currPos = ftell(fp);

	//Updating status file
	fclose(fp);
	fp = fopen(fullfilename,"r");

	fseek(fp,0,SEEK_END);
	sizeFile = ftell(fp);
	fseek(fp,currPos,SEEK_SET);

	pShortHeader.length = 0;

	//read short header
	fread(&pShortHeader,sizeof(unsigned int),1,fp);
    fread(&pShortHeader.version,sizeof(unsigned short),1,fp);
	fread(&pShortHeader.currentBlock,sizeof(unsigned int),1,fp);
    fread(&pShortHeader.utime,sizeof(int),1,fp);
    fread(&pShortHeader.msec,sizeof(unsigned short),1,fp);
    fread(&pShortHeader.timeZone,sizeof(short),1,fp);
    fread(&pShortHeader.dst,sizeof(short),1,fp);
    fread(&pShortHeader.countError,sizeof(unsigned int),1,fp);

    if ((!pShortHeader.length) || (sizeFile < currPos + sHeader.sizeDataBlock + pShortHeader.length)){
    	fseek(fp,currPos,SEEK_SET);
    	return(0);
	}

    sHeader.length = pShortHeader.length;
    sHeader.version = pShortHeader.version;
    sHeader.currentBlock = pShortHeader.currentBlock;
    sHeader.ltime = pShortHeader.utime;
    sHeader.msec = pShortHeader.msec;

	time_t sec;
	tm *timeinfo;
    sec  = (time_t)(sHeader.ltime);
    timeinfo = localtime(&sec);

    //sHeader.timeinfo = timeinfo;
    sHeader.year = timeinfo->tm_year+1900;
    sHeader.doy = timeinfo->tm_yday+1;
    sHeader.hour = timeinfo->tm_hour;
    sHeader.min =  timeinfo->tm_min;
    sHeader.sec =  timeinfo->tm_sec;
	return(1);
}

int CpdataRead::readLongHeader(){

	//Defining variables and pointers
	struct sysHeader pSysHeader;
	struct rcHeader pRcHeader;
	struct winHeader pWinHeaderRC[20], pWinHeaderProc[20];
	struct procHeader pProcHeader;

	unsigned int i, k, nchan, ncross;
	unsigned int nCodes,nBauds,*code;
	unsigned int flip1,flip2;
	float tau;
	unsigned int iant[100], jant[100];
	long currPos;

	//Begin -------->>>>>
	fread(&pSysHeader,sizeof(struct sysHeader),1,fp);

	currPos = ftell(fp);

	fread(&pRcHeader,sizeof(struct rcHeader),1,fp);

	for(i=0; i<pRcHeader.nWindowsRC; i++){
		fread(&pWinHeaderRC[i],sizeof(struct winHeader),1,fp);
	}

	if (pRcHeader.nTaus > 0){
		fread(&tau, pRcHeader.nTaus*sizeof(float),1,fp);
	}

	if(pRcHeader.codeType != 0){
		fread(&nCodes,sizeof(unsigned int),1,fp);
		fread(&nBauds,sizeof(unsigned int),1,fp);

		//code = new unsigned int[nCodes];

		//fread(&code, nCodes*sizeof(unsigned int),1,fp);
		
		//delete [] code;
	}

	if(pRcHeader.line5 == FLIP){
		fread(&flip1,sizeof(unsigned int),1,fp);
	}

	if(pRcHeader.line6 == FLIP){
		fread(&flip2,sizeof(unsigned int),1,fp);
	}

	fseek(fp,currPos+pRcHeader.length, SEEK_SET);

	//read processing information
	currPos = ftell(fp);

	fread(&pProcHeader,sizeof(struct procHeader),1,fp);

	for(i=0; i<pProcHeader.nWindowsProc; i++){
		fread(&pWinHeaderProc[i],sizeof(struct winHeader),1,fp);
	}

	// read cross spectral order
	for(i=0;i<pProcHeader.totalSpectra;i++){
		iant[i]=fgetc(fp);jant[i]=fgetc(fp);
		//printf("(%d,%d)\n",iant[i],jant[i]);
	}

	fseek(fp,currPos+pProcHeader.length, SEEK_SET);

    nchan = 0;
    ncross = 0;

	for(i=0; i<pProcHeader.totalSpectra; i++){
		if (iant[i] == jant[i]) nchan++;
		else ncross++;
	}

    sHeader.nSamples = pWinHeaderProc[0].nsa;; //pSysHeader.nSamples;
    sHeader.nProfiles = pProcHeader.nProfilesPerBlock; //pSysHeader.nProfiles;
    sHeader.nChannels = pSysHeader.nChannels;
    //RC HEADER
    sHeader.expType = pRcHeader.expType;
    sHeader.ipp = pRcHeader.ipp;
    sHeader.txa = pRcHeader.txa;
    sHeader.txb = pRcHeader.txb;
    sHeader.nTaus = pRcHeader.nTaus;
    sHeader.codeType = pRcHeader.codeType;
    sHeader.fClock = pRcHeader.fClock;
    //WINDOW HEADER
    sHeader.h0 = pWinHeaderProc[0].h0;
    sHeader.dh = pWinHeaderProc[0].dh;
    sHeader.nsa = pWinHeaderProc[0].nsa;
	//PROCESS HEADER
    sHeader.dataType = pProcHeader.dataType;
    sHeader.sizeDataBlock = pProcHeader.sizeDataBlock;
    sHeader.nProfilesPerBlock = pProcHeader.nProfilesPerBlock;
    sHeader.nBlocksPerFile = pProcHeader.nBlocksPerFile;
    sHeader.nWindows = pProcHeader.nWindowsProc;
    sHeader.processFlag = pProcHeader.processFlag;
    sHeader.nCoherentInt = pProcHeader.nCoherentInt;
    sHeader.nIncoherentInt = pProcHeader.nIncoherentInt;
    sHeader.nIncoherentInt *= nAvg;
    sHeader.nFftPoints = sHeader.nProfiles/nBeamAvg;
    sHeader.totalSpectra = pProcHeader.totalSpectra;
	sHeader.nChannels = nchan;
    sHeader.nPairs = ncross;
    sHeader.blockDelay = (sHeader.ipp/150*1e-3)*sHeader.nCoherentInt*\
						sHeader.nProfilesPerBlock*sHeader.nIncoherentInt;

    k = 0;
	for(i=0; i<pProcHeader.totalSpectra; i++){
		if (iant[i] == jant[i]){
			sHeader.channels[i] = iant[i];
			//printf("CHAN(%d)\n",sHeader.channels[i]);
		}
		else{
			sHeader.crossPairs[k][0] = iant[i];
			sHeader.crossPairs[k][1] = jant[i];
			//printf("CROSS(%d,%d)\n\n",sHeader.crossPairs[k][0],sHeader.crossPairs[k][1]);
			k++;
		}
	}

	readDC = ((pProcHeader.processFlag & SAVE_CHANNELS_DC) == SAVE_CHANNELS_DC);

	return(1);
}

int CpdataRead::setFilename(){
	/*
	 * Realiza una busqueda de los archivos válidos en el directorio especificado en "path"
	 * 		Si ss = -1 selecciona el ultimo archivo encntrado en el directorio de datos
	 * 		Si ss = -2 selecciona el primer archivo encontrado en el directorio de datos
	 * 		Si ss > 0 el archivo seleccionado es el determinado por "ss"
	 *
	 * Outut:
	 * 		1 : Asigna los valores correspondientes a la metadata asociada al archivo seleccionado.
	 * 		0 : El directorio de datos no existe
	 * 		-1 : No hay archivos en el directorio de datos
	 * 		-2 : El archivo especificado con el set ss no existe.
	 *
	 * */

	DIR *dp;
	unsigned int lenfilename=17; //'P2010125001.pdata'
	char tmppath[140], tmpfile[30], tmpset[30], extension[]="pdata";
	bool retLastFile=true;

	sprintf(tmppath,"%s/d%4.4d%3.3d/%s/", path, yy, dd, label);

	if((dp = opendir(tmppath)) == NULL){
		printf("The directory %s doesn't exist\n", tmppath);
		return(0);
	}
	else closedir(dp);

	if (ss<0){ //Search files (setting set)
		if((ss!=-1) and (ss!=-2)) return(0);
		//Online process
		//if(ss==-1) retLastFile = true;		//Online process (Last file)
		if(ss==-2) retLastFile=false;		//offline process (First file)

		if (not(findFiles(tmppath, extension, retLastFile, tmpfile, lenfilename))){
			printf("There are no files in the directory %s with %s extension\n",tmppath, extension);
			return(-1);
		}
		
		strcpy(tmpset,tmpfile+8);
		ss = atoi(tmpset);
	}

	char ini[] = "P";

	if (fileOpened == true){
		fclose(fp);
	}

	fileOpened = false;

	for (int i=0; i<1; i++)
	{
		if (i>0) strcpy(ini, "p");

		sprintf(filename,"%s%4.4d%3.3d%3.3d.%s", ini, yy, dd, ss, extension);
		strcpy(fullfilename,tmppath);
		strcat(fullfilename,filename);

		fp = fopen(fullfilename,"r");

		if (fp != NULL) {
			//printf("Reading file: %s\n\n", filename);
			fileOpened = true;
			break;
		}

		printf("Cannot open the file: %s\n", fullfilename);

	}

	if (fileOpened == false)
		return(-2);

	return(this->readHeader());
}

int CpdataRead::readNextFile(int cTime){

	int cFile;

	ss++; //set++
	cFile = 0;
	newDoy = false;		// this variable indicates a new doy setting

	if (cTime<0) cTime=0;

	while(1){
		if (this->setFilename()==1)
			break;

		if (cTime < 3){
			if (online){
				printf("Waiting %d seconds to read this file again\n", timeout);
				sleep(timeout);
			}
			cTime++;
			continue;
		}

		// Search next file set = set+1
		ss++;
		cFile++;

		//
		if(cFile==3){
			newDoy = true;
			dd += 1;
			ss=-2;
			if(dd>366) {yy++; dd=1;}
		}
		if(cFile>=4) return(0);
	}

	if(sHeader.ltime>endTime)
		return(0);

	return(1);
}
int CpdataRead::getNoise(){
	//Scale DC per channel
	for(UINT i=0; i<sHeader.nChannels; i++){
		int ncAvg = nAvg*nBeamAvg*sHeader.nIncoherentInt;
		pNoise[i] = enoise(pSelfSpect[i], sizeBlockperChan, ncAvg);
		//printf("NOISE[%d] = %f", i, pNoise[i]);
	}

	return(0);
}

void CpdataRead::setLastFile(void){
	sprintf(filename,"d2009101000.pdata");
	yy = 2010;
	dd = 01;
	ss = 00;
}


int CpdataRead::readNextBlock(){
	int cTime=0;

	//*******************************************************
	//Setting the memory to zero

	if(nBeamAvg!=1) return(0);

	if (fileOpened == false){
		printf("There are no files opened\n");
		return(0);
	}

	if (sHeader.nBlocksRead >= sHeader.nBlocksPerFile) {
		if(!this->readNextFile(cTime)){
			return(0);
		}
	}

	//*************************************************
	//Reading shortHeader, if it isn't completed you should wait a moment (timeout)
	if(firstBlock == false){

		while(!this->readShortHeader()){
			if(online){
				printf("Waiting %d seconds for reading this block again\n", timeout);
				sleep(timeout);
			}
			cTime++;
			if (cTime >= 3)	break;
		}

		if (cTime >= 3){
			printf("Continuing with the next file\n");
			//If the next file has not been found finishing the program
			if(!this->readNextFile(cTime)){
				return(0);
			}
			//Return 1 when a new file has been found but it has not been read yet
			//return(-1);
		}
		newDoy = false;
	}

	//*************************************************
	// Reading spectra
	for(UINT i=0; i<sHeader.nChannels; i++)
		fread(pSelfSpect[i],sizeof(float)*sizeBlockperChan,1,fp);


	// Reading cross-spectra
	for(UINT i=0; i<sHeader.nPairs; i++)
		fread(pCrossSpect[i],sizeof(fcomplex)*sizeBlockperChan,1,fp);


	// Read dc bias estimates
	if (readDC){
		for(UINT i=0; i<sHeader.nChannels; i++){
			fread(pDcSpect[i],sizeof(fcomplex)*sHeader.nSamples,1,fp);
			//Averaging DC
			for(UINT j=0; j<sHeader.nSamples; j++){
				pDcPerChannel[i] = Cadd(pDcPerChannel[i],pDcSpect[i][j]);
			}
		}
		//Scale DC per channel
		for(UINT i=0; i<sHeader.nChannels; i++){
			pDcPerChannel[i] = RCdiv(pDcPerChannel[i],(float)sHeader.nSamples);
		}

	}

	firstBlock = false;
	sHeader.nBlocksRead++;
	nTotalBlocksRead++;

	return(1);
}

int CpdataRead::removeDC(){

	//*************************************************
	// Reading spectra
	for(UINT i=0; i<sHeader.nChannels; i++)
		for(UINT j=0; j<sHeader.nSamples; j++)
			pSelfSpect[i][j*sHeader.nFftPoints+0] =
					(pSelfSpect[i][j*sHeader.nFftPoints+1] + pSelfSpect[i][j*sHeader.nFftPoints+sHeader.nFftPoints-1])/2;

	for(UINT i=0; i<sHeader.nPairs; i++)
		for(UINT j=0; j<sHeader.nSamples; j++)
			pCrossSpect[i][j*sHeader.nFftPoints+0] =
					RCdiv( Cadd(pCrossSpect[i][j*sHeader.nFftPoints+1], pCrossSpect[i][j*sHeader.nFftPoints+sHeader.nFftPoints-1]), 2);

	return(1);
}
