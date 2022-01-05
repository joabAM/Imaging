/*
 * CjroPlot.cpp
 *
 *  Created on: Mar 4, 2010
 *      Author: murco
 */

#include "CjroPlot.h"

CjroPlot::CjroPlot() {
	// TODO Auto-generated constructor stub
	allocatedMemAxis = false;
	init = false;
	pls = new plstream();
	N = 31;
	red = new float[N-1];
	green = new float[N-1];
	blue = new float[N-1];
	CjroPlot::setCmapJet();
}

CjroPlot::~CjroPlot() {
	// TODO Auto-generated destructor stub
	delete [] red;
	delete [] green;
	delete [] blue;
	delete [] pls;
}

void CjroPlot::initialize(int xWidth, int yWidth){
	// plplot graphics initialization
	char geometry[]=" 500x600 ";
	sprintf(geometry,"%dx%d",xWidth,yWidth);
	pls->scolbg(255,255,255); // redefine background color as white
	pls->setopt("geometry",geometry); // set X window size

	pls->ssub(1,1);
	pls->sdev("xwin");
	pls->scolbg(255,255,255);
	pls->init();
	pls->fontld(1); // load extended fonts
	pls->font(2); // set Roman font
	pls->scol0(15,0,0,0); // redefine white as black
	pls->col0(15); // black foreground
	// next, set up RTI field
	pls->schr(0,0.75); // scale font size
	pls->smaj(0,1.0); // set tic length
	init = true;
}
void CjroPlot::setAxis(float xMin, float xMax, int xDim, float yMin, float yMax, int yDim, float zMin, float zMax, bool useTime){

	PLFLT x[4],y[4];
	int nTicks=4;
	float *xTicks;
	char **labels;


	xTicks = new float[nTicks];
	labels = new char*[nTicks];
	for(int i=0;i<nTicks;i++){
		labels[i] = new char[10];
	}

	xMinimum = xMin;
	xMaximum = xMax;
	yMinimum = yMin;
	yMaximum = yMax;
	zMinimum = zMin;
	zMaximum = zMax;

	XDIM = xDim;
	YDIM = yDim;

	xResolution = (xMaximum - xMinimum)/XDIM;
	yResolution = (yMaximum - yMinimum)/YDIM;

	//printf("\n");

	//creating box
	//pls->vpor(0.08,0.95,0.15,0.90); // RTI viewport
	//pls->wind(xMinimum, xMaximum, yMinimum, yMaximum); // window
	//pls->box("bcs",xResolution,0.0,"bcinst",0.0,1); // box
	pls->env(xMinimum,xMaximum,yMinimum,yMaximum,0,1);

	//Setting xTicks
	if(useTime) CjroPlot::getTimeTicks(xTicks, labels, &nTicks);
	else CjroPlot::getXTicks(xTicks, labels, nTicks);
	//CjroPlot::setXTicks(xTicks, labels, nTicks);


	// background = black
	x[0]=x[3]=xMinimum; x[1]=x[2]=xMaximum; y[0]=y[1]=yMinimum; y[2]=y[3]=yMaximum;
	pls->psty(0);
	pls->fill(4,x,y);


	delete [] xTicks;
	del2dArrCh(labels,nTicks);
}
void CjroPlot::setTitles(char* xTitle, char* yTitle, char* Title){
	pls->schr(0,0.65);
	pls->lab(xTitle, yTitle, Title);
}

void CjroPlot::fillValue(float value, int i, int j){

}

void CjroPlot::fillArray(float *array, int nx, int ny){
	//nx = nFftPoints and ny = nsamples
	PLFLT value;
	int shift=1;

	pls->Alloc2dGrid(&z, XDIM, YDIM);

	for(int j=0;j<ny;j++){
		for(int i=0;i<nx/2;i++){
			value = array[j*nx + i];
			z[i+shift*nx/2][j] = value;
		}
		for(int i=nx/2;i<nx;i++){
			value = array[j*nx + i];
			z[i-shift*nx/2][j] = value;
			//CjroPlot::fillValue(value, i, j);
		}
	}
    //pls->image(z, XDIM, YDIM, xMinimum, xMaximum, yMinimum, yMaximum, zMinimum, zMaximum,xMinimum, xMaximum, yMinimum, yMaximum);
    pls->imagefr(z, XDIM, YDIM, xMinimum, xMaximum, yMinimum, yMaximum, 0., 0., zMinimum, zMaximum, NULL, NULL);
    pls->Free2dGrid(z,XDIM,YDIM);
    //pls->adv(0);
}

