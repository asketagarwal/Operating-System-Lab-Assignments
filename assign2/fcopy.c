#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc,char* argv[])
{
	if(argc!=3)
		printf("Wrong command line arguments/n");

	int f1=open(argv[1],0,"r");
	//int f2=open(argv[2],0,"w");
	if(f1==-1)
	{
		perror("File open failed");
		exit(0);
	}
	int f2=open(argv[2], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(f2==-1)
	{
		perror("File open failed");
		exit(0);
	}
	int fd1[2],fd2[2];
	int a=pipe(fd1);
	int b=pipe(fd2);

	if(a==-1 || b==-1)
	{
		perror("Pipe creation failed");
		exit(1);
	}
	
	int id=fork();
	if(id==-1){perror("Error");exit(1);}
	char buf1[200];
	char buf2[20];

	if(!id)
	{
		close(fd1[1]);close(fd2[0]);
		while(1)
		{
			int temp=read(fd1[0],buf1,100);

			
			int x=write(f2,buf1,temp);
			if(x!=-1)
				sprintf(buf2,"%d",0);
			else sprintf(buf2,"%d",-1);
			write(fd2[1],buf2,4);
			if(temp<100)
			{
				printf("File copied successfully \n");
				exit(1);
			}
		}






	}
	else{
		close(fd1[0]);close(fd2[1]);
		char buf1[200];
		char buf2[20];
	
		while(1)
		{
			int temp=read(f1,buf1,100);
			//printf("%d",temp);
			write(fd1[1],buf1,temp);
			
			
			read(fd2[0],buf2,4);
			int x;
			sscanf(buf2,"%d",&x);
			if(temp<100 && !x)
				exit(1);
			if(x==-1)
				exit(1);

		}
	}

}