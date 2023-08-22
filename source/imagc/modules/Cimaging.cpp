/*
 * Imaging.cpp
 *
 *  Created on: Mar 15, 2010
 *      Author: murco
 */

#include "Cimaging.h"



Cimaging::Cimaging() {
	// TODO Auto-generated constructor stub
	NTHREADS = sysconf( _SC_NPROCESSORS_ONLN );
	srand((unsigned)time(0));
	key = rand() % 9999;
	key = 191;
}

Cimaging::Cimaging(int nProc, int process_key) {
	// TODO Auto-generated constructor stub
	NTHREADS = nProc;
	srand((unsigned)time(0));
	key = rand() % 9999;
	key = process_key;
}

Cimaging::~Cimaging() {
	// TODO Auto-generated destructor stub
	int rc;
	struct shmid_ds shmid_struct;

	delete [] ind1;
	delete [] ind2;
	delete [] g;
	delete [] sig2;
	delete [] data;
	delete [] pIdData;
	//del2dArrD(covar,2*nb);
	//del2dArrD(f,nx);
	del2dArrF(rimage,nx);
//	del2dArrF(image,nx*ny);
	del2dArrF(image_test,nfft*nHeis);
	//del3dArrD(hp,nx,ny);
	//del3dArrD(h,nx,ny);

	rc = shmdt(shm_addr);
	if (rc==-1)
	{
		printf("main: shmdt() failed\n");
		return;
	}

	rc = shmctl(shmid, IPC_RMID, &shmid_struct);
	if (rc==-1)
	{
		printf("main: shmctl() failed\n");
		return;
	}
}

int Cimaging::setParameters(int nX , int nY, int nChannels, UINT *CHANNELS, int nPairs, CROSS *CROSS_PAIRS, int nFft, int *indHeiRange,
		UINT __nChannelsSel, UINT* __channelsSel){

	extern imagingBuff *pImg;

	int i,k;
	UINT p,q;
	long shmsz;
	struct shmid_ds shmid_struct;

	nx = nX;
	ny = nY;
	nchan = nChannels;
	npair = nPairs;
	nfft = nFft;
	indHei[0] = indHeiRange[0];
	indHei[1] = indHeiRange[1];
	channels = CHANNELS;
	crossPairs = CROSS_PAIRS;

	setChannels(__nChannelsSel, __channelsSel);

	nb = nPairsSel + 1;
	nHeis = indHei[1] - indHei[0] + 1;

//	printf("nFFTs = %d\nnHeis = %d\nnx = %d\nny = %d\n", nFft, nHeis, nx, ny);

	//shared memory
	shmsz = nx*ny*nfft*nHeis*sizeof(float);

	printf("Shared memory with key %d and size %ld bytes\n", int(key), shmsz);

	if((shmid = shmget(key, shmsz, IPC_EXCL | 0666)) == -1){
		cerr<<("shmget: Creating new shared memory\n");
		// maybe segment needs to be created
		if((shmid = shmget(key, shmsz, IPC_CREAT | 0666)) == -1){
			cerr<<"shmget: Shared memory could not be created\n";

			if((shmid = shmget(key, shmsz, IPC_EXCL | 0666)) == -1)
				printf("Trying to erase memory ...\n");
			else
				return 0;
			//Erasing memory
			shmid = shmget(key, 0, IPC_EXCL | 0666);
			shmctl(shmid, IPC_STAT, &shmid_struct);
			shm_addr = (float *)shmat(shmid, 0, 0);

			i = shmdt(shm_addr);
//			if (i==-1)
//			{
//				printf("main: shmdt() failed\n");
//			}

			i = shmctl(shmid, IPC_RMID, &shmid_struct);
			if (i==-1)
			{
				printf("Memory could not be erased.\n\n");
				printf("Be sure you have enabled enough shared memory in your system.\n");
				printf("\tOn Linux:\n");
				printf("\t\tsudo echo \"%d\" >/proc/sys/kernel/shmmax\n", int(shmsz*1.5));
				printf("\t\tsudo echo \"%d\" >/proc/sys/kernel/shmall\n", int(shmsz*1.5));
				printf("\tOn OSX:\n");
				printf("\t\tsudo sysctl -w kern.sysv.shmall=%d\n", int(shmsz*1.5));
				printf("\t\tsudo sysctl -w kern.sysv.shmmax=%d\n", int(shmsz*1.5));
			}
			else{
				printf("Memory erased. Try executing the script again.\n");
			}

			return 0;
		}
    }

    // Now we attach the segment to our data space.//
    if((shm_addr = (float *)shmat(shmid, 0, 0)) == (float *) -1) {
        cerr<<"shmat";
        return 0;
    }

	ind1 = new int[nb];
	ind2 = new int[nb];
	g = new float[(2*nb)];
	sig2 = new double[(2*nb)];
	data = new float[2*nb];

	/*covar = allocate2dArrD(2*nb,2*nb);

	rimage = allocate2dArrF(nx,ny);
	image = allocate2dArrF(nx*ny,nfft*nHeis);
	hp = allocate3dArrD(nx,ny,2*nb);
	f = allocate2dArrD(nx,ny);
	h = allocate3dArrD(nx,ny,2*nb);*/

	/*covar = new double*[(2*nb)];
	f = new double*[nx];
	hp = new double**[nx];
	h = new double**[nx];*/

	/*for(i=0;i<(2*nb);i++)
		covar[i] = new double[2*nb];*/

//	image = new float*[(nx*ny)];
//	for(i=0;i<nx*ny;i++){
//		image[i] = new float[(nfft*nHeis)];
//	}

	/*image_test = new float*[(nfft*nHeis)];

	for(i=0;i<(nfft*nHeis);i++){
		image_test[i] = new float[(nx*ny)];
	}*/

	image_test = new float*[(nfft*nHeis)];
	image_test[0] = new float[(nfft*nHeis)*(nx*ny)];

	for(i=1;i<(nfft*nHeis);i++){
		image_test[i] = image_test[i-1] + (nx*ny);
	}

	rimage = new float*[nx];
	for(i=0;i<nx;i++){
		rimage[i] = new float[ny];
		/*f[i] = new double[ny];
		hp[i] = new double*[ny];
		h[i] = new double*[ny];*/
	}

	/*for(i=0;i<nx;i++){
		for(j=0;j<ny;j++){
			hp[i][j] = new double[(2*nb)];
			h[i][j] = new double[(2*nb)];
		}
	}*/

	//OPTIMIZING
	f = new double[nx*ny];
	h3d = new double[nx*ny*2*nb];
	hp3d = new double[nx*ny*2*nb];

	pImg = new struct imagingBuff;

	//pImg->f = new double[nx*ny];
	//pImg->h3d = new double[nx*ny*2*nb];

	ind1[0] = 0;
	ind2[0] = 0;
	k = 0;
	for(p=0;p<(nChannelsSel-1);p++){
		for(q=p+1;q<nChannelsSel;q++){
			ind1[k+1] = q;
			ind2[k+1] = p;
			k = k + 1;
		}
	}

	pIdData = new struct idData[nfft*nHeis];

	return 1;
}