void CjroPlot::getColor(float value, int *r, int *g, int *b){
	int x;
	float delta;

	//lineal scale
	delta = (float)(zMaximum - zMinimum)/N;
	x = (value - zMinimum)/delta;

	if(x<0) x = 0;
	if(x>=N) x = N-1;

	*r = red[x]; *g = green[x]; *b = blue[x];
}

void CjroPlot::getXTicks(float *ticks, char **labels, int nTicks){

	float dt;

	dt = (xMaximum - xMinimum)/nTicks;
	for(int i=0;i<nTicks;i++){
		ticks[i] = xMinimum + dt*i;
		sprintf(labels[i],"%1.2g",ticks[i]);
	}
}

void CjroPlot::getTimeTicks(float *ticks, char **labels, int *nTicks){

	double time,dlab=0;
	float dt;
	int dsec[10]={5,10,30,60,300,600,1800,3600,7200,18000};
	int i,ih,im,is,n;

	char ctime[15];
	float csize = 0.65;

	dt = xMaximum - xMinimum;

	for(i=0;i<10;i++){
		if(dt>(float)dsec[i]/3600.0){
			dlab=dsec[MAX(0,i-1)]; // label increments in seconds
			//dtic=dsec[MAX(0,i-2)]; // tic increments in seconds
		}
	}

	pls->schr(0,csize);
	time=0.0;
	n=0;

	do{
		ih=time;
		im=60*(time-(float)ih);
		is=3600*(time-ih)-60*im;
		sprintf(ctime,"%02i:%02i:%02i",ih,im,is);

		if(dlab>300) strcpy(ctime+5,"\0");
		if(time >= xMinimum && time <= xMaximum){
			strcpy(labels[n], ctime);
			ticks[n] = time;
			n++;
		}
		time+=dlab/3599.999;

	}while(time <= xMaximum);
	*nTicks = n;
}

void CjroPlot::setXTicks(float *ticks, char **labels, int n){

	float csize = 0.65, posC, dpos, dt = xMaximum - xMinimum;

	dpos =  (float)(ticks[1] - ticks[0])/(2.0*dt);

	for(int i=0;i<n;i++){
		posC = (float)(ticks[i] - xMinimum)/dt;
		pls->schr(0,csize/2.0);
		pls->mtex("b",1.0,posC,0.5,"|");
		pls->schr(0,csize);
		pls->mtex("b",1.5,posC,0.5,labels[i]);
		pls->schr(0,csize/3.0);
		pls->mtex("b",1.0,(posC+dpos),0.5,"|");
	}
}
void CjroPlot::setXTicks(float *ticks, int n){

	float csize = 0.65, pos;
	char label[10];

	for(int i=0;i<n;i++){
		pos = (float)(ticks[i] - xMinimum)/(xMaximum - xMinimum);
		pls->schr(0,csize/2.0);
		pls->mtex("b",1.0,pos,0.5,"|");
		pls->schr(0,csize);
		sprintf(label,"%09.2f",ticks[i]);
		pls->mtex("b",1.5,pos,0.5,label);
	}
}

