/*
 * calibration.cpp
 *
 *  Created on: Apr 22, 2015
 *      Author: murco
 */
#define N_POINTS_Y 16
#define N_MAX_STEPS 150




#include "calibration.h"

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;

#include <iostream>

float dispMatrix[N_MAX_STEPS][N_POINTS_Y];
float powerMatrix[N_MAX_STEPS][N_POINTS_Y];

float dbmToMw( float dbm )
{
  float mW;
	if (dbm == -99.0)
		return 0;
  mW = dbm/10;
  mW = pow( 10.0, mW );
  return mW;
}

float maxValue(float array[][N_POINTS_Y], int n, int m){
	double maxValue = 0;
	for(int i=0; i<n; i++){
		for (int j=0; j<m; j++){
			if (array[i][j] > maxValue)
				maxValue = array[i][j];
		}
	}
	return maxValue;
}

float minValue(float array[][N_POINTS_Y], int n, int m){
	float minValue =1e308;
	for(int i=0; i<n; i++){
		for (int j=0; j<m; j++){
			if (array[i][j] < minValue)
				minValue = array[i][j];
		}
	}
	return minValue;
}

void normMatrixInt(float arrayIn[][N_POINTS_Y], uint8_t arrayOut[][N_POINTS_Y], int n, int m, int maxVal){
	float max = maxValue(arrayIn,n,m);
	float min = minValue(arrayIn,n,m);
	float arrayAux[n][m];
	//float max =100;
	for(int i=0; i<n; i++){
		for (int j=0; j<m; j++){
			arrayAux[i][j] = (arrayIn[i][j]-min)/(max-min);
		}
	}
	for(int i=0; i<n; i++){
		for (int j=0; j<m; j++){
			arrayOut[i][j] = int(arrayAux[i][j]*maxVal);
		}
	}
}

void normMatrix(float arrayIn[][N_POINTS_Y], float arrayOut[][N_POINTS_Y], int n, int m){
	float max = maxValue(arrayIn,n,m);
	float min = minValue(arrayIn,n,m);

	for(int i=0; i<n; i++){
		for (int j=0; j<m; j++){
			arrayOut[i][j] = (arrayIn[i][j]-min)/(max-min);
		}
	}

}

int getMaxIndex(float *vector,int len_vector){
	int index=0;
	float max_value=-1e308;
//	printf("Len=%d, PowerList = ", len_vector);
	for (int i=0; i<len_vector; i++){
//		printf("%10.3f, ", vector[i]);

		if (vector[i] > max_value){
			max_value = vector[i];
			index = i;
		}
	}
//	printf("\nMaxValue = %10.3f\nIndex = %d\n", max_value, index);

	return index;
}

int getMinIndex(float *vector,int len_vector){
	int index=0;
	float min_value=1e308;

//	printf("Len=%d, PowerList = ", len_vector);

	for (int i=0; i<len_vector; i++){

//		printf("%10.3f, ", vector[i]);

		if (vector[i] < min_value){
			min_value = vector[i];
			index = i;
		}
	}
//	printf("\nMaxValue = %10.3f\nIndex = %d\n", max_value, index);

	return index;
}

Ccalibration::Ccalibration() {

	this->initialized = false;
	this->cal_auto = 0;
	this->enabled = 0;
	this->phase_step = 0.05;
	this->channel = 0;
	this->power_len = 0;

}

