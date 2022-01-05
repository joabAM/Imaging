#include "cnoise.h"

float enoise(float *data, int npts, int navg)
{
//     Routine to estimate noise level
//    Based on Hildebrand and Sekhon [1974]
	double *data2;
	double s,s2,leftc,rightc;
//	double p,p2,q;
	double anoise;
	int i,j,nmin;
	int noise_pts=0;
	float k;
	
	if(npts < 100){
		printf("error in noise - requires at least 100 points \n");
		scanf("%d",&i);
		return -1;
	}

	data2 = (double *)malloc(sizeof(double)*(npts+1));
	
	//     data sorted in ascending order
	nmin = (int) (npts*0.3);
	noise_pts = nmin;

	data2[0]=0;

	for(i=0;i<npts;i++){
	  data2[i] = (double) data[i];
	}

	sort((long)npts,data2-1);

//	printf("\nData = ");
//	for(i=0;i<npts;i++){
//		printf("%010.2f, ", data2[i]);
//	}
//	printf("\n");

	s=0.0;
	s2=0.0;
	for(i=0;i<(nmin);i++){
		s += data2[i];
		s2 += pow(data2[i],2);
	}

	for(i=nmin;i<(npts);i++){
		s += data2[i];
		s2 += pow(data2[i],2);


		k = (float)i/(i-1) + 1.0/navg;
		rightc = k*pow(s,2);
		leftc = s2 * i;

//		p=s/((double) i);
//		p2 = pow(p,2);
//		q = s2/((double) i)-p2;
//		leftc = p2;
//		rightc = q*((double) navg);

		if(leftc > rightc){
		//Signal detect: R2 < 1 (R2 = leftc/rightc)
			noise_pts = i;
			break;
		}
	}

//	printf("\nNoise points %d/ Total %d\n", noise_pts, npts);
	anoise=0;

	for(j = 0; j< noise_pts-1;j++){
		anoise=anoise+data2[j];
	}

	anoise=anoise/((double)(noise_pts));

	free(data2);
	return((float) anoise);
}

void sort(unsigned long n, double *arr)
{
	unsigned long i,ir=n,j,k,l=1;
	int jstack=0,istack[500],nstack=500,m=7;
	double a,temp;

	for (;;) {
		if (int(ir-l) < m) {
			for (j=l+1;j<=ir;j++) {
				a=arr[j];
				for (i=j-1;i>=1;i--) {
					if (arr[i] <= a) break;
					arr[i+1]=arr[i];
				}
				arr[i+1]=a;
			}
			if (jstack == 0) break;
			ir=istack[jstack--];
			l=istack[jstack--];
		} else {
			k=(l+ir) >> 1;
			SWAP(arr[k],arr[l+1]);
			if (arr[l+1] > arr[ir]) {
				SWAP(arr[l+1],arr[ir]);
			}
			if (arr[l] > arr[ir]) {
				SWAP(arr[l],arr[ir]);
			}
			if (arr[l+1] > arr[l]) {
				SWAP(arr[l+1],arr[l]);
			}
			i=l+1;
			j=ir;
			a=arr[l];
			for (;;) {
				do i++; while (arr[i] < a);
				do j--; while (arr[j] > a);
				if (j < i) break;
				SWAP(arr[i],arr[j]);
			}
			arr[l]=arr[j];
			arr[j]=a;
			jstack += 2;
			if (jstack > nstack) {
            	fprintf(stderr,"NSTACK too small in sort. now exiting the system ...\n");
				exit(1);
			}
			if (ir-i+1 >= j-l) {
				istack[jstack]=ir;
				istack[jstack-1]=i;
				ir=j-1;
			} else {
				istack[jstack]=j-1;
				istack[jstack-1]=l;
				l=i;
			}
		}
	}
}
