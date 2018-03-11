

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "function.h"

#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[])
{
	double y; 
	char write_msg[BUFFER_SIZE];
	char read_msg[BUFFER_SIZE];
	//y = compute_f(100);
	double intervalLow;
	double intervalUp;

	pid_t pid;
	pid = getpid();
	int L,U,K,N;
	L= atoi(argv[1]);//lower value of x
	U= atoi(argv[2]);//upper value of x
	K= atoi(argv[3]);//number of subintervals
	N= atoi(argv[4]);//number of child processes
	int pipes[N][2];//Created pipes will be held here, for the parent to read them
	int *fd;//current process' fd, for processes to write in them
	
	for(int i=0;i<N;i++){
		if(pid>0)
		{
			//Create the pipe 
			if(pipe(pipes[i])==-1){
				fprintf(stderr,"Pipe failed");
				return 1;
			}
			
			//Create the child process 
			pid = fork();
			if(pid < 0)
			{
				fprintf(stderr,"Fork Failed");
				return 1;		
			}
			if(pid == 0)
			{
				fd=pipes[i];
				intervalLow= L+(i)*((1.0*(U-L))/N);
				intervalUp= intervalLow + ((1.0*(U-L))/N);
				break;
			}
			
		}
	}
	
	//only for child processes compute the values and write them to pipe 
	if(pid == 0){
		//Message to be passed	
		double computedValue;
		
		for(int i=0;i<K;i++){
			double trapLow= intervalLow+((intervalUp-intervalLow)/K)*i;
			double trapUp= trapLow + ((intervalUp-intervalLow)/K);
			double currentinteg= (trapUp-trapLow)*((compute_f(trapLow)+compute_f(trapUp))/2.0);
			computedValue+= currentinteg;
		}
		close(fd[READ_END]);
		write(fd[WRITE_END],&computedValue,sizeof(computedValue));
		close(fd[WRITE_END]);	
	}

	// In the parent process read from all pipes and add them up to find the final value
	if(pid>0){
		for(int i=0;i<N;i++)
		{
			double computedPart ;
			close(pipes[i][WRITE_END]);
			read(pipes[i][READ_END],&computedPart,sizeof(computedPart));
			y += computedPart;
			close(pipes[i][READ_END]);
		}
	
		for(int i=0;i<N;i++)
			wait(NULL);

		printf ("%lf\n", y);
	}
	

}