void Ccalibration::init(int __cal_auto, UINT* __chan_sequence, UINT __max_nchannels,
						int __enable, float __phase_step, int __channel,
						UINT __nchannels, UINT* __channels, int __nphases,
						UINT __nx, UINT __ny,
						UINT __nFFTs, UINT __nHeis,float Xmax, float Ymax) {
							// TODO Auto-generated constructor stub

	this->nChannels = 1;
	this->nx = __nx;
	this->ny = __ny;
	this->nffts = __nFFTs;
	this->nheis = __nHeis;
	this->max_x = Xmax;
	this->max_y = Ymax;
	//****************************************************************
	//

	//*******************************************************************
	if (__nphases < 1)
		this->nphases = int(2*PI/__phase_step+0.5);
	else
		this->nphases = __nphases;

	if (__enable > 0){
		this->enabled = 1;
		this->phase_step = __phase_step;
		this->channel = __channel;
		this->nChannels = __nchannels;
		this->channels = __channels;
	}

	if (__cal_auto > 0){
		this->cal_auto = 1;
		this->enabled = 1;
		this->phase_step = __phase_step;
		this->channel = 0;
		this->nChannels = 1;

		this->chan_sequence = __chan_sequence;
		this->max_nchannels = __max_nchannels;

		this->channels = new UINT[this->max_nchannels];

	}

	if ((__enable = 0) && (__cal_auto == 0))
		return;

	int len_vector = (int)(2*3.15/this->phase_step);

	this->powerList = new float[len_vector+1];
	this->phaseList = new float[len_vector+1];
	this->desvList = new float[len_vector+1];
	this->errorList = new bool[len_vector+1];

	this->estimated_phase_list = new float[__max_nchannels];
	this->estimated_phase_list[0] = 0;

	this->power_len = 0;
	this->initialized = true;

	if (__cal_auto > 0){
		this->nextSequence();
	}

}

void Ccalibration::nextSequence() {
	// TODO Auto-generated constructor stub

	if (!this->initialized)
		return;

	this->nChannels += 1;

	if (this->nChannels > this->max_nchannels){
		this->power_len = 0;
		return;
	}

	this->channel = this->chan_sequence[nChannels-1];

	for(UINT i=0; i<this->nChannels; i++)
		this->channels[i] = this->chan_sequence[i];

	sort(this->channels, this->channels + this->nChannels);

	this->power_len = 0;

	printf("\nSelected channels: ");

	for (UINT i=0; i<this->nChannels; i++){
		printf("%02d, ", this->channels[i]);
	}

	printf("\nCalibrating channel = %02d\n", this->channel);


}

int Ccalibration::isTheLastChannel() {
	// TODO Auto-generated constructor stub
	int answer = 0;

	if (!this->initialized)
		return 1;

	if (this->nChannels > this->max_nchannels)
		answer = 1;
	return answer;

}

float Ccalibration::__getPower(float **image, int nx, int ny) {
	/*
		nx = alturas
		ny = pointsX * pointsY
	*/
	// TODO Auto-generated constructor stub
	float sum=0;
	float minValue = -20;
	if (!this->initialized)
		return minValue;
	for(int i=0; i<nx; i++){
		for (int j=0; j<ny; j++){
			if (image[i][j] == -99.0){
				sum += minValue;
				continue;
			}
//			tmp = pow(10,image[i][j]);
//			if (tmp < minValue)  tmp = minValue;
			sum += image[i][j];
		}
	}
	return sum/(nx*ny);
}



// float Ccalibration::getDispIndx(float **image, int n, int m, int pos){
// 	float sum=0;
// 	float minValue = 0;

// 	if (!this->initialized)
// 		return minValue;
// 	int offset = pos*m;
// 	int rows = m + offset;
// 	for(int i=0; i<n; i++){ //alturas
// 		for (int j=offset; j<rows; j++){  //putnos de x
// 			if (j < rows/2)
// 				sum += (j-offset)*dbmToMw(image[i][j]);
// 			else
// 				sum += (rows-j)*dbmToMw(image[i][j]);
// 			}
// 	}
// 	return sum;
// }

float Ccalibration::__getDesv(float **image, int nx1, int ny1) {
	// TODO Auto-generated constructor stub
//	int npeaks=0;
	float desv=0;
	float intensity[nx][ny], intensity_k;


//	if (!this->initialized)
//		return minValue;

	//nx = nFFT * nHeis
	//ny = nx_cuts * ny_cuts


	for(int k=0;k<nheis;k++){
		intensity_k = 0;
		for(int i=0;i<nx;i++){
			for(int j=0;j<ny;j++){
				intensity[i][j] = 0;
				for(int l=0;l<nffts;l++){
					intensity[i][j] += image[k*nffts+l][j*nx+i];
				}
				intensity[i][j] /= nffts;
				intensity_k += intensity[i][j];
			}
		}

		intensity_k = intensity_k/(nx*ny);

		// Desviacion, se considera un dcosx=0.2 y dcosy=0.1

		for(int j=ny/4;j<3*ny/4-1;j++){
			for(int i=nx/4;i<3*nx/4-1;i++){
				if(intensity[i][j] > intensity_k){
					desv+=( (float(i)-float(nx-1)/2)*(float(i)-float(nx-1)/2) +
							(float(j)-float(ny-1)/2)*(float(j)-float(ny-1)/2));
					}

			}
		}

	}

	return desv;

}