void CjroPlot::setCmapJet(){
	float rf=0, gf=0, bf=0;
	PLFLT pos[N], r[N], g[N], b[N];
	//************Jet map********************************
	for(int x=0;x<N;x++){
		if(x <= 35.0/100*(N-1)) rf = 0.0;
		else if (x <= 66.0/100*(N-1)) rf = (100.0/31)*x/(N-1) - 35.0/31;
		else if (x <= 89.0/100*(N-1)) rf = 1.0;
		else rf = (-100.0/22)*x/(N-1) + 111.0/22;

		if(x <= 12.0/100*(N-1)) gf = 0.0;
		else if(x <= 38.0/100*(N-1)) gf = (100.0/26)*x/(N-1) - 12.0/26;
		else if(x <= 64.0/100*(N-1)) gf = 1.0;
		else if(x <= 91.0/100*(N-1)) gf = (-100.0/27)*x/(N-1) + 91.0/27;
		else gf = 0.0;

		if(x <= 11.0/100*(N-1)) bf = (50.0/11)*x/(N-1) + 0.5;
		else if(x <= 34.0/100*(N-1)) bf = 1.0;
		else if(x <= 65.0/100*(N-1)) bf = (-100.0/31)*x/(N-1) + 65.0/31;
		else bf = 0;

		r[x] = rf;
		g[x] = gf;
		b[x] = bf;
		pos[x] = 1.0/(N-1);
	}
	r[N-1] = rf;
	g[N-1] = gf;
	b[N-1] = bf;
    for(int i=0;i<N;i++){
    	pos[i]=1.0*i/(N-1);
    	//printf("pos[%d]=%f r[%d]=%f g[%d]=%f b[%d]=%f\n",i,pos[i],i,r[i],i,g[i],i,b[i]);
    }
	pls->scmap1n(512);
	pls->scmap1l(1,N,pos,r,g,b,NULL);
}

void CjroPlot::setCmapRdBu(){
	float rf=0, gf=0, bf=0;
	//************RdBu map********************************
	for(int x=0;x<N;x++){
		if(x <= 50.0/100*(N-1)) rf = 2.0*(242.0/255)*x/(N-1) + 5.0/255;
		else if (x <= 60.0/100*(N-1)) rf = 10.0*(6.0/255)*x/(N-1) + 217.0/255;
		else rf = -100.0/40*(150.0/255)*x/(N-1) + 478.0/255;

		if(x <= 50.0/100*(N-1)) gf = 2.0*(199.0/255)*x/(N-1) + 48.0/255;
		else gf = -2.0*(247.0/255)*x/(N-1) + 2.0*247.0/255;

		if(x <= 50.0/100*(N-1)) bf = 2.0*(150.0/255)*x/(N-1) + 97.0/255;
		else bf = -2.0*(216.0/255)*x/(N-1) + 463.0/255;

		red[x] = rf*255;
		green[x] = gf*255;
		blue[x] = bf*255;
		//printf("r[%d]=%05.2f g[%d]=%05.2f b[%d]=%05.2f\n",x,red[x],x,green[x],x,blue[x]);
	}
}

void CjroPlot::setCmapRdBu0(){
	float rf=0, gf=0, bf=0;
	PLFLT pos[N], r[N], g[N], b[N];
	//************RdBu classic map********************************
	for(int x=0;x<N;x++){
		if(x <= 50.0/100*(N-1)) rf = 2.0*x/(N-1);
		else rf = 1;

		if(x <= 50.0/100*(N-1)) gf = 2.0*x/(N-1);
		else gf = -2.0*x/(N-1) + 2.0;

		if(x <= 50.0/100*(N-1)) bf = 1.0;
		else bf = -2.0*x/(N-1) + 2.0;

		r[x] = rf;
		g[x] = gf;
		b[x] = bf;
		pos[x] = 1.0/(N-1);

		//printf("r[%d]=%05.2f g[%d]=%05.2f b[%d]=%05.2f\n",x,red[x],x,green[x],x,blue[x]);
	}
	r[N-1] = rf;
	g[N-1] = gf;
	b[N-1] = bf;

    for(int i=0;i<N;i++){
    	pos[i]=1.0*i/(N-1);
    	//printf("pos[%d]=%f r[%d]=%f g[%d]=%f b[%d]=%f\n",i,pos[i],i,r[i],i,g[i],i,b[i]);
    }
	pls->scmap1n(255);
	pls->scmap1l(1,N,pos,r,g,b,NULL);

}


void CjroPlot::endPlot(){
	if (init==false) return;
	pls->spause(false);
	pls->eop();
}
