#include<stdio.h>
#include<pthread.h>
void *counter(void *param){
	for(int i=0;i<100;i++)
		printf("%d ",i);
		
	printf("\n");
	pthread_exit(0);
}

main(int argc,char* argv[]){
	int noOfthreads = atoi(argv[1]);
	
	pthread_t tid[noOfthreads];
	pthread_attr_t attr;
	
	int a;

	pthread_attr_init(&attr);

	for(int i=0;i<noOfthreads;i++)
		pthread_create(&tid[i],&attr,counter,a);


	for(int i =0;i<noOfthreads;i++)
		pthread_join(tid[i],NULL);

}