// float Ccalibration::getSharpness(float **src, int nFFTPoints, int nHeis, int nx, int ny){
// 	// Declare the variables we are going to use

// 	int kernel_size = 3;
// 	int scale = 1;
// 	int delta = 0;
// 	int ddepth = -1;

// 	// printf(" Done 001\n");
// 	Mat image(nHeis, nx, CV_64FC3);
// 	Mat imageo(nHeis, nx, CV_64FC3);

// 	// double minVal; 
// 	// double maxVal; 
// 	// Point minLoc; 
// 	// Point maxLoc;
// 	//std::minMaxLoc( image, &minVal, &maxVal, &minLoc, &maxLoc );
// 	// Mat img = imread("/home/japaza/workspace/Imaging/doc/Lenna.png",
//     //                    IMREAD_GRAYSCALE);
// 	// cv::imshow("Display window", img);
// 	// int k = waitKey(0); // Wait for a keystroke in the window

// 	Scalar mean,stddev;

// 	float sy=0, st=0;
// 	//printf(" Done 0\n");
// 	for( int h=0; h<nHeis; h++ ){

// 		for(int x=0; x<nx; x++){
// 			sy = 0;
// 			for(int y=0; y<ny; y++ ){
// 				st = 0;
// 				for(int t=0; t<nFFTPoints; t++){
// 					//printf( " i:%d,j:%d ", h*nFFTPoints+t, x + y*nx);
// 					st += src[h+nFFTPoints*t][x+ nx*y];
// 				}
// 				sy += st;
				
// 			}
// 			//printf( " x:%d,h:%d ", x, h);
// 			image.at<double>(h,x, 0) = sy; 
// 			// image.at<double>(h,x, 1) = sy; 
// 			// image.at<double>(h,x, 2) = sy; 
			
// 		}
// 	}
// 	// cv::Mat img3;
// 	// image.convertTo(img3, CV_32F, 1.0 / 255, 0);
	
// 	cv::imshow("Display window", image);
// 	int k = waitKey(0); // Wait for a keystroke in the window

// 	cv::Laplacian( image, imageo, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
// 	// cv::imshow("Display window", imageo);
// 	// int k = waitKey(0); // Wait for a keystroke in the window
// 	//printf(" Done 2\n");
// 	cv::meanStdDev(imageo, mean,  stddev);
// 	//return 0.0;
// 	return stddev.val[0];

// }

float Ccalibration::getOptFunction(float** array, int nFFTPoints, int nHeis, int nx, int ny){

	// float sh, sx, sy, st;
	// float q,p;

	// UINT maxHei = nHeis*nFFTPoints; 
	// UINT NXY = nx*ny;

	// sh = 0;
	// for( int h=0; h<nHeis; h++ ){
	// 	sx=0;
	// 	for(int x=0; x<nx; x++){
	// 		sy = 0;
	// 		for(int y=0; y<ny; y++ ){
	// 			st = 0;
	// 			for(int t=0; t<nFFTPoints; t++){
	// 				st += dbmToMw(array[h+nFFTPoints*t][x+ nx*y]);
	// 			}
	// 			// if (y < (ny/2)){
	// 			// 	p = y / (ny/2);      //distance to center factor
	// 			// }
	// 			// else{
	// 			// 	p = 1 - (y -(ny/2))/(ny/2); 
	// 			// }
	// 			// sy += (st* std::pow(p, 2));
	// 			// sy += p*st;
	// 			sy += st;
	// 		}
			
	// 		if (x < (nx/2)){
	// 			q = x / (nx/2);      //distance to center factor
	// 		}
	// 		else{
	// 			q = 1 - (x -(nx/2))/(nx/2); 
	// 		}
	// 		// sx += (sy* std::pow(q, 2));
	// 		sx += (sy*q);
			
	// 	}
	// 	sh += sx;

	// }
	
	// sh /=(nFFTPoints*nHeis*nx*ny);
	// return sh;
	return this->__getPower(array,nFFTPoints*nHeis, nx*ny)+10;
}

