#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int lsearch(int arr[],int lo,int hi,int k)
{
	if(hi-lo+1<=10)
	{
		for(int i=lo;i<=hi;i++)
			if(arr[i]==k)
				return 256;
		return 0;
	}
	int mid=(hi+lo)/2;
	int id[2],status[2];
	for(int i=0;i<2;i++)
	{
		id[i]=fork();
		int a;
		if(id[i]==0 && i==0)
		{
			a=lsearch(arr,lo,mid,k);
			if(a==256)
				exit(1);
			else exit(0);
		}
		if(id[i]==0 && i==1)
		{
			a=lsearch(arr,mid+1,hi,k);
			if(a==256)
				exit(1);
			else exit(0);
		}

	}
	waitpid(id[0],&status[0],0);
	waitpid(id[1],&status[1],0);
	if(status[0]==256 || status[1]==256)
		return 256;
	else return 0;
}


int main()
{	printf("Enter the filename \n");
	char filename[100];
	scanf("%s",filename);
	FILE *filep=fopen(filename,"r");
	int arr[1000];int i=0;
	
	while(!feof(filep))
		fscanf(filep,"%d",&arr[i++]);
	int k;

	while(1)
	{
		printf("Enter the value of k:  ");
		scanf("%d",&k);
		if(k<0)
			break;
		int a=lsearch(arr,0,i-1,k);
		if(a==256)
			printf("Element found\n" );
		else printf("Element not found\n");

	}


}