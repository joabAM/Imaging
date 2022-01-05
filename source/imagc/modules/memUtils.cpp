/*
 * cmemory.cpp
 *
 *  Created on: Apr 7, 2010
 *      Author: murco
 */
#include "memUtils.h"

//////////////////////// ALLOCATE MEMORY ///////////////////////
char** allocate2dArrCh(const int nx, const int ny)
{
	char **ppArr;

	ppArr = new char*[nx];
	for(int indx = 0; indx < nx; indx++ )
		ppArr[indx] = new char[ny];

	return ppArr;
}

int** allocate2dArrI(const int nx, const int ny)
{
	int **ppArr;

	ppArr = new int*[nx];
	for(int indx = 0; indx < nx; indx++ )
		ppArr[indx] = new int[ny];

	return ppArr;
}

float** allocate2dArrF(const int nx, const int ny)
{
	float **ppArr;

	ppArr = new float*[nx];
	for(int indx = 0; indx < nx; indx++ )
		ppArr[indx] = new float[ny];

	return ppArr;
}

double** allocate2dArrD(const int nx, const int ny)
{
	double **ppArr;

	ppArr = new double*[nx];
	for(int indx = 0; indx < nx; indx++ )
		ppArr[indx] = new double[ny];

	return ppArr;
}

fcomplex** allocate2dArrC(const int nx, const int ny)
{
	fcomplex **ppArr;

	ppArr = new fcomplex*[nx];
	for(int indx = 0; indx < nx; indx++ )
		ppArr[indx] = new fcomplex[ny];

	return ppArr;
}

double*** allocate3dArrD(const int nx, const int ny, const int nz)
{
	double ***ppArr;
	int indx,indy;

	ppArr = new double**[nx];
	for(indx = 0; indx < nx; indx++){
		ppArr[indx] = new double*[ny];
		for(indy = 0; indy < ny; indy++)
			ppArr[indx][indy] = new double[nz];
	}

	return ppArr;
}

/////////////////////   DELETE MEMORY //////////////////////////////////
void del2dArrI(int **ppArr, int nx)
{
	for(int indx = 0; indx < nx; indx++ )
		delete [] ppArr[indx];

	delete [] ppArr;
}

void del2dArrF(float **ppArr, int nx)
{
	for(int indx = 0; indx < nx; indx++ )
		delete [] ppArr[indx];

	delete [] ppArr;
}

void del2dArrC(fcomplex **ppArr, int nx)
{
	for(int indx = 0; indx < nx; indx++ )
		delete [] ppArr[indx];

	delete [] ppArr;

}

void del2dArrCh(char **ppArr, int nx)
{
	for(int indx = 0; indx < nx; indx++ )
		delete [] ppArr[indx];

	delete [] ppArr;

}

void del2dArrD(double **ppArr, int nx)
{
	for(int indx = 0; indx < nx; indx++ )
		delete [] ppArr[indx];

	delete [] ppArr;
}

void del3dArrD(double ***ppArr, int nx, int ny)
{
	for(int indx = 0; indx < nx; indx++ )
	{
		for(int indy = 0; indy < ny; indy++)
			delete [] ppArr[indx][indy];

		delete [] ppArr[indx];
	}
	delete [] ppArr;
}