void Cimaging::setChannels(UINT __nChannelsSel, UINT *__channelsSel) {
	//######################################################################333
	int i;
	UINT p,q;
	UINT __nPairsSel;
	UINT* __crossSel = new UINT[npair];

	if (__nChannelsSel != 0){

		nChannelsSel = __nChannelsSel;
		channelsSel = __channelsSel;

		__nPairsSel = 0;

		for (i=0; i<npair; i++){

			for (p=0; p<nChannelsSel; p++){

				if ( crossPairs[i][0] != channelsSel[p] )
					continue;

				for (q=p+1; q<nChannelsSel; q++){

					if (crossPairs[i][1] != channelsSel[q])
						continue;

					__crossSel[__nPairsSel] = i;
					__nPairsSel++;
				}
			}
		}
	}
	else{
		nChannelsSel = nchan;
		channelsSel = channels;

		__nPairsSel = npair;

		for (p=0; p<__nPairsSel; p++){
			__crossSel[p] = p;
		}
	}

	nPairsSel = __nPairsSel;
	crossSel = __crossSel;

	CROSS* __crossPairsSel = new CROSS[nPairsSel];

	for (p=0; p<nPairsSel; p++){
		__crossPairsSel[p][0] = crossPairs[crossSel[p]][0];
		__crossPairsSel[p][1] = crossPairs[crossSel[p]][1];
	}

	crossPairsSel = __crossPairsSel;


	printf("NUMBER OF SEL CHANNELS = %d\n", nChannelsSel);
	printf("SELECTED CHANNELS = ");
	for(p=0; p< nChannelsSel; p++){
		printf("%d, ", channelsSel[p]);
	}
	printf("\n");
	printf("\n");

	printf("NUMBER OF SEL PAIRS = %d\n", nPairsSel);
	printf("SELECTED CROSS PAIRS = ");
	for(p=0; p< nPairsSel; p++){
		printf("[%d, %d] ", crossPairsSel[p][0], crossPairsSel[p][1]);
	}
	printf("\n");
	printf("\n");
	//#########################################################################
	return;

}

