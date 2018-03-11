#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char const *argv[])
{

	mqd_t messq;
    struct mq_attr atr;


    atr.mq_flags = 0;
    atr.mq_maxmsg = 10;
    atr.mq_msgsize = 6;
    atr.mq_curmsgs = 0;



    messq = mq_open("/10k", O_CREAT | O_WRONLY, 0644, &atr);
    if(messq==-1) fprintf(stderr, "%s:%d:%s ", __func__, __LINE__, strerror(errno)); 

	pid_t pid;
	pid = fork();
	if(pid!=0){
		pid = fork();
		if(pid!=0)
			pid = fork();
	}

	if(pid == 0){//Child Processes will count until 10k 
		for(int i=1; i<10001;i++){
			int writeSize= malloc(6*sizeof(char));
			char* writeVal=(char *)writeSize ;
			sprintf(writeVal, "%d", i);
			mq_send(messq, writeVal, strlen(writeVal) , 0);//and send these values to queue
		}
		mq_close(messq);
	}
	else{//Meanwhile Parent process will read those messages 
		mq_close(messq);
		execlp("./C", "c",(char *)NULL);
	}

	return 0;
}
