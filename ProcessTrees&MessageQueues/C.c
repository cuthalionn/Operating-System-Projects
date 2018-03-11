#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>


int main(int argc, char const *argv[]){
    mqd_t messq;

    
    messq = mq_open("/10k", O_RDONLY);
    if(messq==-1)
    	fprintf(stderr, "%s:%d:%s ", __func__, __LINE__, strerror(errno)); 
    char curQ[6];
    int curP=0;//counter for the result of the process# 

    while(curP<3)
    {//Since there 3 processes writing to queue we set curP to 3 
	ssize_t read;
	read = mq_receive(messq, curQ, 7, NULL);
	if(read>=0){//if anything read exists
		curQ[read]= '\0';
		printf("%s\n", curQ);
		if (atoi(curQ)==10000)
		    curP++;//Whenever the program encounters a 10k it means we have finished reading a whole process
	}
    }
    mq_close(messq);
    mq_unlink("/10k");
}
