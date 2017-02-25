#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define P(s) semop(s, &pop, 1)  /* pop is the structure we pass for doing
				   the P(s) operation */
#define V(s) semop(s, &vop, 1)  /* vop is the structure we pass for doing
				   the V(s) operation */



int main()
{
	int bufid,sumid,inid,outid,countid;
	int mutex,full,empty;
	int status;
	int m,n,k=0;
	printf("Enter value of m and n ");
	scanf("%d%d",&m,&n);
	int id[100];
	
	//Creating and initializing shared variables
	bufid=shmget(IPC_PRIVATE,20*sizeof(int),0777|IPC_CREAT);
	sumid=shmget(IPC_PRIVATE,sizeof(int),0777|IPC_CREAT);
	inid=shmget(IPC_PRIVATE,sizeof(int),0777|IPC_CREAT);
	outid=shmget(IPC_PRIVATE,sizeof(int),0777|IPC_CREAT);
	countid=shmget(IPC_PRIVATE,sizeof(int),0777|IPC_CREAT);
	if(bufid==-1 || sumid==-1 || inid==-1 || outid==-1 || countid==-1)
	{
		perror("Error: ");exit(-1);
	}

	
	int *buf,*sum,*in,*out,*count;

	buf=(int *) shmat(bufid, 0, 0);
	sum=(int *) shmat(sumid, 0, 0);
	in=(int *) shmat(inid, 0, 0);
	out=(int *) shmat(outid, 0, 0);
	count=(int *) shmat(countid, 0, 0);
	
	if(*buf==-1 || *sum==-1 || *in==-1 || *out==-1 || *count==-1)
	{
		perror("Error: ");exit(-1);
	}
	*sum=0;*in=0;*out=0;*count=(50*m);
	////////////////////////////////////
	
	

	//Creating and initializing semaphores
	struct sembuf pop,vop;
	pop.sem_num = vop.sem_num = 0;
	pop.sem_flg = vop.sem_flg = 0;
	pop.sem_op = -1 ; vop.sem_op = 1 ;

	mutex= semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
	full = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
	empty = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);

	if(mutex==-1 || full==-1 || empty==-1)
	{
		perror("Error");
		exit(-1);
	}

	semctl(mutex,0,SETVAL,1);
	semctl(full,0,SETVAL,20);
	semctl(empty,0,SETVAL,0);
	////////////////////////////////////




	for(int i=0;i<m;i++)
	{
		id[k++]=fork();
		if(!id[k-1])
		{
			//Producer code

			int *buf,*in;

			buf=(int *) shmat(bufid, 0, 0);
			in=(int *) shmat(inid, 0, 0);

			
			//printf("Asket\n");
			for(int j=1;j<=50;j++)
			{
				P(full);
				P(mutex);
				//CS
				buf[*in]=j;
				//printf("%d writes  %d\n",getpid(),buf[*in]);
				*in=(*in+1)%20;

				V(mutex);
				V(empty);
			
			}
			shmdt(buf);shmdt(in);
			exit(0);
		}
	}
	for(int i=0;i<n;i++)
	{
		id[k++]=fork();
		if(!id[k-1])
		{
			//consumer code

			int *buf,*sum,*out,*count;

			buf=(int *) shmat(bufid, 0, 0);
			sum=(int *) shmat(sumid, 0, 0);
			out=(int *) shmat(outid, 0, 0);
			count=(int *) shmat(countid, 0, 0);

			while(1)
			{	if(*count==0)
				{ V(empty);	break;}
				
				P(empty);if(*count==0)	{ V(empty); break;}
				P(mutex);
				
				//CS
				*sum=(*sum)+(buf[*out]);
				//printf("%d reads %d\n",getpid(),buf[*out]);
				*out=((*out)+1)%20;
				*count=(*count)-1;
				V(mutex);
				V(full);
				
			}
			printf("%d exits count= %d\n",getpid(),*count);
			shmdt(buf);shmdt(sum);shmdt(out);shmdt(count);

			exit(0);

		}
	}

	//wait(NULL);// Wait for all process to finish
	while(k--)
		waitpid(id[k],&status,0);



	semctl(mutex, 0, IPC_RMID, 0);
	semctl(full, 0, IPC_RMID, 0);
	semctl(empty, 0, IPC_RMID, 0);
	printf("The value of the shared variable SUM is : %d \n",*sum);
	printf("The value of the shared variable COUNT is : %d \n",*count);
	shmdt(buf);shmdt(sum);shmdt(out);shmdt(count);shmdt(in);
	shmctl(bufid, IPC_RMID, 0);
	shmctl(sumid, IPC_RMID, 0);
	shmctl(outid, IPC_RMID, 0);
	shmctl(inid, IPC_RMID, 0);
	shmctl(countid, IPC_RMID, 0);


	

	return 0;
}