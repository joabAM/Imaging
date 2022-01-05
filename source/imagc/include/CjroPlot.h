/*
 * CjroPlot.h
 *
 *  Created on: Mar 4, 2010
 *      Author: murco
 */

#ifndef CJROPLOT_H_
#define CJROPLOT_H_
#include <string.h>
#include <cmath>
#include <plplot/plplot.h>
#include <plplot/plstream.h>
#include "memUtils.h"

#ifndef MAX
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#endif

class CjroPlot {
private:
	plstream *pls;
	PLFLT **z;
	PLINT XDIM,YDIM;
	int N;
	float *red, *green, *blue;
	bool allocatedMemAxis, init;

	void getColor(float, int*, int*, int*);
	void getXTicks(float*, char **, int);
	void getTimeTicks(float*, char **, int*);
	void setXTicks(float*, char**, int);
	void setXTicks(float*, int);
public:
	CjroPlot();
	~CjroPlot();
	void initialize(int, int);
	void setAxis(float,float,int,float,float,int,float,float,bool);
	void setTitles(char*,char*,char*);
	void fillValue(float,int,int);
	void fillArray(float *, int , int);
	void setCmapJet();
	void setCmapRdBu();
	void setCmapRdBu0();
	void endPlot();
	float xMinimum, xMaximum, yMinimum, yMaximum, zMinimum, zMaximum, xResolution, yResolution;
};


#endif /* CJROPLOT_H_ */
