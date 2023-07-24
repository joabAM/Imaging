/*
 * readConfigFile.c
 *
 *  Created on: Feb 15, 2010
 *      Author: murco
 */

#include "fileUtils.h"

struct configParameters* readConfigFile(char* path, char* file){
	struct configParameters *cparam;
	FILE *fp;
	char tempStr[1024];//Readed Line Text
	char *valstr;//Readed Value
	unsigned int i;
	float beaconPhase[20];
	char filename[1024];
	char os_sep[] = "/";

	if (file[0] == os_sep[0])
	{	//file is an absolute path
		strcpy(filename,file);
	}
	else{
		//file is not an absolute path so add path
		strcpy(filename, path);

		if (path[strlen(path) - 1] != os_sep[0]){
			strcat(filename, "/");
		}

		strcat(filename,file);
	}

	printf("CONFIG_FILE = %s\n", filename);

	fp=fopen(filename,"r");

	if(fp == NULL){
		printf("Configuration file '%s' was not found\n",filename);
		return(NULL);
	}

	//printf("CONFIG FILE  : %s\n",filename);
	cparam = (struct configParameters*)malloc(sizeof(struct configParameters));

	while(!feof(fp)){

		//Read one line by one line
		fgets(tempStr,sizeof(tempStr),fp);

		if (tempStr[0]=='#' || tempStr[0]=='/'){
			continue;
		}

		for(i = 0; i<strlen(tempStr); i++)
			tempStr[i] = toupper(tempStr[i]);

		//printf("%s",tempStr);

		if (strstr(tempStr, "EXPERIMENT NAME=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			sprintf(cparam->sExpName,"%s",valstr);
			continue;
		}

		if (strstr(tempStr, "NCHAN=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			cparam->nChannels=atoi(valstr);
			//allocating memory for arrays
			cparam->pDeltaPhase = new float[cparam->nChannels];
			cparam->pHydraPhase = new float[cparam->nChannels];
			cparam->pRx = new float[cparam->nChannels];
			cparam->pRy = new float[cparam->nChannels];
			cparam->pRz = new float[cparam->nChannels];
			continue;
		}

		if (strstr(tempStr, "NPOINTS X=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			cparam->nx=atoi(valstr);
			continue;
		}

		if (strstr(tempStr, "NPOINTS Y=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			cparam->ny=atoi(valstr);
			continue;
		}

		if (strstr(tempStr, "RADAR WAVELENGTH=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			cparam->wavelength=(float)atof(valstr);
			continue;
		}

		if (strstr(tempStr, "MAX X=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			cparam->scalex=(float)atof(valstr)*2;
			continue;
		}

		if (strstr(tempStr, "MAX Y=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			cparam->scaley=(float)atof(valstr)*2;
			continue;
		}

		if (strstr(tempStr, "ANGLE OF ROTATION=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			cparam->rotangle=(float)atof(valstr)*(3.1415926535897931)/180;
			continue;
		}

		if (strstr(tempStr, "DCOSY OFFSET=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			cparam->offset=(float)atof(valstr);
			continue;
		}

		if (strstr(tempStr, "DPATH=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			sprintf(cparam->sPath,"%s",valstr);
			continue;
		}

		if (strstr(tempStr, "RX=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			FillArrays(valstr,cparam->pRx,cparam->nChannels);
			continue;
		}

		if (strstr(tempStr, "RY=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			FillArrays(valstr,cparam->pRy,cparam->nChannels);
			continue;
		}

		if (strstr(tempStr, "RZ=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			FillArrays(valstr,cparam->pRz,cparam->nChannels);
			continue;
		}

		if (strstr(tempStr, "PHASE BEACON=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			FillArrays(valstr,beaconPhase,cparam->nChannels);
			continue;
		}

		if (strstr(tempStr, "PHASE HYDRA=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			FillArrays(valstr,cparam->pHydraPhase,cparam->nChannels);
			continue;
		}

		if (strstr(tempStr, "PROC RANGE=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			FillArrays(valstr,cparam->fProcRange,2);
			continue;
		}

		if (strstr(tempStr, "BEACON RANGE=") != NULL){
			valstr = strchr(tempStr,'=');valstr++;
			FillArrays(valstr,cparam->fBeaconRange,2);
			continue;
		}

		if (strstr(tempStr, "END") != NULL){
			break;
		}
	}

	fclose(fp);

	for(i=0;i<cparam->nChannels;i++){
		cparam->pDeltaPhase[i] = cparam->pHydraPhase[i] - beaconPhase[i];
	}

	return(cparam);
}

//Fill the arrays on the corresponding variable
void FillUIntArray(char* list, unsigned int* InBuffer, unsigned int n){
	unsigned int i=0;

	char *pch;
	pch = strtok(list,",");
	while((pch != NULL) || (i>n)){
		*(InBuffer++) = (unsigned int)atoi(pch);
		pch = strtok (NULL, ",");
		i++;
	}
	return;
}

//Fill the arrays on the corresponding variable
void FillIntArray(char* list, int* InBuffer, unsigned int n){
	unsigned int i=0;

	char *pch;
	pch = strtok(list,",");
	while((pch != NULL) || (i>n)){
		*(InBuffer++) = (int)atoi(pch);
		pch = strtok (NULL, ",");
		i++;
	}
	return;
}

//Fill the arrays on the corresponding variable
void FillArrays(char* list, float* InBuffer, unsigned long n){
	unsigned long i=0;

	char *pch;
	pch = strtok(list,",");
	while((pch != NULL) || (i>n)){
		*(InBuffer++) = (float)atof(pch);
		pch = strtok (NULL, ",");
		i++;
	}
	return;
}

int wrArray2File(char* path, char* file, float** array, int xDim, int yDim){
	FILE *fp;
	char filename[1024];

	strcpy(filename,path);
	strcat(filename,"/");
	strcat(filename,file);
	strcat(filename,".bin");

	//printf("Writing file: %s\n",filename);

	fp = fopen(filename,"w");

	if(!fp)
	{
		printf("%s: This file cannot open\n", filename);
		exit(1);
	}

	//Eliminar las filas con suma menor a NaN*yDim, NaN es <-90 para nuestro caso

	for(int m=0;m<xDim;m++)
	{
		fwrite(array[m],sizeof(float),yDim,fp);
	}
	fclose(fp);
	return 0;
}

int apArray1DI2File(char* path, char* file, int* array, int xDim){
	FILE *fp;
	char filename[1024];

	strcpy(filename,path);
	strcat(filename,"/");
	strcat(filename,file);

	//printf("Writing file: %s\n",filename);

	fp = fopen(filename,"a");

	if(!fp)
	{
		printf("%s: This file cannot open\n", filename);
		exit(1);
	}

	fprintf(fp,"\nNEW INT 1D ARRAY\n");
	for(int m=0;m<xDim;m++)
		fprintf(fp,"ARRAY[%4d] = %+4d\n",m,array[m]);

	fclose(fp);

	return 0;
}

int apArray1DF2File(char* path, char* file, float* array, int xDim){
	FILE *fp;
	char filename[1024];

	strcpy(filename,path);
	strcat(filename,"/");
	strcat(filename,file);

	//printf("Writing file: %s\n",filename);

	fp = fopen(filename,"a");

	if(!fp)
	{
		printf("%s: This file cannot open\n", filename);
		exit(1);
	}

	fprintf(fp,"\nNEW 1D ARRAY\n");
	for(int m=0;m<xDim;m++)
		fprintf(fp,"ARRAY[%4d] = %+10.8g\n",m,array[m]);

	fclose(fp);

	return 0;
}

int apArray2DF2File(char* path, char* file, float** array, int xDim, int yDim){
	FILE *fp;
	char filename[1024];

	strcpy(filename,path);
	strcat(filename,"/");
	strcat(filename,file);

	//printf("Writing file: %s\n",filename);

	fp = fopen(filename,"a");

	if(!fp)
	{
		printf("%s: This file cannot open\n", filename);
		exit(1);
	}

	fprintf(fp,"\nNEW 2D ARRAY\n");
	for(int m=0;m<xDim;m++)
		for(int n=0;n<yDim;n++)
			fprintf(fp,"ARRAY[%4d][%4d] = %+10.8g\n",m,n,array[m][n]);


	fclose(fp);
	return 0;
}

int apArray1DD2File(char* path, char* file, double* array, int xDim){
	FILE *fp;
	char filename[1024];

	strcpy(filename,path);
	strcat(filename,"/");
	strcat(filename,file);

	//printf("Writing file: %s\n",filename);

	fp = fopen(filename,"a");

	if(!fp)
	{
		printf("%s: This file cannot open\n", filename);
		exit(1);
	}

	fprintf(fp,"\nNEW 1D ARRAY\n");
	for(int m=0;m<xDim;m++)
		fprintf(fp,"ARRAY[%4d] = %+10.8g\n",m,array[m]);

	fclose(fp);

	return 0;
}

int apArray2DD2File(char* path, char* file, double** array, int xDim, int yDim){
	FILE *fp;
	char filename[1024];

	strcpy(filename,path);
	strcat(filename,"/");
	strcat(filename,file);

	//printf("Writing file: %s\n",filename);

	fp = fopen(filename,"a");

	if(!fp)
	{
		printf("%s: This file cannot open\n", filename);
		exit(1);
	}

	fprintf(fp,"\nNEW 2D ARRAY\n");
	for(int m=0;m<xDim;m++)
		for(int n=0;n<yDim;n++)
			fprintf(fp,"ARRAY[%4d][%4d] = %+10.8g\n",m,n,array[m][n]);


	fclose(fp);
	return 0;
}

int apArray1DC2File(char* path, char* file, fcomplex* array, int xDim){
	FILE *fp;
	char filename[1024];

	strcpy(filename,path);
	strcat(filename,"/");
	strcat(filename,file);

	//printf("Writing file: %s\n",filename);

	fp = fopen(filename,"a");

	if(!fp)
	{
		printf("%s: This file cannot open\n", filename);
		exit(1);
	}

	fprintf(fp,"\nNEW 1D ARRAY\n");
	for(int m=0;m<xDim;m++)
		fprintf(fp,"ARRAY[%4d] = (%+10.8g,%+10.8g)\n",m,array[m].r,array[m].i);

	fclose(fp);

	return 0;
}

int apArray2DC2File(char* path, char* file, fcomplex** array, int xDim, int yDim){
	FILE *fp;
	char filename[1024];

	strcpy(filename,path);
	strcat(filename,"/");
	strcat(filename,file);

	//printf("Writing file: %s\n",filename);

	fp = fopen(filename,"a");

	if(!fp)
	{
		printf("%s: This file cannot open\n", filename);
		exit(1);
	}

	fprintf(fp,"\nNEW 2D ARRAY\n");
	for(int m=0;m<xDim;m++)
		for(int n=0;n<yDim;n++)
			fprintf(fp,"ARRAY[%4d][%4d] = (%+10.8g,%+10.8g)\n",m,n,array[m][n].r,array[m][n].i);


	fclose(fp);
	return 0;
}

int wrHeaderFile(char* path, int year, int doy, int hour, int min, int sec, float h0, float dh, int nsamples, float ipp, int nx, int ny, float scalex, float scaley, float rotangle, float offset, float wavelength, int heiIndex[2], int nProfiles){
	//HEADER IMAGING FILE VARIABLES
	FILE *pFilename;
	char filename[100];
	float buffHeader[20];

	//WRITING HEADER IMAGING FILE
	//buffHeader[0] = 36000.0*hour + 600.0*min + 10.0*sec;
	buffHeader[1] = 1000.0*(year) + doy;
	buffHeader[2] = (float)h0;
	buffHeader[3] = (float)dh;
	buffHeader[4] = (float)nsamples;
	buffHeader[5] = (float)ipp;
	buffHeader[6] = (float)nx;
	buffHeader[7] = (float)ny;
	buffHeader[8] = (float)scalex;
	buffHeader[9] = (float)scaley;
	buffHeader[10] = (float)rotangle;
	buffHeader[11] = (float)offset;
	buffHeader[12] = (float)wavelength;
	buffHeader[13] =(float)heiIndex[0];
	buffHeader[14] =(float)heiIndex[1];
	buffHeader[15]= (float)nProfiles;

	sprintf(filename,"%s/i%04d%03d.hdr",path,year,doy);

	pFilename = fopen(filename,"w");
	if(!pFilename)
	{
		printf("Image header file not opened: %s\n", filename);
		exit(1);
	}

	fwrite(buffHeader,sizeof(float),20,pFilename);
	fclose(pFilename);
	return 0;
}

void GetCurrentPath(char* buffer){
	//getcwd(buffer, 200);
	return;
}

/*function... might want it in some class?*/
int getdir (string dir, string match, vector<string> &files){
	DIR *dp;
	string file;
	struct dirent *dirp;
	struct stat _stat;

	if((dp = opendir(dir.c_str())) == NULL) {
		printf("Error opening this directory %s\n",dir.c_str());
		return 0;
	}

	while ((dirp = readdir(dp)) != NULL) {
		string candidate( dir + "/" + dirp->d_name );
		lstat(candidate.c_str(), &_stat);
		if(S_ISREG(_stat.st_mode)){
			file = string(dirp->d_name);
			int p = file.find(match.c_str(),0);
			if(p>0)
				files.push_back(file);
		}
	}
	closedir(dp);
	sort(files.begin(),files.end());
	return 0;
}

int getFilesFromDir (string dir, vector<string> &files, string extension, unsigned int lenfilename){
	DIR *dp;
	string file;
	struct dirent *dirp;
	struct stat _stat;

	if((dp = opendir(dir.c_str())) == NULL) {
		printf("Error opening this directory %s\n",dir.c_str());
		return 0;
	}

	while ((dirp = readdir(dp)) != NULL) {
		string candidate( dir + "/" + dirp->d_name );
		lstat(candidate.c_str(), &_stat);
		
		if (not(S_ISREG(_stat.st_mode)))
			continue;

		file = string(dirp->d_name);
		
		if (not(file.substr(file.find_last_of(".") + 1) == extension))
			continue;
		if (not(file.size()==lenfilename))
			continue;

		files.push_back(file);

	}
	closedir(dp);
	sort(files.begin(),files.end());
	return 0;
}


int searchLastFile(char *path, char *pattern, char *filename)
{
	string dir, match;
	dir = path;
	match = pattern;
	//dir = string("./");
	vector<string> files = vector<string>();

	getdir(dir,match,files);

	/*for (unsigned int i = 0;i < files.size();i++) {
		printf("%s\n",files[i].c_str());
	}*/
	if (files.size()>0)
		sprintf(filename,"%s",files[files.size()-1].c_str());
	else{
		sprintf(filename,"none");
		return(0);
	}

	return(1);
}

int findFiles(char *path, char *extension, bool retLastFile, char *filename, unsigned int lenfilename)
{
	string dir, ext;
	char tmppath[100];

	strcpy(tmppath, path);

	dir = tmppath;
	ext = extension;
	vector<string> files = vector<string>();

	//getdir(dir,match,files);
	getFilesFromDir(dir, files, ext, lenfilename);

	if (files.size()>0){
		if(retLastFile) //Online process (last file)
			sprintf(filename,"%s",files[files.size()-1].c_str());
		else //Offline process (setting file)
			sprintf(filename,"%s",files[0].c_str());
	}
	else{
		sprintf(filename,"none");
		return(0);
	}

	return(1);
}

int findFilesIntoRange(const char *path, const char *pattern, const int year, const int doy, const int range, const char *ext, char *filename)
{
	int i,idoy;
	char name[30];
	struct stat _stat;
	vector<string> files = vector<string>();

	for(i=0;i<range;i++){

		idoy = doy-i;
		sprintf(name,"%s%04d%03d%s",pattern,year,idoy,ext);
		string file = string(path) + "/" + string(name);
		if(lstat(file.c_str(), &_stat)==0) files.push_back(string(name));
		idoy = doy+i;
		sprintf(name,"%s%04d%03d%s",pattern,year,idoy,ext);
		file = string(path) + "/" + string(name);
		if(lstat(file.c_str(), &_stat)==0) files.push_back(string(name));
	}

	if (files.size()>0){
		sprintf(filename,"%s",files[0].c_str());
	}
	else{
		sprintf(filename,"none");
		return(0);
	}

	return(1);
}