void Cimaging::getSpreadFunc(float *rx, float *ry, float *rz, float scalex, float scaley, float rotangle, float offset, float wl){

	int i,j,k=0;

	float *dx = new float [nb];
	float *dy = new float [nb];
	float *dz = new float [nb];

	float dcosx[nx][ny],dcosy[nx][ny];
	float dcosxp[nx][ny],dcosyp[nx][ny];
	double pi,psi,eta,phase;
	float wavel;

	//OPTIMIZING
	double *pM;

	for(i=0;i<nb;i++){
		dx[i] = rx[ channelsSel[ind2[i]] ]-rx[ channelsSel[ind1[i]] ];
		dy[i] = ry[ channelsSel[ind2[i]] ]-ry[ channelsSel[ind1[i]] ];
		dz[i] = rz[ channelsSel[ind2[i]] ]-rz[ channelsSel[ind1[i]] ];
	}

	pi=4.0*atan(1.0);
	wavel = wl;
	for(i=0;i<nx;i++){
		for(j=0;j<ny;j++){
			dcosx[i][j] = scalex*(float)(i-nx/2)/(float)nx;
			dcosy[i][j] = scaley*(float)(j-ny/2)/(float)ny+offset;
		}
	}

	// Rotating direction cosines and offset
	for(i=0;i<nx;i++){
		for(j=0;j<ny;j++){
			dcosxp[i][j] = dcosx[i][j]*cos(rotangle)-dcosy[i][j]*sin(rotangle);
			dcosyp[i][j] = dcosx[i][j]*sin(rotangle)+dcosy[i][j]*cos(rotangle);
		}
	}

	//Calculating point spread function
	pM = hp3d;
	for(i=0;i<nx;i++){
		for(j=0;j<ny;j++){
			psi=dcosxp[i][j];
			eta=dcosyp[i][j];
			for(k=0;k<nb;k++){
				phase=(2.0*pi/wavel)*(psi*dx[k]+eta*dy[k]+sqrt(1.0-psi*psi-eta*eta)*dz[k]);
				//hp[i][j][2*k]=cos(phase);
				//hp[i][j][2*k+1]=sin(phase);
				*(pM++) = cos(phase);
				*(pM++) = sin(phase);
			}
		}
	}

	delete [] dx;
	delete [] dy;
	delete [] dz;
}

void Cimaging::getSpreadFunc2(float *rx, float *ry, float *rz, float scalex, float scaley, float rotangle, float offsetx, float offsety, float wl){

	int i,j,k=0;

	float *dx = new float [nb];
	float *dy = new float [nb];
	float *dz = new float [nb];

	float dcosx[nx][ny],dcosy[nx][ny];
	float dcosxp[nx][ny],dcosyp[nx][ny];
	double pi,psi,eta,phase;
	float wavel;

	//OPTIMIZING
	double *pM;

	for(i=0;i<nb;i++){
		dx[i] = rx[ channelsSel[ind2[i]] ]-rx[ channelsSel[ind1[i]] ];
		dy[i] = ry[ channelsSel[ind2[i]] ]-ry[ channelsSel[ind1[i]] ];
		dz[i] = rz[ channelsSel[ind2[i]] ]-rz[ channelsSel[ind1[i]] ];
	}

	pi=4.0*atan(1.0);
	wavel = wl;
	for(i=0;i<nx;i++){
		for(j=0;j<ny;j++){
			dcosx[i][j] = scalex*(float)(i-nx/2)/(float)nx+offsetx;
			dcosy[i][j] = scaley*(float)(j-ny/2)/(float)ny+offsety;
		}
	}

	// Rotating direction cosines and offset
	for(i=0;i<nx;i++){
		for(j=0;j<ny;j++){
			dcosxp[i][j] = dcosx[i][j]*cos(rotangle)-dcosy[i][j]*sin(rotangle);
			dcosyp[i][j] = dcosx[i][j]*sin(rotangle)+dcosy[i][j]*cos(rotangle);
		}
	}

	//Calculating point spread function
	pM = hp3d;
	for(i=0;i<nx;i++){
		for(j=0;j<ny;j++){
			psi=dcosxp[i][j];
			eta=dcosyp[i][j];
			for(k=0;k<nb;k++){
				phase=(2.0*pi/wavel)*(psi*dx[k]+eta*dy[k]+sqrt(1.0-psi*psi-eta*eta)*dz[k]);
				//hp[i][j][2*k]=cos(phase);
				//hp[i][j][2*k+1]=sin(phase);
				*(pM++) = cos(phase);
				*(pM++) = sin(phase);
			}
		}
	}

	delete [] dx;
	delete [] dy;
	delete [] dz;
}