void Ccalibration::meanFilter(float src[],float dst[], int windowsize, int N) {



    for (int i = 0; i < N; i++) {
		float acum = 0;
        if (i < (windowsize/2)){
			for (int k=0; k< (windowsize/2)+i; k++)
				acum += src[k];
			acum /= (windowsize/2 + i );
			
		} 
		else if(i < (N - (windowsize/2))){
			for (int k=-(windowsize/2); k<(windowsize/2); k++)
				acum += src[i+k];
			acum /= (windowsize-1);
		
		}
		else{
			for (int k= i - (windowsize/2); k < N; k++)
				acum += src[k];
			acum /= ( N - i + windowsize/2);
			
		}
		dst[i] = acum; 
        
    }

}
//nx = alturas
//ny = this->nx * this->ny
// void Ccalibration::addPhasePower2(float phase, float **image, int nx, int ny, int nFFTPoints, int nHeis) {
// 	// TODO Auto-generated constructor stub
// 	//
// 	// printf("\npoints nx = %d\n",nx);
// 	// printf("\npoints ny = %d\n",ny);
// 	if (!this->initialized)
// 		return;

// 	this->phaseList[this->power_len] = phase;
// 	this->powerList[this->power_len] = this->__getPower(image,nx,ny);
// 	// this->powerList[this->power_len] = this->getOptFunction(image, nFFTPoints, nHeis, nx, ny);

// 	this->desvList[this->power_len] = this->__getDesv(image,nx,ny);

//  	// 128   a   8   ->  1024=ny; 
// 	for(UINT i=0; i< this->ny; i++){
// 		//printf("\nindex = %d  %d \n",this->power_len, i);
// 		//dispMatrix[this->power_len][i] = 1.0;
// 		dispMatrix[this->power_len][i] = this->getDispIndx(image,nx,this->nx,i);
// 	}

// 	printf("\nphase nro = %d\n",	this->power_len);
// 	this->power_len += 1;

// }

void Ccalibration::addPhasePower(float phase, float **image, int nx, int ny) {
	// TODO Auto-generated constructor stub
	// printf("\npointsx = %d\n",this->nx);
	// printf("\npointsy = %d\n",this->ny);
	if (!this->initialized)
		return;

	this->phaseList[this->power_len] = phase;
	this->powerList[this->power_len] = this->__getPower(image,nx,ny);

	this->desvList[this->power_len] = this->__getDesv(image,nx,ny);

	printf("\nphase nro = %d\n",	this->power_len);
	this->power_len += 1;

}

