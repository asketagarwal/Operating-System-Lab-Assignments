
/* This is process X which runs forever. It initializes all the required shared variables 
	and semaphores.It also checks whether there has been a change in the records every 5 sec 
	and updates the file with the changes made. It also implements reader code as it reads the shared variable shmid*/




#include <stdio.h>
#include<unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>

#define P(s) semop(s, &pop, 1)  /* pop is the structure we pass for doing
				   the P(s) operation */
#define V(s) semop(s, &vop, 1)  /* vop is the structure we pass for doing
				   the V(s) operation */
int shmid,rcid,ccid,nrid,check_id,semwrt,semmutex;
void my_handler(int signum)			//signal handler for deleting all shared variables and semaphores.ctrl+C
{
	//shmdt(arr);shmdt(cc);shmdt(rc);shmdt(n);
	shmctl(shmid,IPC_RMID,0);
	shmctl(rcid,IPC_RMID,0);
	shmctl(ccid,IPC_RMID,0);
	shmctl(nrid,IPC_RMID,0);
	shmctl(check_id,IPC_RMID,0);
	semctl(semwrt,IPC_RMID,0);
	semctl(semmutex,IPC_RMID,0);
	printf("Exit with deleting all shared variables\n");
	exit(0);
}

int main(int argc, char const *argv[])
{
	signal(SIGINT, my_handler);
	if(argc!=2)
	{
		printf("wrong no of argument");
		exit(0);
	}

	FILE* f=fopen(argv[1],"r+");
	if(!f)
	{
		perror("Error");
		exit(-1);
	}
	struct record{			//structure for holding the records
		char fname[20];
		char lname[20];
		int roll;
		float cg;
	};

	shmid=shmget(ftok("/etc",20),100*sizeof(struct record),0777|IPC_CREAT);		//shared variable for holding all records
	if(shmid==-1)
		{perror("Error:");
			exit(0);}

	struct record* arr=(struct record*)shmat(shmid,0,0);						
	int no_rec=0;
	while(!feof(f))
	{
		fscanf(f,"%s",arr[no_rec].fname);
		fscanf(f,"%s",arr[no_rec].lname);
		fscanf(f,"%d",&arr[no_rec].roll);
		fscanf(f,"%f",&arr[no_rec].cg);
		no_rec++;
	}
	fclose(f);
	rcid=shmget(ftok("/etc",21),sizeof(int),0777|IPC_CREAT);		//shared variable for holding readcount
	int *rc=(int*)shmat(rcid,0,0);
	
	ccid=shmget(ftok("/etc",24),sizeof(int),0777|IPC_CREAT);		//shared variable to check whether there has been a change in the records
	int *cc=(int*)shmat(ccid,0,0);
	*rc=0;*cc=0;

	nrid=shmget(ftok("/etc",25),sizeof(int),0777|IPC_CREAT);		//shared variable for holding the no. of records
	int *n=(int*)shmat(nrid,0,0);
	*n=no_rec;

	check_id=shmget(ftok("/etc",26),sizeof(int),0777|IPC_CREAT);	//dummy shared variable to check if X runs before any Y
	semwrt=semget(ftok("/etc",22),1,0777|IPC_CREAT);				//semaphore wrt for ensure that when there is a writer performing no other reader can read the data.
	semmutex=semget(ftok("/etc",23),1,0777|IPC_CREAT);				//semaphore to protect readcount against race condition.Used in reader code.


	struct sembuf pop, vop ;
	semctl(semwrt, 0, SETVAL, 1);		//Initializing the semaphores.
	semctl(semmutex, 0, SETVAL, 1);
	//semctl(semdummy,0,SETVAL,1);
	//P(semdummy);V(semdummy);

	pop.sem_num = vop.sem_num = 0;
	pop.sem_flg = vop.sem_flg = 0;
	pop.sem_op = -1 ; vop.sem_op = 1 ;
	//printf("%d",no_rec);

	while(1)
	{
		sleep(5);			//X sleeps for 5 seconds
		if((*cc)==0)
		{	printf("No changes\n");continue;}		//No changes made in shared variables

		
		P(semmutex);		//Entry Section for Reader Code
			(*rc)=(*rc)+1;
			if((*rc)==1)
				P(semwrt);

		V(semmutex);

		f=fopen(argv[1],"r+");			//Critical Section
		int a=0;
		for(int i=0;i<no_rec-1;i++)
		{
			a+=fprintf(f,"%-20s%-20s%-4d%-4f\n",arr[i].fname,arr[i].lname,arr[i].roll,arr[i].cg);
			//printf("%-20s%-20s%-4d%-4f\n",arr[i].fname,arr[i].lname,arr[i].roll,arr[i].cg);
		}
		//printf("%d",a);
		printf("Changes made in file\n");
		*cc=0;
		fclose(f);


		P(semmutex);				//Exit Section
			(*rc)=(*rc)-1;
			if((*rc)==0)
				V(semwrt);

		V(semmutex);





	}










	return 0;
}