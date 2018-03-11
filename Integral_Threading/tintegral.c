

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "function.h"
int L,U,K,N;
double y; 
 int count=0;
void *PartialIntegral(void *no){
	double intervalLow;
	double intervalUp;
	int *threadNo = (int*)(no);
	intervalLow= L+(*threadNo)*((1.0*(U-L))/N);
	intervalUp= intervalLow + ((1.0*(U-L))/N);
	double computedValue=0;
	for(int i=0;i<K;i++){
		double trapLow= intervalLow+((intervalUp-intervalLow)/K)*i;
		double trapUp= trapLow + ((intervalUp-intervalLow)/K);
		double currentinteg= (trapUp-trapLow)*((compute_f(trapLow)+compute_f(trapUp))/2.0);
		computedValue+= currentinteg;
	}
	
	y+=computedValue;

	
	pthread_exit(0);
}
int main(int argc, char *argv[])
{
	

	//y = compute_f(100);
	
	L= atoi(argv[1]);//lower value of x
	U= atoi(argv[2]);//upper value of x
	K= atoi(argv[3]);//number of subintervals
	N= atoi(argv[4]);//number of child processes

	pthread_t tid[N];
	pthread_attr_t attr;

	
	int arr[N];
	for(int i=0;i<N;i++)
	{
		arr[i]=i;
	}
	pthread_attr_init(&attr);
	for(int i=0;i<N;i++){
		int* no=&arr[i];
		pthread_create(&tid[i],&attr,PartialIntegral,no);
	}
	
	for(int i =0;i<N;i++)
		pthread_join(tid[i],NULL);
	
	printf ("%lf\n", y);
}
