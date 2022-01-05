#ifndef COMPLEXFILE_H_
#define COMPLEXFILE_H_

typedef struct FCOMPLEX {float r,i;} fcomplex;

#include <cmath>

extern float Cmod2(fcomplex z);
extern fcomplex Cadd(fcomplex a, fcomplex b);
extern fcomplex Csub(fcomplex a, fcomplex b);
extern fcomplex Cmul(fcomplex a, fcomplex b);
extern fcomplex Complex(float re, float im);
extern fcomplex Conjg(fcomplex z);
extern fcomplex Cdiv(fcomplex a, fcomplex b);
extern float Cabs(fcomplex z);
extern fcomplex Csqrt(fcomplex z);
extern fcomplex RCmul(float x, fcomplex a);
extern fcomplex RCdiv(fcomplex a, float x);

#endif /* COMPLEXFILE_H_ */
