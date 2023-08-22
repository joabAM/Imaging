/*
 * imagingFile.h
 *
 *  Created on: Feb 15, 2010
 *      Author: murco
 */

#ifndef JROFILE_H_
#define JROFILE_H_

#include <stdio.h>
#include <stdlib.h>
#include <cctype>
#include <unistd.h> // getcwd() definition
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <string.h>
#include <algorithm>
#include "complex.h"

using namespace std;

typedef unsigned int UINT;
typedef UINT CROSS [2];

#ifndef MAX_NUMBER_OF_CHANNELS
#define MAX_NUMBER_OF_CHANNELS	16
#endif

struct configParameters{
  char sExpName[255];
  UINT  nChannels;
  UINT *channels;
  UINT nPairs;
  CROSS *crossPairs;
  char sPath[255];
  int nx;
  int ny;
  float scalex;
  float scaley;
  float rotangle;
  float offset;
  float wavelength;
  float* pDeltaPhase;
  float* pHydraPhase;
  float* pRx;
  float* pRy;
  float* pRz;
  float fProcRange[2];
  float fBeaconRange[2];
};
int writeConfigFile(struct configParameters*, char* path, char* file);
struct configParameters* readConfigFile(char* path, char* file);
int wrHeaderFile(char* path, int year, int doy, int hour, int min, int sec, float h0, float dh, int nsamples, float ipp, int nx, int ny, float scalex, float scaley, float rotangle, float offset, float wavlength, int heiIndex[2], int nProfiles);
int wrArray2File(char* path, char* file, float** array, int xDim, int yDim);
int apArray1DI2File(char* path, char* file, int* array, int xDim);
int apArray1DF2File(char* path, char* file, float* array, int xDim);
int apArray2DF2File(char* path, char* file, float** array, int xDim, int yDim);
int apArray1DD2File(char* path, char* file, double* array, int xDim);
int apArray2DD2File(char* path, char* file, double** array, int xDim, int yDim);
int apArray1DC2File(char* path, char* file, fcomplex* array, int xDim);
int apArray2DC2File(char* path, char* file, fcomplex** array, int xDim, int yDim);
void FillUIntArray(char* list, unsigned int* InBuffer, unsigned int n);
void FillIntArray(char* list, int* InBuffer, unsigned int n);
void FillArrays(char* list, float* InBuffer, unsigned long n);
void GetCurrentPath(char* buffer);
int getdir (string dir, string match, vector<string> &files);
int getFilesFromDir (string dir, vector<string> &files, string extension, unsigned int lenfilename);
int searchLastFile(char *path, char *pattern, char *filename);
int findFiles(char *path, char *extension, bool retLastFile, char *filename, unsigned int lenfilename);
int findFilesIntoRange(const char *path, const char *pattern, const int year, const int doy, const int range, const char *ext, char *filename);

#endif /* JROFILE_H_ */

