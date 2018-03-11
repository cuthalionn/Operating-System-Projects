#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])

{
	char cmd[100];
	


	strcpy(cmd, argv[1]);

	for(int i=2 ; i<argc ; i++){
		strcat(cmd, " ");
		strcat(cmd, argv[i]);
	}

	execlp("/bin/sh", "shell","-c", cmd, (char *)NULL);

	return 0;

}
