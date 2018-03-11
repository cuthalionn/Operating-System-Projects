/* This is the header file for orLinkList.c */
struct node {
    int data;
    int pid;
    int weight;
    struct node *next;
};
typedef struct node NODE;
void insert(NODE *, int ,int,int );
int deleteFirst(NODE *);
void traverse(NODE *);
int count(NODE * );
int total(NODE * );
int minTime(NODE*);
int* allPids(NODE*);