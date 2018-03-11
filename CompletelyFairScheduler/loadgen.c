#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
double ran_expo(double mean){
	double lambda= 1 /mean;
	double u;
	u = rand() / (RAND_MAX + 1.0);
	return  -log(1-u) / lambda;
}
int uniform_dist(int rangeLow,int rangeHigh){
	double myRand  = rand() / (1.0 + RAND_MAX);
	int range = rangeHigh- rangeLow +1;
	int myRand_scaled = (myRand*range)+rangeLow;
	return myRand_scaled; 
}
void concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the null-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    s1 = result;
    free(result);
}
int main(int argc, char* argv[]){
	int pidCount = atoi(argv[1]);
	double avgStartTime=atof(argv[2]);
	double avgNumBursts= atof(argv[3]);
	double avgCpuLen=atof(argv[4]);
	double avgIoLen=atof(argv[5]);
	char* filename= argv[6];
	FILE* file =  fopen(filename,"w");
	srand((unsigned)time(NULL));
	//fputs(output,file);
	//snprintf(temp,sizeof(temp),"%d",runQueue.currentRunTime);
	char* output="";
	int before=0;
	int before2=-1;
	for(int i=1;i<pidCount+1;i++){
		int startTime= (int) ran_expo(avgStartTime);
		int numCpuBursts= (int) ran_expo(avgNumBursts)+1;
		int ioBursts = numCpuBursts-1;
		int priority = uniform_dist(0,39);
		char buf[10];
		snprintf(buf,sizeof(buf),"%d",i);
		fputs(buf,file);
		fputs(" start ",file);
		snprintf(buf,sizeof(buf),"%d",startTime);
		fputs(buf,file);
		fputs(" prio ",file);
		snprintf(buf,sizeof(buf),"%d",priority);
		fputs(buf,file);
		fputs("\n",file);
		for(int j=0;j<numCpuBursts+ioBursts;j++){
			if(j%2==0){//cpuBurst
					snprintf(buf,sizeof(buf),"%d",i);
					fputs(buf,file);
					fputs(" cpu ",file);
					int cpu = (int)ran_expo(avgCpuLen)+1;
					snprintf(buf,sizeof(buf),"%d",cpu);
					fputs(buf,file);
					fputs("\n",file);
			}
			else{//ioBurst
					snprintf(buf,sizeof(buf),"%d",i);
					fputs(buf,file);
					fputs(" io ",file);
					int io = (int)ran_expo(avgIoLen)+1;
					snprintf(buf,sizeof(buf),"%d",io);
					fputs(buf,file);
					fputs("\n",file);
			}
		}
		snprintf(buf,sizeof(buf),"%d",i);
		fputs(buf,file);
		fputs(" end\n",file);
	}
}