float** Cimaging::getImaging(float **pSelfSpect, fcomplex **pCrossSpect, float *noise, float snrThr, int navg, int* xDim, int* yDim, bool show){
	/*
	 *
	 * 		**pSelfSpect
	 * 		**pCrossSpect
	 * 		*noise
	 * 		snrThr
	 * 		navg
	 *
	 * 		*xDim
	 * 		*yDim
	 *
	 */
	
	float avgNoise, avgSignal, s0, s1;
	float anorm;
	fcomplex ccf;
	float snr, isnr, ftemp;
	int i,j,k;
	int m,n;
	UINT p;
	int ix, iy;
	//print message
	char msg_progress[10]="-\0/\0|\0\\\0";
	int progress_count=0, count=0;
	//int minfft, maxfft;
	int minRange, maxRange, nRanges;
	//imaging processing
	int xSize,ySize;
	int ndata;
	//Threads
	int o, idThr;
	pid_t pidList[NTHREADS], pid;

	int xi=45, yi=22, xscreen=0, yscreen=0;
	int info;

	//allow memory for pIdData
	xSize = nx*ny;
	ySize = nfft*nHeis;

	//Averaged noise
	avgNoise = 0;
	for(p=0;p<nChannelsSel;p++)
		avgNoise += noise[ channelsSel[p] ];

	avgNoise /= (float)nChannelsSel;

	//Signal with snr higher than snrThr
	ndata = 0;
	for(i=0;i<nfft;i++)
	{
		for(j=indHei[0];j<=indHei[1];j++)
		{
			avgSignal = 0;
			iy = j*nfft+i;
			for(p=0;p<nChannelsSel;p++)
				avgSignal += pSelfSpect[ channelsSel[p] ][iy] - noise[ channelsSel[p] ];

			avgSignal /= (float)nChannelsSel;

			if(avgSignal < 0.0)
				avgSignal = 1.0e-10;

			//Estimating S/N
			snr = avgSignal/avgNoise;

			if(snr > snrThr){
				pIdData[ndata].i = i;
				pIdData[ndata].j = j;
				pIdData[ndata].snr = snr;
				ndata++;
			}
		}
	}
	gotoxy(1,yi);
	if (show) printf("Processing ...\n");

	//Clear shared memory
	memset(shm_addr,NAN_INT,(xSize)*(ySize)*sizeof(float)); //setting -1734829824 (float) = -50 (int)

	idThr = 0;
	pid = 191;

//	for(i=0;i<NTHREADS-1;i++){
//		pidList[i] = fork();
//		if(pidList[i]){
//			pid = pidList[i];
//			break;
//		}
//		idThr++;
//	}
	xscreen = int(idThr/12)*xi+25+2;
	yscreen = yi + 1 + idThr % 12;

	gotoxy(xscreen-20, yscreen);
	if (show) printf("PID No");

	for(i=0;i<NTHREADS-1;i++){
		pid = fork();
		if(pid == 0){
			//I am a children process
			//pid = pidList[i];
			break;
		}
		pidList[i] = pid;
		idThr++;
	}
	progress_count=0;
	count = 0;
	xscreen = int(idThr/12)*xi+25+2;
	yscreen = yi + 2 + idThr % 12;

	minRange = (idThr*ndata)/NTHREADS;
	maxRange =  ((idThr+1)*ndata)/NTHREADS;
	nRanges = maxRange-minRange;

	gotoxy(xscreen-20, yscreen);
	if (show) printf("[%02d]: (%4d,%4d)\n", getpid(), minRange, maxRange-1);


	for(int id=minRange;id<maxRange;id++)
	{
		i = pIdData[id].i;
		j = pIdData[id].j;
		snr = pIdData[id].snr;
		isnr = snr*((float) (xSize));

		//clearing data and image
		memset(data,0,2*nb*sizeof(float));

		for(k=0;k<nx;k++)
			memset(rimage[k],0,ny*sizeof(float));

		data[0]=1.0;
		data[1]=0.0;

		//Computing normalized ccf
		iy = j*nfft+i;


		for(p=0;p<nPairsSel;p++)
		{
			s0 = pSelfSpect[crossPairsSel[p][0]][iy] - noise[crossPairsSel[p][0]];
			s1 = pSelfSpect[crossPairsSel[p][1]][iy] - noise[crossPairsSel[p][1]];

			anorm = sqrt(s0*s1);

			if((s0>0) && (s1>0))
				ccf = RCdiv(pCrossSpect[ crossSel[p] ][iy],anorm);
			else
				ccf = Complex(0.0,0.0);

			if(Cabs(ccf) > 1.0) ccf = RCdiv(ccf,Cabs(ccf));

			data[2*(p+1)] = ccf.r;
			data[2*(p+1)+1] = ccf.i;
		}

		info = this->imgInvert(rimage, data, snr, navg);

		iy = (j-indHei[0])+i*nHeis;


		for(n=0;n<ny;n++)
		{
			for(m=0;m<nx;m++)
			{
				//printf("rimage[%d][%d] = %10.4g\n",m,n,rimage[m][n]);
				ftemp = MAX(rimage[m][n]*isnr,0.0);

				ix = (m+n*nx)*(ySize);

				//Setting to NaN value when ftemp is less than ZERO
				if(ftemp > 0.0)
					shm_addr[ix+iy] = (float)10.0*log10((double) ftemp);
				else
					shm_addr[ix+iy] = NAN_FLOAT;
			}
		}

		gotoxy(xscreen, yscreen);
		if (show) printf("[%s] %2d%c {Info=%d}\n", msg_progress+progress_count, (count+1)*100/nRanges, 0x25, info);

		count += 1;
		progress_count+=2;

		if (progress_count>6) progress_count=0;
	}

	//waiting for the child process exited
//	gotoxy(xscreen, yscreen);
//	printf("waiting pid=%4d\n", pid);
//	waitpid(pid,&o,0);
//	gotoxy(xscreen, yscreen);
//	printf("exiting           \n");
//
//	if(pidList[0]==0) _exit(0);
//
//	gotoxy(xscreen, yscreen);
//	printf("exited            \n");

	if (pid == 0)
	{
		gotoxy(xscreen, yscreen);
		if (show) printf("exiting                    \n");
		_exit(0);
	}

	for(i=0;i<NTHREADS-1;i++){
		gotoxy(xscreen, yscreen);
		if (show) printf("waiting for pid=%4d\n", pidList[i]);
		waitpid(pidList[i],&o,0);
	}
	gotoxy(xscreen, yscreen);
	if (show) printf("exited                        \n");

	*xDim = xSize;
	*yDim = ySize;

//    for(i=0;i<xSize;i++)
//    	for(j=0;j<ySize;j++)
//    		image[i][j] = shm_addr[i*ySize+j];

    for(i=0;i<xSize;i++)
    	for(j=0;j<ySize;j++)
    		image_test[j][i] = shm_addr[i*ySize+j];

	if (!show) gotoxy(0, 0);
	return(image_test);
}

