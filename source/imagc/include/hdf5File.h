/*
 * hdf5File.h
 *
 *  Created on: Nov 9, 2010
 *      Author: roj-idl71
 */

#ifndef HDF5FILE_H_
#define HDF5FILE_H_

#define H5Acreate_vers            2
#define H5Adelete_vers            2
#define H5Aiterate_vers           2
#define H5Arename_vers            2
#define H5Dcreate_vers            2
#define H5Dopen_vers              2
#define H5Eclear_vers             2
#define H5Eprint_vers             2
#define H5Epush_vers              2
#define H5Eset_auto_vers          2
#define H5Eget_auto_vers          2
#define H5Ewalk_vers              2
#define H5Gcreate_vers            2
#define H5Gopen_vers              2
#define H5Pget_filter_vers        2
#define H5Pget_filter_by_id_vers  2
#define H5Pinsert_vers            2
#define H5Pregister_vers          2
#define H5Rget_obj_type_vers      2
#define H5Tarray_create_vers      2
#define H5Tcommit_vers            2
#define H5Tget_array_dims_vers    2
#define H5Topen_vers              2

#define H5_NO_DEPRECATED_SYMBOLS

#include <H5Cpp.h>

//#include "hdf5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef VERSION
#define VERSION "2.1.0"
#endif

using namespace std;

extern "C" {
	float sasum_(const int *n, float *X, const int *incx);
}

int svHeader2HDF5(char* path, int year, int doy, int hour, int min, int sec,
		float h0, float dh, int nsamples, float ipp, int nx, int ny,
		float scalex, float scaley, float rotangle, float offset,
		float wavlength, int heiIndex[2], int nProfiles);

int svArray2HDF5(char* path, char* file, float** array, int xDim, int yDim); //, int nx, int ny, int nfft, int nHeis);


int svHeader2HDF5v2(char* path, int year, int doy, int hour, int min, int sec,
		float h0, float dh, int nsamples, float ipp,
		unsigned int ncoh, unsigned int nincoh,
		int nx, int ny,
		float scalex, float scaley, float rotangle, float offset,
		float wavlength,
		int heiIndex[2], int beaconIndex[2],
		int nFFTPoints,
		unsigned int nchannels, float *pHydraPhase, float *pDeltaPhase);

int svArray2HDF5v2(char* path, char* file, float** array, int nFFTPoints, int nHeis, int nx, int ny);

class CHeaderImag {
	private:

	public:
		int calibration;

		char* path;
		int year, doy, hour, min, sec;

		float wavelength;
		float ipp;
		float h0, dh;
		int nsamples, nprofiles, nchannels;

		int hindex[2];
		int bindex[2];

		int nx, ny;
		float scalex, scaley, rotangle, yoffset;

		float *phydraphase, *pdeltaphase;

		CHeaderImag();
		~CHeaderImag();
		int write2file();

};

#endif /* HDF5FILE_H_ */

