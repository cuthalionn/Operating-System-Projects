#include  <stdio.h>
#include  <string.h>
#include  <sys/types.h>

#define   MAX_COUNT  100
#define   BUF_SIZE   100

void  main(void)
{
     pid_t  pid,pidParent,root;
     int    i;
     char   buf[BUF_SIZE];
     char myLetter  = 'a'; 
     root = getpid();
     int count = 0;
     do{
	pidParent = getpid();
     	pid =fork();
	if(pid == 0){ 
		if(count==0) myLetter='b';
		else if(count == 1)
	        { 
			count = 0;
			myLetter='c';
			do {
			    pidParent = getpid();
                            pid = fork();
			    if(pid == 0){
			    	if(count == 0) myLetter='d';
				if(count == 1) myLetter='e'; 
			    }
			count++;
			}while(pid!=0 && count < 2);	
		
		}
		else {
			myLetter = 'f';
			pid = fork();
			if(pid==0) myLetter= 'g';
		}
	}
        count++;
     }while(pid!=0 && count<3);//only parent does this part 

     while(1){
	     for (i = 1; i <= MAX_COUNT; i++) {
		  pid = getpid();
		  pidParent = getppid();
		  sprintf(buf, "This line is from pid %d,whose parent process is = %d: %c \n", 			pid,pidParent,myLetter);
		  write(1, buf, strlen(buf));
		   sleep(50);
	     } 
	     char *cmd= "ps aux";
	     //system(cmd);
	     //the command makes terminal pretty ugly
	    
     }
}