int Cimaging::imgInvert(float **image, float *data, float snr, int navg){


	// Using Fortran Library
	int info=0,n=2*nb,lda=2*nb,lwork;
	float cov[lda*n];
	float *w, *work, w1[2*nb];
    float wkopt;
	int i, i1, j1, use_diagonal=0;


	// calculate covariance
	this->errors_(data, cov, snr, navg);

	// Fortran library
	w = new float[n];

	lwork = -1;
	ssyev_("V","L",&n,cov,&lda,w,&wkopt,&lwork,&info);
	lwork = (int)wkopt;
    lwork = 20*nb;
	work = new float[lwork];
	ssyev_("V","L",&n,cov,&lda,w,work,&lwork,&info);

	for(i=0;i<n;i++)
		w[i] = MAX(w[i],0.0001);

// To Work with diagonal covariance
    if(use_diagonal == 1){
        for(i=0;i<2*nb;i++)
            w1[i] = MAX(cov[i+i*2*nb],0.0005);
        for(i=0;i<nb;i++)
            w1[2*i] = w1[2*i+1]=(w1[2*i]+w1[2*i+1])/2.0;//*sqrt(2.0);//;///0.5; ///2.0;

        for(i1=0;i1<2*nb;i1++)
            for(j1=0;j1<2*nb;j1++){
                if(i1 == j1){
                    w[i1] = w1[i1];
                //        if(talk==1) printf("i1 %d %d %f\n",i1,j1,w[i1]);
                    cov[i1+j1*2*nb] = 1.0;
                }
                else cov[i1+j1*2*nb] = 0.0;
            }
    }


	// invert data
	this->mem2d_(image, data, cov, w, &info);
	//i = apArray2DF2File("/home/murco/temp","new.txt",image,nx,ny);

	delete [] w;
	delete [] work;

	return info;
}