int Ccalibration::getPhaseIndex(){
	int index_min, index_max, index;
	float norm_power[this->power_len], norm_desv[this->power_len];
	float norm_power2[this->power_len];
//	float delta2_power[this->power_len], delta_power[this->power_len];
//	const int factor = 10;

	index_min = getMinIndex(this->desvList, this->power_len);
	index_max = getMaxIndex(this->desvList, this->power_len);

	for (int i=0; i<this->power_len; i++)
		norm_desv[i] = (this->desvList[i] - this->desvList[index_min])/(this->desvList[index_max] - this->desvList[index_min]);

	index_min = getMinIndex(this->powerList, this->power_len);
	index_max = getMaxIndex(this->powerList, this->power_len);

	for (int i=0; i<this->power_len; i++)
		norm_power[i] = (this->powerList[i] - this->powerList[index_min])/(this->powerList[index_max] - this->powerList[index_min]);
	
	//////////////////////////////////////////
	this->meanFilter(norm_power,norm_power2, 7 , this->power_len);
	for (int i=0; i<this->power_len; i++)
		norm_power2[i] = (norm_power[i] - norm_power[index_min])/(norm_power[index_max] - norm_power[index_min]);
	
	index_min = getMinIndex(norm_power2, this->power_len);
	index_max = getMaxIndex(norm_power2, this->power_len);
	index = index_max;
	//////////////////////////////////////////

	// if (this->nChannels < 3)
	// 	index = index_min;
	// else
	// 	index = index_max;

	//***********************************************************
	//float norm_dispMatrix[this->power_len][N_POINTS_Y];
	//uint8_t normIntMatrix[this->power_len][N_POINTS_Y];
	//uint8_t normPowerMatrix[this->power_len][N_POINTS_Y];
	// float normIntMatrix[this->power_len][N_POINTS_Y];
	// float normPowerMatrix[this->power_len][N_POINTS_Y];
	//normMatrixInt(dispMatrix, normIntMatrix,this->power_len, this->ny,255);
	// normMatrix(dispMatrix, normIntMatrix,this->power_len, this->ny);
	// normMatrix(powerMatrix, normPowerMatrix,this->power_len, this->ny);

	//cv::Mat normIMG(this->power_len,N_POINTS_Y, CV_8U, &normIntMatrix);
  	//cv::Mat normPWR(this->power_len,N_POINTS_Y, CV_8U, &normPowerMatrix);

	//cout << "P = " << endl << " " << normPWR << endl << endl;

	// cv::Mat IMG;
	// cv::Mat greyImg = cv::Mat( this->power_len,this->ny, CV_8U, &normIntMatrix);
	// std::string greyArrWindow = "Grey Array Image";
	// cv::resize(greyImg,IMG,cv::Size(128,512),cv::INTER_AREA);
	// cv::Mat img_color;
	// cv::applyColorMap(IMG, img_color, cv::COLORMAP_HOT);
	// //cv::applyColorMap(greyImg, img_color, cv::COLORMAP_HOT);
	// cv::namedWindow(greyArrWindow, cv::WINDOW_NORMAL);
	// //cv::resizeWindow(greyArrWindow, 100, 580);
	// cv::imshow(greyArrWindow, img_color);
	//
	// cv::waitKey(0);
	// cv::destroyAllWindows();

	// std::cout << "POTENCIA: "<<endl;
	// for (int i = 0; i < this->power_len; i++) {
	// 		for (int j = 0; j < this->ny; j++) {
	// 				printf("%1.3f  ",normPowerMatrix[i][j]);
    //     	//std::cout << norm_dispMatrix[i][j] << '      ';
	// 			}
	// 		std::cout << endl;
    // }

	// 	std::cout << "DISPERSION: "<< endl;
	// 	for (int i = 0; i < this->power_len; i++) {
	// 			for (UINT j = 0; j < this->ny; j++) {
	// 					printf("%1.3f  ",normIntMatrix[i][j]);
	//         	//std::cout << norm_dispMatrix[i][j] << '      ';
	// 				}
	// 			std::cout << endl;
	//     }
//***********************************************************

	printf("\n\nIndex \t Norm power \t desv std \n");
	for (int i=0; i<this->power_len; i++)
		printf("[%02d] = %8.4f\t%8.4f \n",i, norm_power2[i], norm_desv[i]);
	printf("\nmin-index=%d, max-index=%d", index_min, index_max);
	printf("\nMin phase[min-index]=%f, Max phase[max-index]=%f\n", this->phaseList[index_min], this->phaseList[index_max]);
	//printf("\nmax diference=%f at index=%d with phase=%f\n",diff_power_desv[max_diff_index],max_diff_index,phase_diff);

	return index;

}

float Ccalibration::estimatePhase(){
	int index;
	float phase;

	if (!this->initialized)
		return 0;

	index = this->getPhaseIndex();

	if (index < 0) return 0;

	if (this->phaseList[index] <= PI)
		phase = this->phaseList[index];
	else
		phase = this->phaseList[index] - 2*PI;

	this->estimated_phase_list[this->nChannels-1] = phase;

	return phase;

}

void Ccalibration::printEstimatedPhases(){
	printf("\nAuto-calibrated phases = ");
	for(unsigned int i=0; i<this->max_nchannels; i++)
		printf("%4.2f, ", this->estimated_phase_list[this->chan_sequence[i]]);
	printf("\n");

}

bool Ccalibration::isTrust(){
	bool trusted = true;

	for (int i=0; i<this->power_len-1; i++){
		if (this->errorList[i] == false)
			continue;

		if (trusted == true)
			printf("\nErrors found on phases = ");

		trusted = false;
		printf("%5.2f, ", phaseList[i]);
	}
	printf("\n");
	return trusted;
}
