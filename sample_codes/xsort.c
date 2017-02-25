#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main(int argc,char* argv[])
{
	if(argc<2)
		printf("No filename entered");

	int id=fork();
	if(id==0)
	{
		execl("/usr/bin/xterm","xterm","-e","./sort1",argv[1],(char*)NULL);
		perror("Execl failed: ");
	}
	return 0;
}