void Cimaging::mem2d_(float **image, float *data, float *cov, float *sigma2, int *info){
	/*
	 * 	main imaging routine
	 * 	nb is number of baselines, which includes the zero lag by convention.
	 * 	Data begin with single estimate of zero lag, which should be 1.0 + i 0.0..
	 * 	Baseline lengths, wavelength in meters.
	 * 	*/

	extern imagingBuff *pImg;

	//OPTIMIZING
	double *pM;
	double cte1,cte2;

	int incx=1, incy=1, nb2=2*nb,sz=nx*ny;
	float alphaf = 1, betaf = 0;
	double alpha = 1, beta = 0;
	double cova[nb2*nb2];

	double amu,csum;
	double lambda[2*nb];//,lambda0[2*nb];
	double fvec[2*nb],fjac[2*nb*2*nb];
	double diag[2*nb],r[2000],qtf[2*nb],w1[2*nb],w2[2*nb],w3[2*nb],w4[2*nb];
	double factor,xtol;
	int nfev,njev;
	int i,j,k,n;
	int maxfev,nprint,mode;

	// point spread function is setup somewhere else
	// transform data

	sgemv_("T", &nb2, &nb2, &alphaf, cov, &nb2, data, &incx, &betaf, g, &incy);

	//**********PASS TO DOUBLE ****
	for(i=0;i<2*nb*2*nb;i++)
		cova[i]=cov[i];

	// copy variances (diagonalized)
	for(i=0;i<2*nb;i++)
		sig2[i]=sigma2[i];

	dgemm_("T","N", &nb2, &sz, &nb2, &alpha, cova, &nb2, hp3d, &nb2, &beta, h3d, &nb2, 1, 1);

	//**** H3D TRANSPOSE **************//
	//dgemm_("T","N", &sz, &nb2, &nb2, &alpha, hp3d, &nb2, cova, &nb2, &beta, h3d, &sz, 1, 1);

	// initial guess: based on covariance diagonal to avoid matrix inversion

	gam=g[0];
	amu=1.0e-4;
	cte1 = gam/(float)(nb2);
	cte2 = 2.0*amu;

	for(i=0;i<nb2;i++){
		csum=0.0;
		pM = h3d+i;
		for(j=0;j<nx;j++)
			for(k=0;k<ny;k++){
				csum += *pM; // note f ~ gam/m
				pM+=nb2;
			}
		csum *= cte1;
		lambda[i]=(csum-g[i])*cte2/sig2[i];
	}

	lambda[0]=lambda[1]=1.0e-4;

	// parameters for Powell's method
	n=nb2;
	maxfev=2000;
	nprint=-1;
	mode=1;
	xtol=1.0e-4;
	factor=1.0e2;

	pImg->f = f;
	pImg->g = g;
	pImg->sig2 = sig2;
	pImg->gam = gam;
	pImg->nx = nx;
	pImg->ny = ny;
	pImg->nb2 = nb2;
	pImg->h3d = h3d;

	*info = hybrj(funcvj_opt,0,n,lambda,fvec,fjac,n,xtol,maxfev,diag,mode,factor,nprint,&nfev,&njev,r,maxfev,qtf,w1,w2,w3,w4);

	pM = f;
	for(i=0;i<nx;i++)
		for(j=0;j<ny;j++)
			image[i][j] = *(pM++);

	//mode = apArray2DF2File("/home/murco/temp","new.txt",image,nx,ny);
}


int funcvj_opt(void *p, const int n, const double* x, double *fvec, double *fjac, const int ldfjac, const int iflag){

	extern imagingBuff *pImg;

	int nx, ny, nb2;
	double *f,*sig2,gam;
	float *g;
	double *h3d;
	double *vec, *pTemp, *pTemp1, *pTemp2, *pTemp3;
	double cte1, cte2;

	double omega,amu,avg1,avg2;
	int i,j,k,l,sz;

	double sumg;
	double alpha = 1, beta = 0;
	int incx =1, incy = 1;

	//Using local variable is faster than using global variable
	//Passing global variable to local variable
	nx = pImg->nx;
	ny = pImg->ny;
	nb2 = pImg->nb2;
	f = pImg->f;
	g = pImg->g;
	sig2 = pImg->sig2;
	gam = pImg->gam;
	h3d = pImg->h3d;

	omega=(float)nb2/2.0;
	amu=0.0;

	for(i=0;i<nb2;i++)
		amu+=x[i]*x[i]*sig2[i];

	amu = sqrt(amu/(4.0*omega));

	//**************** Begin ex-function calf ***************************

	//********* F[i][j]s = Sum(k){ H[i][j][k]s * X[k]s } ****************//
	//***************** F[i]v = H[i]m * Xv  ****************************//
	// ***** Fortran libraries are faster than "for sentences":
	// ***** dgemv_("T", &columns, &rows, 1, Hm, &columns, Xv, 1, 0, Fv, 1)
	// **** Hm : matrix H
	// **** Fv : vector F
	// **** Ps : scalar P

	sz=nx*ny;
	dgemv_("T", &nb2, &sz, &alpha, h3d, &nb2, x, &incx, &beta, f, &incy);

	pTemp = f;
	for(i=0;i<nx;i++)
		for(j=0;j<ny;j++){
			if((*pTemp > -88.0) && (*pTemp < 88.0)){
				*pTemp = exp(-*pTemp);
				pTemp++;
				continue;
			}
			else if(*pTemp > 88.0){
				*pTemp = 0.0;
				pTemp++;
				continue;
			}
			else if(*pTemp < -88.0){
//				*pTemp = 1.6e38;
//				*pTemp = 1.7e308;
				*pTemp = 1.6e304;
				pTemp++;
				continue;
			}
		}

	//****** sum = Sum(i,j) {F[i][j]} *************//
	sumg = dasum_(&sz, f, &incx);

	//****** F[i][j] = alpha*F[i][j] *************//
//	alpha = gam/MAX(sumg,1.4e-45);
//	alpha = gam/MAX(sumg,5e-304);
	alpha = gam/MAX(sumg,1.6e-300);
	dscal_(&sz, &alpha, f, &incx);

	//******************* End ex-function calf  *******************

	if(iflag==1){

		//********* Fvec[k]s = Sum(i,j){ H[i][j][k]s * F[i][j]s } ****************//
		//***************** Fvec v = HmT * Fv  ****************************//
		// **** HmT      : matrix transpose H
		// **** Fvec v   : vector Fvec
		// **** Fvec[k]s : scalar Fvec[k]

		alpha = 1.0;
		cte2 = (2.0*amu);
		dgemv_("N", &nb2, &sz, &alpha, h3d, &nb2, f, &incx, &beta, fvec, &incy);

		for(i=0;i<nb2;i++)
			fvec[i] += (-g[i]-x[i]*sig2[i]/cte2);

	}
	else if(iflag==2){

		alpha = 1.0;
		cte1 = (8.0*pow(amu,3)*omega);
		cte2 = (2.0*amu);
		pTemp = fjac;

		//********* Fvec[k]s = Sum(i,j){ H[i][j][k]s * F[i][j]s } ****************//
		//***************** Fvec v  = HmT * Fv  ****************************//
		// **** HmT      : matrix transpose H
		// **** Fvec v   : vector Fvec
		// **** Fv       : vector F

		vec = new double[nb2];
		dgemv_("N", &nb2, &sz, &alpha, h3d, &nb2, f, &incx, &beta, vec, &incy);


		for(i=0;i<nb2;i++){
			for(j=0;j<nb2;j++){
				//Working with pointers is faster than working with arrays
				//Creating temporary pointers
				pTemp1 = h3d+i;
				pTemp2 = h3d+j;
				pTemp3 = f;

				avg1 = 0.0;

				for(k=0;k<nx;k++)
					for(l=0;l<ny;l++){
						//Setting indices as sum, and not as product "F[i*n+j]"
						avg1 -= *(pTemp3++)**(pTemp2)**(pTemp1);
						pTemp1+=nb2;
						pTemp2+=nb2;
					}

				avg2 = vec[i]*vec[j]/gam;

				*pTemp = avg1+avg2;
				*pTemp += x[i]*x[j]*sig2[i]*sig2[j]/cte1;

				if(i==j) *pTemp -= sig2[i]/cte2;

				pTemp++;
			}
		}

		delete [] vec;

	}

	return 0;

}


