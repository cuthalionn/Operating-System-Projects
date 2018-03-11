#include <stdio.h>
#include <stdlib.h>
 
struct node {
    int pid;
    int data;
    int weight;
    struct node *next;
};
typedef struct node NODE;
 
/* linked list functions */
void insert(NODE *, int , int,int);
int delete(NODE *);
void traverse(NODE *);
int count(NODE * );
int total(NODE * );
int minTime(NODE*);
int* allPids(NODE*);



void insert(NODE *head, int data, int pid,int weight)
{
    NODE * newnode;
    newnode = (NODE *)malloc(sizeof(NODE));
    newnode->data = data;
    newnode->pid = pid;
    newnode->weight = weight;
    NODE * previous = head;
    NODE * current = head->next;
    while (current != NULL && data >= current->data)
    {
        previous = current;
        current = current->next;
    }
    previous->next = newnode;
 newnode->next = current;
}
 
//change it in a way so that it deletes he first node and returns its pid  
int deleteFirst(NODE *head)
{
    NODE * previous = head;
    NODE * current = head->next; 
    int pidCur = current->pid;
    if (current != NULL) /* if the list is not empty  */
    {
        previous->next = current->next;
        free(current);
        return pidCur;
    }
    else
        return -1;
}
 
void traverse(NODE * head)
{
    NODE * current = head->next;
    while (current != NULL)
    {
        printf("%d %d \n", current->data, current->pid);
        current = current->next;
    }
    printf("\n");
}

int count(NODE * head)
{
    int result=0;
    NODE * current = head->next;
    while (current != NULL)
    {
        result++;
        current = current->next;
    }
    return result;
}
int total(NODE * head)
{
    NODE * current = head->next;
    int result=0;
    while (current != NULL)
    {
        result+= current->weight;
        current = current->next;
    }
    return result;
}
int minTime(NODE*head)
{
     NODE * current = head->next;
     return current->data;
}
int* allPids(NODE*head)
{
    int count=0;
    NODE * current = head->next;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }


    int *arr=(int*) malloc(count*sizeof(int));
    current = head->next;
    int i=0;
    while (current != NULL)
    {
        arr[i]= current->pid;
        current = current->next;
        i++;
    }
    return arr;
}