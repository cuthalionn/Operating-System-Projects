	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdbool.h>
	#include "orLinkList.h"

	/*Define a new structure here called runqueue having following:
		an ordered linked list for ready queue
		Currently running process
		min_vruntime
		no of process in runq
		load of the runqueue -> total weight of processes in runq (weights are given as function of prio)*/ 

	/* Question: say we had a process ending its io burst in tthe middle of the loop (ex: it had ..5 ms of io burst) should the algorihtm preempt it at that instant or
	should it wait until next iteration of 10ms. If it has to do it instantly then having 10 ms loops is useless we have to have it in iterations of 1ms  */

	struct runq{
		NODE readyQ;
		int runningP;
		int currentRunTime;
		int min_vruntime;
		int numOfP;
		int targetedLatency;
		double timeslice;
	};

	typedef struct runq RUNQ;
	static const int prio_to_weight[40] = {
	/* -20 */ 88761, 71755, 56483, 46273, 36291,
	/* -15 */ 29154, 23254, 18705, 14949, 11916,
	/* -10 */ 9548, 7620, 6100, 4904, 3906,
	/* -5 */ 3121, 2501, 1991, 1586, 1277,
	/* 0 */ 1024, 820, 655, 526, 423,
	/* 5 */ 335, 272, 215, 172, 137,
	/* 10 */ 110, 87, 70, 56, 45,
	/* 15 */ 36, 29, 23, 18, 15,
	};

	char* concat(const char *s1, const char *s2)
	{
	    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the null-terminator
	    //in real code you would check for errors in malloc here
	    strcpy(result, s1);
	    strcat(result, s2);
	    return result;
	}
	bool hasOne(int* arr,int size){
		for(int i=0;i<size;i++){
			if(arr[i] == 1)
				return true;
		}
		return false;
	}
	#define MAXCHAR 1000
	int main(int argc, char* argv[]){
		

		
		char* InputFileName = argv[1];
		char* OutputFileName = argv[2];
		// Read all processes and store them in a double array with different row sizes 
		// regarding their number of cpu-i/o sequences.
		
		FILE *fp;
	    	char str[MAXCHAR];
	    	char* filename = InputFileName;
	 
	   	fp = fopen(filename, "r");
	   	if (fp == NULL){
	        	printf("Could not open file %s",filename);
	        	return 1;
	   	}
		int **cpu;//first entry is no of cpus
		int **io;
		int pidCount=0;
		int pid=0,start,prio;
		
		cpu = (int**)(malloc(sizeof(int*)*500));
		
		int **pids;
		io = (int**)(malloc(sizeof(int*)*500));
		pids =(int**)( malloc(sizeof(int*)*500));
		int cpuCount = 0;
		int ioCount = 0;
		int processedPid=-1;
		//reads the file line by line
		
		int maxTime = 0;
	    	while (fgets(str, MAXCHAR, fp) != NULL){
			char * token;
	  		token = strtok (str, " ");
			//for every disinct pid create array
			if(atoi(token) != processedPid && atoi(token) !=NULL){
				cpu[pidCount] = (int*)(malloc(sizeof(int)*50));
				io[pidCount++] = (int*)(malloc(sizeof(int)*50));
				//printf("I just incremented Pidcount to%d \n",pidCount);
				cpuCount = 1;
				ioCount=1;
				cpu[pidCount-1][0]=0;//number of cpu bursts
				io[pidCount-1][0]=0;//number of io bursts
				pids[pidCount-1]=(int*)(malloc(sizeof(int)*4));
				pids[pidCount-1][3]=0;// Initialized vruntime for a process
			}
			
			
			
			//should handle the case for max 
			while(token != NULL)
			{	
				if(atoi(token)!=NULL){
					pid =  atoi(token);
					processedPid = pid;
					pids[pidCount-1][0]=pid;
				}
				if(token[0]=='s')
				{
				   token = strtok(NULL," ");
				   start=atoi(token);	
				   pids[pidCount-1][1]= start; 
				   maxTime += start;
				   token = strtok(NULL," ");
	 			   token = strtok(NULL," ");
				   prio = atoi(token);
				   pids[pidCount-1][2]= prio;
				   //printf("I just set pid %d's start to %d \n",pids[pidCount-1][0],start);
				   break;	
				}
				if(token[0]=='c'){
				   token = strtok(NULL," ");
				   cpu[pidCount-1][cpuCount++]= atoi(token);
				   cpu[pidCount-1][0] = cpu[pidCount-1][0] +1;
				   break;
				}
				if(token[0]=='i'){
				   token = strtok(NULL," ");
				   io[pidCount-1][ioCount++]= atoi(token);
				   io[pidCount-1][0] = io[pidCount-1][0] +1;
				   break;
				}
				token = strtok(NULL," ");
			}
		}
		
	    	fclose(fp);

		//printf("pid is %d start is %d prio is %d vruntime is %d\n",pids[0][0],pids[0][1],pids[0][2],pids[0][3]);	
		//printf("pid is %d start is %d prio is %d vruntime is %d\n",pids[1][0],pids[1][1],pids[1][2],pids[0][3]);	
		//printf("pid is %d start is %d prio is %d vruntime is %d\n",pids[2][0],pids[2][1],pids[2][2],pids[0][3]);	

		for(int i=0; i<pidCount; i++){
			for(int j=1;j<cpu[i][0]+1;j++)
				maxTime += cpu[i][j];
			}
		

		for(int i=0; i<pidCount; i++){
			for(int j=1;j<io[i][0]+1;j++)
				maxTime += io[i][j];
			}
		
		RUNQ runQueue;
		runQueue.readyQ.pid = 0;
		runQueue.readyQ.data = 0;
		runQueue.readyQ.next = NULL;
		runQueue.targetedLatency= 200;// Just a temmmporary change it is normally 200
		runQueue.runningP=-1;
		runQueue.min_vruntime=0;
		//In the while loop whenever you add a process to runq initialize its vruntime to
		//min_vruntime-10 miliseconds(vonvert to ns)
		int system_time = 0;
	 	int ioPids[pidCount];//The pids of processes currently doing io
		for(int i=0;i<pidCount;i++)ioPids[i]=0;
		//printf("rq is of big %d\n",count(&runQueue.readyQ) );

	    int comeTime[pidCount];
	    for(int i=0;i<pidCount;i++)
			comeTime[i]=pids[i][1];

		 int ioTime[pidCount];
	    for(int i=0;i<pidCount;i++)
			ioTime[i]=0;

		int hasRun[pidCount];
		for(int i=0;i<pidCount;i++)
			hasRun[i]=0;

		int finishes[pidCount];
		for(int i=0;i<pidCount;i++)
			finishes[i]=0;


		char* output= "";
		int idleTime=0;
		int statistics[pidCount][4];//[3]-> no of io to cpu,[4]-> total wait
		for(int i=0;i<pidCount;i++)
			for(int j=0;j<4;j++)
				statistics[i][j]=0;
		int pidSec=0;
		if(pidCount>1)
			pidSec=pids[1][0];
		int conSwCount=0;
		while(system_time<maxTime+1000){//just to be safe
			char *outTemp="";
			bool conSw = false;
			bool readyQUpdate = false;
			//The loop for getting new processes to the ready queue
			for(int i=0;i<pidCount;i++)
			{
				if(pids[i][1] == system_time){
					pids[i][3]= runQueue.min_vruntime-10;
					//printf("I am inserting %d %d and its %dth step in time %d\n",pids[i][3],pids[i][0],i,system_time);
					insert(&runQueue.readyQ,pids[i][3],pids[i][0],prio_to_weight[pids[i][2]]);
					//min_vruntime update
					int firstNodeVruntime =  minTime(&runQueue.readyQ);
					int currentMin;
					if(runQueue.runningP!=-1){
						int pidIndex=0;
						while(runQueue.runningP !=pids[pidIndex][0])
							pidIndex++;
						currentMin= (pids[pidIndex][3]<firstNodeVruntime) ? pids[pidIndex][3]: firstNodeVruntime;

					}
					else
						currentMin= firstNodeVruntime;
					runQueue.min_vruntime=(currentMin>runQueue.min_vruntime) ? currentMin : runQueue.min_vruntime;

					conSw= true;// If a new process comes to ready queue there is a need for context switch
					readyQUpdate=true;

				}	
			}

			// process the io bursts here 
			// actually instead of this just storing the return time of all processes from io might be a better solution	
			for(int i=0;i<pidCount;i++){
				if(ioPids[i]==0)
					continue;
				int pidIndex=i;
				//Find the first non zero io in io at row pidIndex
				int column=1;
				while(io[pidIndex][column]<=0 && column<io[pidIndex][0]+1)	//io[pidIndex][0] is he number of bursts
					column++;
				if(column==io[pidIndex][0]+1) continue;// No more io bursts
				// at this point we have the row and column value of io burst now just process it 1 more ms
				io[pidIndex][column] = io[pidIndex][column] -1 ;
				//printf("The time is %d: io for pid %d is on process for one more ms left: %d\n",system_time,pids[pidIndex][0],io[pidIndex][column]);
				if(io[pidIndex][column]<=0){// İt will return to readyQ then
					ioPids[i]=0;
					if(pids[pidIndex][3]<runQueue.min_vruntime- runQueue.targetedLatency)
						pids[pidIndex][3]= runQueue.min_vruntime- runQueue.targetedLatency;
					insert(&runQueue.readyQ,pids[pidIndex][3],pids[pidIndex][0],prio_to_weight[pids[pidIndex][2]]);

					//min_vruntime update
					int firstNodeVruntime =  minTime(&runQueue.readyQ);
					int currentMin;
					if(runQueue.runningP!=-1){
						int pidIndex=0;
						while(runQueue.runningP !=pids[pidIndex][0])
							pidIndex++;
						currentMin= (pids[pidIndex][3]<firstNodeVruntime) ? pids[pidIndex][3]: firstNodeVruntime;
					}
					else
						currentMin= firstNodeVruntime;
					runQueue.min_vruntime=(currentMin>runQueue.min_vruntime) ? currentMin : runQueue.min_vruntime;

					conSw=true;// ReadyQ changed we need a context switch,
					readyQUpdate= true;
					//if(runQueue.runningP == -1)printf("The time is %d: cpu is idle for one more ms\n",system_time);
				}
			}
			if(runQueue.runningP == -1 && count(&runQueue.readyQ) > 0 ){

				runQueue.runningP= deleteFirst(&runQueue.readyQ);
				//every time you put a new process update the timeslice and currentRunTime values
				int pidIndex=0;
				while(runQueue.runningP !=pids[pidIndex][0])
					pidIndex++;
				runQueue.timeslice = (double)(runQueue.targetedLatency*prio_to_weight[pids[pidIndex][2]])/(total(&runQueue.readyQ)+prio_to_weight[pids[pidIndex][2]]);
				if(runQueue.timeslice<10) 
					runQueue.timeslice=10;
				runQueue.currentRunTime=0;
					
				//printf("the time is %d: put the new process to cpu with id %d current runtime %d and  timeslice is %f\n",system_time,runQueue.runningP,runQueue.currentRunTime,runQueue.timeslice);
				conSw=false;//if we put a process here no need to do it again
				if( system_time!=0){
					if(runQueue.runningP==pidSec && conSwCount==1){
						idleTime--;
					}
					if(idleTime!=0 ){
						char temp[5];
						output = concat(output,"idle");
						snprintf(temp,sizeof(temp),"%d",idleTime);
						output = concat(output," ");
						output = concat(output,temp);
						output = concat(output,"\n");
						idleTime=0;
					}
				}	
			}
			//Increase the time for current process
			//runQueue.runningP = deleteFirst(&runQueue.readyQ); do it if the timeslice of current proc is above than what is assigned
			if(runQueue.runningP == -1){//Cpu is idle since the ready queue is empty
					if(count(&runQueue.readyQ) == 0){
						
						//printf("the time is %d:  cpu is idle for 1 more ms\n",system_time);
						idleTime +=1;
						system_time+=1;
						continue;// Go out of loop no need to check for cs the ready q is empty
					}
					else{
						runQueue.runningP = deleteFirst(&runQueue.readyQ);
						//every time you put a new process update the timeslice and currentRunTime values
						int pidIndex=0;
						while(runQueue.runningP !=pids[pidIndex][0])
							pidIndex++;

						runQueue.timeslice = (double)(runQueue.targetedLatency*prio_to_weight[pids[pidIndex][2]])/(total(&runQueue.readyQ)+prio_to_weight[pids[pidIndex][2]]);	
						if(runQueue.timeslice<10) runQueue.timeslice=10;
						runQueue.currentRunTime=0;
						
						//printf("the time is %d: put the new process to cpu with id %d current runtime %d and  timeslice is %f\n",system_time,runQueue.runningP,runQueue.currentRunTime,runQueue.timeslice);
						conSw=false;//if we put a process here no need to do it again
						//Increment waiting times
						int* waitings = allPids(&runQueue.readyQ);
						for(int i=0;i<count(&runQueue.readyQ);i++){
							int pid = waitings[i];
							int pidIndex=0;
							while(pids[pidIndex][0]!= pid)
								pidIndex++;
							statistics[pidIndex][1]++;
						}
						system_time+=1;
						continue;// Go out of loop no need to check for cs the ready q is empty
					}
			}
			//printf("**************\n");
		        
			int pidIndex=0;
			while(runQueue.runningP !=pids[pidIndex][0])
				pidIndex++;
			int column=1;
			while(cpu[pidIndex][column]<=0 && column<cpu[pidIndex][0]+1)
				column++;
			//printf("the column is %d pidIndex is %d\n",column,pidIndex);
			int currentRuntime = runQueue.currentRunTime;
			int currentProcess= pids[pidIndex][0];
			bool TimeSliceExpiry=false;
			bool cpuDone=false;
			bool curCpuDone=false;
			if(column==cpu[pidIndex][0]+1)
			{
				//The cpu bursts has ended the process can terminate


				runQueue.runningP = -1;
				readyQUpdate= true;	
				conSw=true;
				cpuDone = true;

				//The running process terminated update min_vruntime
				//min_vruntime update
				if(count(&runQueue.readyQ)>0){
					int firstNodeVruntime =  minTime(&runQueue.readyQ);
					int currentMin;
					currentMin= firstNodeVruntime;
					runQueue.min_vruntime=(currentMin>runQueue.min_vruntime) ? currentMin : runQueue.min_vruntime;
				}

			}
			else{
				if(hasRun[pidIndex]==0){//First time in cpu
					statistics[pidIndex][3]+=system_time-ioTime[pidIndex]-comeTime[pidIndex];
					hasRun[pidIndex]=1;
					statistics[pidIndex][2]= statistics[pidIndex][2]+1;
					//printf("time: %d id: %d added wait time:%d\n",system_time,pids[pidIndex][0]);
				}

				cpu[pidIndex][column] = cpu[pidIndex][column]-1;
				int delta = 1*prio_to_weight[20]/prio_to_weight[pids[pidIndex][2]];
				pids[pidIndex][3]+=delta;//increase vruntime
				runQueue.currentRunTime +=1;

				//Check if all cpus done if so note the turnaround time
				int pidIndexTemp=0;
				while(runQueue.runningP !=pids[pidIndexTemp][0])
					pidIndexTemp++;
				int columnTemp=1;
				while(cpu[pidIndexTemp][columnTemp]<=0 && columnTemp<cpu[pidIndexTemp][0]+1)
				columnTemp++;
				if(columnTemp==cpu[pidIndexTemp][0]+1){
					finishes[pidIndexTemp]=system_time+1;
					statistics[pidIndexTemp][0]= system_time-pids[pidIndexTemp][1]+1;
				}


				if(cpu[pidIndex][column] == 0){
					//If the current cpu made it to 0 either go to io or if no io exists terminate.
					conSw=true;
					ioPids[pidIndex]=1;

					
					int columnTemp=1;
					while(io[pidIndex][columnTemp]<=0 && columnTemp<io[pidIndex][0]+1)
						columnTemp++;

					if(columnTemp<io[pidIndex][0]+1){
						hasRun[pidIndex]=0;
						ioTime[pidIndex]=io[pidIndex][columnTemp];
						comeTime[pidIndex]=system_time;
					}

					
					 
					
					curCpuDone = true;
					//min_vruntime update-> runnigp gets -1
				}
				
				currentRuntime = runQueue.currentRunTime;
				currentProcess= pids[pidIndex][0];
				if(system_time%10 == 9 && runQueue.currentRunTime >= runQueue.timeslice && !curCpuDone){
					//printf("its true yooooooooooooooooooo\n");
					TimeSliceExpiry=true;
					//at every timer tick check if time_slice is reached
					conSw=true;
					outTemp="";
					char temp[5];
					snprintf(temp,sizeof(temp),"%d",runQueue.runningP);
					outTemp = concat(outTemp,temp);
					snprintf(temp,sizeof(temp),"%d",runQueue.currentRunTime);
					outTemp = concat(outTemp," ");
					outTemp = concat(outTemp,temp);
					outTemp = concat(outTemp,"\n");
					runQueue.runningP = -1;
					
					insert(&runQueue.readyQ,pids[pidIndex][3],pids[pidIndex][0],prio_to_weight[pids[pidIndex][2]]);

					//min_vruntime update
					int firstNodeVruntime =  minTime(&runQueue.readyQ);
					int currentMin;
					currentMin= firstNodeVruntime;
					runQueue.min_vruntime=(currentMin>runQueue.min_vruntime) ? currentMin : runQueue.min_vruntime;

				}

			}

			//printf("cpus are like the following: \n");
			//for(int i=0; i<pidCount; i++){                      
			//for(int j=1;j<cpu[i][0]+1;j++)
			//	printf("%d ",cpu[i][j]);
			//	printf("\n");
			//}
			//printf("the time is %d: Just processed one more ms for pid %d its time slice is %f and so far it has run %d ms and its' total vruntime is %d ms\n",system_time,pids[pidIndex][0],runQueue.timeslice,runQueue.currentRunTime,pids[pidIndex][3]);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
			
			if(conSw){
				conSwCount++;
				//printf("Context switch yeeey!\n");
				if(count(&runQueue.readyQ)<=0){//Cpu is idle since the ready queue is empty
					if(runQueue.runningP!=-1){
						char temp[5];
						snprintf(temp,sizeof(temp),"%d",runQueue.runningP);
						output = concat(output,temp);
						snprintf(temp,sizeof(temp),"%d",runQueue.currentRunTime);
						output = concat(output," ");
						output = concat(output,temp);
						output = concat(output,"\n");
					}
					runQueue.runningP = -1;
					idleTime+=1;
					//printf("the time is %d:  cpu is idle from now on\n",system_time);
					system_time+=1;
					continue;// Go out of loop no need to check for cs the ready q is empty
				}
				else{
					if(runQueue.runningP!=-1){
						int pidIndex=0;
						while(runQueue.runningP !=pids[pidIndex][0])
						pidIndex++;
						//printf("should the process with pid %d be inserted to ready q\n",pids[pidIndex][0]);
						if(ioPids[pidIndex]==0){//If the process is not doing io
							insert(&runQueue.readyQ,pids[pidIndex][3],pids[pidIndex][0],prio_to_weight[pids[pidIndex][2]]);
							// no need to update min_vruntime since the process was already in runQueue it just moved from cpu to readyQ
							//printf("done\n");
						}
						else{//Running process went to do io update vruntime
							//min_vruntime update
							int firstNodeVruntime =  minTime(&runQueue.readyQ);
							int currentMin;
							currentMin= firstNodeVruntime;
							runQueue.min_vruntime=(currentMin>runQueue.min_vruntime) ? currentMin : runQueue.min_vruntime;
						}
					}
					
					if(runQueue.runningP!=-1){
						outTemp="";
						char temp[5];
						snprintf(temp,sizeof(temp),"%d",runQueue.runningP);
						outTemp = concat(outTemp,temp);
						snprintf(temp,sizeof(temp),"%d",runQueue.currentRunTime);
						outTemp = concat(outTemp," ");
						outTemp = concat(outTemp,temp);
						outTemp = concat(outTemp,"\n");
					}
					

					runQueue.runningP = deleteFirst(&runQueue.readyQ);
					//printf("noseg till here %d\n",runQueue.runningP);
					pidIndex=0;
					while(runQueue.runningP !=pids[pidIndex][0])
						pidIndex++;
					int column=1;
					while(cpu[pidIndex][column]<=0 && column<io[pidIndex][0]+1)
						column++;
					//every time you put a new process update the timeslice and currentRunTime values


					runQueue.timeslice = (double)(runQueue.targetedLatency*prio_to_weight[pids[pidIndex][2]])/(total(&runQueue.readyQ)+prio_to_weight[pids[pidIndex][2]]);	
					if(runQueue.timeslice<10) runQueue.timeslice=10;
						runQueue.currentRunTime=0;
					if(runQueue.runningP == currentProcess) 
						runQueue.currentRunTime=currentRuntime;
					//printf("time : %d %d =? %d\n",system_time,runQueue.runningP,currentProcess);
					if(TimeSliceExpiry && runQueue.runningP == currentProcess && cpuDone){//timeexpiry but the same process without cpu the system_time will be reprocessed
						output = concat(output,outTemp);
					} 
					if(runQueue.runningP != currentProcess){
						//printf("just do it\n");
						//printf(outTemp);
						output = concat(output,outTemp);
					}
					//If the process that was working before timer tick already is assigned back to cpu its running time should stay same
					//printf("the time is %d: put the new process to cpu with id %d\n",system_time,runQueue.runningP);
				}
			}
			/*cpu[pidIndex][i] for i = ... are all cpus that the running process has. 
			Find the first non zero one**. If a cpu burst gets to zero in the currentt iteration
			it either has to do io for io[runQueue.runningP][k] time (k being the first non zero io) or this process is all done 
			with both io and cpu.
			** It has to have a nonzero one since we check in every loop if a cpu gets to zero and we either terminate it or send it to io. The ones coming
			back from io has to have cpu because no of io burst is eq to cpu burst-1 -> assuming that a process has to start it burst from cpu but io.
			*/
			
			
			if(readyQUpdate)
			{
				if(count(&runQueue.readyQ)>20)
					runQueue.targetedLatency=10*count(&runQueue.readyQ);
				else 
					runQueue.targetedLatency=200;	
			}
			//Increment waiting times
			int* waitings = allPids(&runQueue.readyQ);
			for(int i=0;i<count(&runQueue.readyQ);i++){
				int pid = waitings[i];
				int pidIndex=0;
				while(pids[pidIndex][0]!= pid)
					pidIndex++;
				statistics[pidIndex][1]++;
			}


			system_time+=1;// Im not sure

		 }
		// printf(output);
		 //printf("\n");
		 //int totalResp=0;
		for(int i=0;i<pidCount;i++){
		 	printf("%d %d %d %d %d %d %d \n",pids[i][0],pids[i][2],pids[i][1],finishes[i],statistics[i][0],statistics[i][1],statistics[i][3]/statistics[i][2]);
		 	//totalResp+=statistics[i][3]/statistics[i][2];
		}
		//printf("Average response time is %d\n",totalResp/pidCount);
		FILE* file =  fopen(OutputFileName,"w");
		fputs(output,file);
	}
