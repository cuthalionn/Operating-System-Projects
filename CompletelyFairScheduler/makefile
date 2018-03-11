
all: cfs loadgen

orLinkList.o: orLinkList.c
	gcc -c -g -Wall -o orLinkList.o orLinkList.c 

cfs: cfs.c orLinkList.o
	gcc -g -Wall -o cfs orLinkList.o cfs.c
	
loadgen: loadgen.c 
	gcc -o loadgen loadgen.c -lm

