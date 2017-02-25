/* This is process Y .It can read data and update CGPA. It implements both reader and writer code*/




#include <stdio.h>
#include <errno.h>
#include<unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define P(s) semop(s, &pop, 1)  /* pop is the structure we pass for doing
				   the P(s) operation */
#define V(s) semop(s, &vop, 1)  /* vop is the structure we pass for doing
				   the V(s) operation */


int main()

{
	struct record{
		char fname[20];
		char lname[20];
		int roll;
		float cg;
	};


	//Shared variables and semaphores same as in X

	int shmid=shmget(ftok("/etc",20),100*sizeof(struct record),0777|IPC_CREAT);

	struct record* arr=(struct record*)shmat(shmid,0,0);
	int rcid=shmget(ftok("/etc",21),sizeof(int),0777|IPC_CREAT);
	int *rc=(int*)shmat(rcid,0,0);
	int ccid=shmget(ftok("/etc",24),sizeof(int),0777|IPC_CREAT);
	int *cc=(int*)shmat(ccid,0,0);
	int nrid=shmget(ftok("/etc",25),sizeof(int),0777|IPC_CREAT);
	int *n=(int*)shmat(nrid,0,0);
	int semwrt=semget(ftok("/etc",22),1,0777|IPC_CREAT);
	int semmutex=semget(ftok("/etc",23),1,0777|IPC_CREAT);
	int check_id=shmget(ftok("/etc",26),sizeof(int),0);
	
	int done=0;
	while(check_id==-1)			//Condition to ensure X is started before Y starts executing 
	{

		if(errno==ENOENT)		//if errno is ENOENT there was no shared variable that existed and IPC_CREAT was not specified.
		{
			check_id=shmget(ftok("/etc",26),sizeof(int),0);
		}
		else {perror("Error: ");exit(-1);}
		if(done==0)	printf("Waiting for X\n");
		done=1;

	}



	struct sembuf pop, vop ;
	pop.sem_num = vop.sem_num = 0;
	pop.sem_flg = vop.sem_flg = 0;
	pop.sem_op = -1 ; vop.sem_op = 1 ;

	while(1)
	{
		printf("1. Search for student\n");
		printf("2. Update CGPA\n");
		printf("3. Exit\n");
		printf("Enter Choice ");
		int c;
		scanf("%d",&c);
		if(c==1)			//Reader Code
		{
			printf("Enter roll ");
			int r;
			scanf("%d",&r);
			
			P(semmutex);	//Entry Section
			(*rc)=(*rc)+1;
			if((*rc)==1)
				P(semwrt);

			V(semmutex);
			int flag=0;		//Critical Sec
			for(int i=0;i<(*n)-1;i++)
			{
				if(arr[i].roll==r)
				{	printf("%-20s%-20s%-4d%-4f\n",arr[i].fname,arr[i].lname,arr[i].roll,arr[i].cg);
					flag=1;break;}
				
			}
			if(flag==0)
					printf("Student Not Found\n");

		
			P(semmutex);		//Exit section
			(*rc)=(*rc)-1;
			if((*rc)==0)
				V(semwrt);

			V(semmutex);
		}

		else if(c==2)		//Writer Code
		{
			printf("Enter roll ");
			int r;
			scanf("%d",&r);
			float x;
			printf("Enter new CGPA ");
			scanf("%f",&x);
			
			P(semwrt);		//Entry Section
			int flag=0;
			for(int i=0;i<(*n)-1;i++)		//Critical sec
			{
				if(arr[i].roll==r)
				{	
					arr[i].cg=x;
					flag=1;
					*cc=1;
					printf("CGPA updated\n");
					break;
				}
				
			}
			if(flag==0)
					printf("Student Not Found\n");

			V(semwrt);			//Exit Section

		}

		else if(c==3)		//detach all shared variables
		{
			shmdt(arr);shmdt(cc);shmdt(rc);shmdt(n);exit(0);
		}
		else {
			printf("Enter Choice again ");
		}
	}
	

	return 0;
}