int funcvj(void *p, const int n, const double* x, double *fvec, double *fjac, const int ldfjac, const int iflag){
	extern imagingBuff *pImg;

	/*extern double ***h0,**f0,*g0;
	extern double *sig20,gam0;
	extern int nx0,ny0,nb2;*/

	double omega,amu,sum,avg1,avg2,avg3;
	int i,j,k,l;

	omega=(float)pImg->nb2/2.0;
	amu=0.0;

	for(i=0;i<pImg->nb2;i++)
		amu+=x[i]*x[i]*pImg->sig2[i];

	amu = sqrt(amu/(4.0*omega));

	for(i=0;i<pImg->nx;i++){
		for(j=0;j<pImg->ny;j++){
			sum=0.0;
			for(k=0;k<pImg->nb2;k++){
				sum += x[k]*pImg->h0[i][j][k];
			}
			if(sum>88.0)
				pImg->f0[i][j]=0.0;
			else if(sum<-88.0)
				pImg->f0[i][j]=1.6e38;
			else
				pImg->f0[i][j]=exp(-sum)*1.0;
		}
	}

	sum=0.0;
	for(i=0;i<pImg->nx;i++)
		for(j=0;j<pImg->ny;j++)
			sum += pImg->f0[i][j];

	for(i=0;i<pImg->nx;i++)
		for(j=0;j<pImg->ny;j++)
			pImg->f0[i][j] *= pImg->gam/MAX(sum,1.4e-45);

	if(iflag==1){
		for(i=0;i<pImg->nb2;i++){
			fvec[i]=0.0;
			for(j=0;j<pImg->nx;j++)
				for(k=0;k<pImg->ny;k++)
					fvec[i]+=pImg->f0[j][k]*pImg->h0[j][k][i];

			fvec[i] += (-pImg->g[i]-x[i]*pImg->sig2[i]/(2.0*amu));
		}
	}

	else if(iflag==2){
		sum=0.0;
		for(i=0;i<pImg->nb2;i++){
			for(j=0;j<pImg->nb2;j++){

				avg1=avg2=avg3=sum=0.0;

				for(k=0;k<pImg->nx;k++)
					for(l=0;l<pImg->ny;l++)
						sum += pImg->h0[k][l][j]*pImg->f0[k][l];
				sum /= pImg->gam;


				for(k=0;k<pImg->nx;k++)
					for(l=0;l<pImg->ny;l++){
						avg1-=pImg->f0[k][l]*pImg->h0[k][l][j]*pImg->h0[k][l][i];
						avg2+=pImg->f0[k][l]*pImg->h0[k][l][i]*sum;
					}

				fjac[i*pImg->nb2+j] = avg1+avg2;
				fjac[i*pImg->nb2+j] += x[i]*x[j]*pImg->sig2[i]*pImg->sig2[j]/(8.0*pow(amu,3)*omega);

				if(i==j) fjac[i*pImg->nb2+j]-=pImg->sig2[i]/(2.0*amu);
			}
		}
	}

	return 0;

}

