/*
 * cnoise.h
 *
 *  Created on: Feb 24, 2010
 *      Author: murco
 */

#ifndef CNOISE_H_
#define CNOISE_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SWAP(a,b) temp=(a);(a)=(b);(b)=temp;

float enoise(float *data, int npts, int navg);
void sort(unsigned long n, double *arr);

#endif /* CNOISE_H_ */
