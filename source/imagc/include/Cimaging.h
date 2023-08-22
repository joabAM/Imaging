/*
 * Imaging.h
 *
 *  Created on: Mar 15, 2010
 *      Author: murco
 */

#ifndef CIMAGING_H_
#define CIMAGING_H_
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <string.h>
#include "memUtils.h"
#include "cmdUtils.h"
#include "fileUtils.h"
#include "complex.h"
#include "cminpack.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

//#define NTHREADS 1

#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define NAN_FLOAT -99
#define NAN_INT 0
#define ZERO_INT  0

typedef unsigned int UINT;
typedef UINT CROSS [2];

//You will need to declare *pImg variable inside your main function
//struct imagingBuff *pImg;

int funcvj(void *p, const int n, const double* x, double *fvec, double *fjac, const int ldfjac, const int iflag);
int funcvj_opt(void *p, const int n, const double* x, double *fvec, double *fjac, const int ldfjac, const int iflag);

extern "C" {
	void sgemv_(const char *trans, const int *m, const int *n, const float *alpha, float *A, const int *lda, const float *X, const int *incx, const float *beta, float *Y, const int *incy);
	void dscal_(const int *n, const double *alpha, double *X, const int *incx);
	void ssyev_(const char* jobz, const char* uplo, const int* n, float* a, const int* lda, float* w, float* work, int* lwork, int* info);
	void dgemm_(const char *transA,const char *transB, const int *m, const int *n, const int *k, const double *alpha, double *A, const int *lda, double *B, const int *ldb, const double *beta, double *C, const int *ldc,	const int transa_len, const int transb_len);
	void dgemv_(const char *trans, const int *m, const int *n, const double *alpha, double *A, const int *lda, const double *X, const int *incx, const double *beta, double *Y, const int *incy);
	void dger_(const int *m, const int *n, const double *alpha, double *X, const int *incx, double *Y, const int *incy, double *A, const int *lda );
	double dasum_(const int *n, double *X, const int *incx);
	double ddot_(const int *n, double *X, const int *incx, double *Y, const int *incy);
}

struct imagingBuff{
	double *sig2,gam;
	int nx,ny,nb2;
	//Optimizing
	double ***h0, **f0;
	double *f,*h3d;
	float *g;
};

struct idData{
	//DATE HEADER
	float snr;
    int i;
    int j;
};

class Cimaging {
	private:
		int nb, nchan, npair, indHei[2];
		UINT *channels;
		CROSS *crossPairs;
		int *ind1, *ind2;
		double *f,*h3d,*hp3d;
		double *sig2,gam;
		float *g;
		float **image_test;
		float **rimage, *data;

		void errors_(float*, float*, float, int);
		void mem2d_(float**, float*, float*, float*, int*);
		int imgInvert(float**, float*, float, int);
		//processing
		struct idData *pIdData;
		//shared memory
		key_t key;
		int shmid;
		float *shm_addr;
		int NTHREADS;

		void setChannels(UINT, UINT *);

	public:
		int nx, ny, nfft, nHeis;

		UINT nChannelsSel, *channelsSel;
		UINT nPairsSel, *crossSel;
		CROSS *crossPairsSel;
		//float **image_test;
		Cimaging();
		Cimaging(int, int);
		~Cimaging();
		int setParameters(int, int, int, UINT*, int, CROSS*, int, int*,
				UINT, UINT*);
		void getSpreadFunc(float*, float*, float*, float, float, float, float, float);
		void getSpreadFunc2(float*, float*, float*, float, float, float, float, float, float);
		float** getImaging(float **, fcomplex **, float *, float, int, int*, int*, bool);

};

#endif /* CIMAGING_H_ */
