#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
struct student{
	int sid;
	char firstname[64];
	char lastname[64];
	double cgpa;
};
char* inFileName;
char* outFileName;
int noLines;
int N;
int bufferSize;
#define MAXCHAR 1000

//N by M array
typedef struct student Student;
int comp (const void * elem1, const void * elem2) 
{
    Student f = *((Student*)elem1);
    Student s = *((Student*)elem2);
    if (f.sid > s.sid) return  1;
    if (f.sid < s.sid) return -1;
    return 0;
}
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the null-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
pthread_mutex_t mutex[10] = {
PTHREAD_MUTEX_INITIALIZER,
PTHREAD_MUTEX_INITIALIZER,
PTHREAD_MUTEX_INITIALIZER,
PTHREAD_MUTEX_INITIALIZER,
PTHREAD_MUTEX_INITIALIZER,
PTHREAD_MUTEX_INITIALIZER,
PTHREAD_MUTEX_INITIALIZER,
PTHREAD_MUTEX_INITIALIZER,
PTHREAD_MUTEX_INITIALIZER,
PTHREAD_MUTEX_INITIALIZER};
pthread_cond_t empty[10]={
PTHREAD_COND_INITIALIZER,
PTHREAD_COND_INITIALIZER,
PTHREAD_COND_INITIALIZER,
PTHREAD_COND_INITIALIZER,
PTHREAD_COND_INITIALIZER,
PTHREAD_COND_INITIALIZER,
PTHREAD_COND_INITIALIZER, 	
PTHREAD_COND_INITIALIZER,
PTHREAD_COND_INITIALIZER,
PTHREAD_COND_INITIALIZER};
pthread_mutex_t prodMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t production = PTHREAD_COND_INITIALIZER;
Student students[10][1000];// max number of students and max buffer size
int lastInsert[10]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
bool done[10] ={false,false,false,false,false,false,false,false,false,false};

void* produce(void *param){
	int* pid = (int*) param;
	//printf("Producer %d is working now\n", *pid);
	FILE* fp = fopen(inFileName,"r");
	if (fp == NULL){
        printf("Could not open file %s",inFileName);
       	return NULL;
   	}
   	char str[MAXCHAR];
   	while (fgets(str, MAXCHAR, fp) != NULL){
		char * token;
		token = strtok (str, " ");
		//Every thread reads the lines that starts with their own ID
		if(atoi(token)==*pid){
			Student S;
			S.sid= atoi(strtok(NULL," "));
			strncpy(S.firstname,strtok(NULL," "),sizeof(S.firstname));
			strncpy(S.lastname,strtok(NULL," "),sizeof(S.lastname));
			S.cgpa= atof(strtok(NULL," "));
			//Now add the new student to the buffer
			//Use cond variable and mutex here
			pthread_mutex_lock(&mutex[*pid]);
			//pthread_mutex_lock(&prodMutex);
			if(lastInsert[*pid]>=bufferSize-1){
				//printf("I will wait as the producer %d\n",*pid);
				//pthread_mutex_unlock(&prodMutex);
				pthread_cond_wait(&empty[*pid],&mutex[*pid]);
				//pthread_mutex_lock(&prodMutex);
			}
			//printf("Hi I am section %d I wrote student %d current capacity in buffer is %d\n ",*pid,S.sid,lastInsert[*pid]);

			lastInsert[*pid] = lastInsert[*pid] +1;
			//printf("Trying to access producer%d's  %dth place\n",*pid, lastInsert[*pid]);
			students[*pid][lastInsert[*pid]] = S;
			//printf("signaling consumer now; producer %d\n", *pid);
			pthread_cond_signal(&production);
			//printf("just signaled consumer ; producer %d\n", *pid);
			//pthread_mutex_unlock(&prodMutex);
			pthread_mutex_unlock(&mutex[*pid]);
		}
	}
	pthread_mutex_lock(&prodMutex);
	done[*pid]=true;
	pthread_mutex_unlock(&prodMutex);
}
void* consume(void *param){
	//printf("Consumer working now\n");
	Student OrderedStudents[noLines+1];//count the number of students in main create this accordingly
	int orderedIndex = 0;
	bool doneSomething=false;
	//printf("I am fat\n");
	bool boolAllDone=false;
	while(!boolAllDone || doneSomething){
		doneSomething=false;
		//pthread_mutex_lock(&prodMutex);
		boolAllDone=done[0];
		for(int i=1;i<N;i++){
			boolAllDone= boolAllDone && done[i];
		}
		for(int i=0;i<N;i++){
			while(lastInsert[i]>=0){
				pthread_mutex_lock(&mutex[i]);
				//read student add to list
				OrderedStudents[orderedIndex]= students[i][lastInsert[i]];
				lastInsert[i] = lastInsert[i] -1;
				orderedIndex++;
				doneSomething=true;
				//printf("cons signaled producer %d\n", i);
				pthread_cond_signal(&empty[i]);
				pthread_mutex_unlock(&mutex[i]);
			}
		}
		if(!boolAllDone && !doneSomething)// if the producer are not done yet but consumer did nothing then it should wait for a signal form producers
		{
			
				pthread_mutex_lock(&prodMutex);
				//printf("I am waiting \n");
				pthread_cond_wait(&production,&prodMutex);
				//printf("I am free\n");
				pthread_mutex_unlock(&prodMutex);
			
		}
		//pthread_mutex_unlock(&prodMutex);
	}

	//Order the list here
	qsort (OrderedStudents, orderedIndex, sizeof(Student), comp);
	char* output="";
	for(int i=0;i<orderedIndex;i++){
		//printf("The student id is %d the gpa is %f \n",OrderedStudents[i].sid,OrderedStudents[i].cgpa);
		char temp[100];
		snprintf(temp,sizeof(temp),"%d",OrderedStudents[i].sid);
		output = concat(output,temp);
		output = concat(output," ");
		output = concat(output,OrderedStudents[i].firstname);
		output = concat(output," ");
		output = concat(output,OrderedStudents[i].lastname);
		output = concat(output," ");
		snprintf(temp,sizeof(temp),"%.2f",OrderedStudents[i].cgpa);
		output = concat(output,temp);
		output = concat(output,"\n");
	}
	FILE* file =  fopen(outFileName,"w");
	fputs(output,file);
	// read all buffers if all empty and all other threads are finished exit
}

int  main(int argc, char* argv[]){
	N =atoi(argv[1]); // number of Producers 
	bufferSize = atoi (argv[2]);
	inFileName = argv[3];
	outFileName=argv[4];

	FILE* fp1 = fopen(inFileName,"r");
	if (fp1 == NULL){
        printf("Could not open file %s",inFileName);
   	}
   	char str[MAXCHAR];
   	while (fgets(str, MAXCHAR, fp1) != NULL){
		noLines++;
	}
	//printf("noLines:%d\n",noLines );


	int noOfStudents = 100;
	pthread_t tid[N];
	pthread_t consTid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int arr[N];
	for(int i=0;i<N;i++)
	{
		arr[i]=i;
	}

	for(int i=0;i<N ; i++){
		//Open up N many producer threads
		//i is the producer id of the currently created array
		int* param = &arr[i];
		pthread_create(&tid[i],&attr,produce,param);
	}
	//Open a consumer thread 
	pthread_create(&consTid,&attr,consume,0);


	for (int i=0;i<N;i++){
		pthread_join(tid[i],NULL);
	}//Producers are done after this 
	pthread_cond_signal(&production);
	pthread_join(consTid,NULL);

}