void Cimaging::errors_(float *data, float *cov, float snr, int navg){

	fcomplex r12,r13,r14,r23,r24,r34;
	fcomplex a1,a2,a3,a4,a5,a6,a7,a8;
	fcomplex rho[nb][nb],d2,d2p;

	float fac,c1,c2,c3,c4;
	float nom=-1.0,cte;
	int i,j;
	int l1,l2,l3,l4;

	fac=1.0+1.0/(snr);
	cte = 1.0/(float)(navg);

	// fill up correlation matrix ordered by antenna counting number
	//printf("\nRHO\n");
	for(i=0;i<nb;i++){
		rho[ind1[i]][ind2[i]] = Complex(data[2*i],data[2*i+1]);
		rho[ind2[i]][ind1[i]] = Conjg(rho[ind1[i]][ind2[i]]);
	}


	// handle nonzero lags
	for(i=1;i<nb;i++){
		for(j=i;j<nb;j++){
			l1=ind1[i]; l2=ind2[i]; l3=ind1[j]; l4=ind2[j];

			r12=rho[l1][l2]; if(l1==l2) r12=Complex(fac,0.0);
			r13=rho[l1][l3]; if(l1==l3) r13=Complex(fac,0.0);
			r14=rho[l1][l4]; if(l1==l4) r14=Complex(fac,0.0);
			r23=rho[l2][l3]; if(l2==l3) r23=Complex(fac,0.0);
			r24=rho[l2][l4]; if(l2==l4) r24=Complex(fac,0.0);
			r34=rho[l3][l4]; if(l3==l4) r34=Complex(fac,0.0);

			a1=Cmul(r13,Conjg(r23));
			a2=Cmul(r14,Conjg(r24));
			a3=Cmul(r13,Conjg(r14));
			a4=Cmul(r23,Conjg(r24));

			a5=Cmul(Conjg(r34),Cadd(a1,a2));
			a6=Cmul(r12,Cadd(a3,a4));
			a7=Cmul(r12,Conjg(r34));
			a8=RCmul(pow(Cabs(r13),2)+pow(Cabs(r14),2)+pow(Cabs(r23),2)+pow(Cabs(r24),2),a7);

			d2=Cmul(r13,Conjg(r24));
			d2=Csub(d2,RCmul(0.5,a5));
			d2=Csub(d2,RCmul(0.5,a6));
			d2=Cadd(d2,RCmul(0.25,a8));
			d2=RCmul(cte,d2);

			a5=Cmul(r34,Cadd(a1,a2));
			a6=Cmul(r12,Conjg(Cadd(a3,a4)));
			a7=Cmul(r12,r34);
			a8=RCmul(pow(Cabs(r13),2)+pow(Cabs(r14),2)+pow(Cabs(r23),2)+pow(Cabs(r24),2),a7);

			d2p=Cmul(r14,Conjg(r23));
			d2p=Csub(d2p,RCmul(0.5,a5));
			d2p=Csub(d2p,RCmul(0.5,a6));
			d2p=Cadd(d2p,RCmul(0.25,a8));
			d2p=RCmul(cte,d2p);

			c1=0.5*(Cadd(d2,d2p).r);
			c2=0.5*(Csub(d2,d2p).r);
			c3=0.5*(Cadd(d2,d2p).i);
			c4=0.5*(Csub(d2p,d2).i);

			cov[2*i+2*nb*(2*j)]=c1;
			cov[2*i+1+2*nb*(2*j+1)]=c2;
			cov[2*i+1+2*nb*(2*j)]=c3;
			cov[2*i+2*nb*(2*j+1)]=c4;

			if(j>i){
				cov[2*j+2*nb*(2*i)]=c1;
				cov[2*j+1+2*nb*(2*i+1)]=c2;
				cov[2*j+1+2*nb*(2*i)]=c4;
				cov[2*j+2*nb*(2*i+1)]=c3;
			}
		}
	}
	// do components with zero lags
	for(i=0;i<2*nb;i+=2){
		cov[i]=cov[i+1+2*nb]=0.0;
		cov[i+1]=cov[i+2*nb]=0.0;
		cov[2*nb*i]=cov[1+2*nb*(i+1)]=0.0;
		cov[2*nb*(i+1)]=cov[1+2*nb*i]=0.0;
	}

	cov[0]=cov[2*nb+1]=nom;

}
