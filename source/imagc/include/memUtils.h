/*
 * cmemory.h
 *
 *  Created on: Apr 7, 2010
 *      Author: murco
 */

#ifndef CMEMORY_H_
#define CMEMORY_H_

#include "complex.h"

char** allocate2dArrCh(const int nx, const int ny);
int** allocate2dArrI(const int nx, const int ny);
float** allocate2dArrF(const int nx, const int ny);
double** allocate2dArrD(const int nx, const int ny);
fcomplex** allocate2dArrC(const int nx, const int ny);
double*** allocate3dArrD(const int nx, const int ny, const int nz);

void del2dArrI(int **ppArr, int nx);
void del2dArrF(float **ppArr, int nx);
void del2dArrC(fcomplex **ppArr, int nx);
void del2dArrCh(char **ppArr, int nx);
void del2dArrD(double **ppArr, int nx);
void del3dArrD(double ***ppArr, int nx, int ny);
#endif /* CMEMORY_H_ */
