#include<stdio.h>
#include<stdlib.h>

void merge(int arr[],int a,int n ,int b,int m)
{
	int c[1000];
	int k=0,i=0,j=0;
	while(i<n && j<m)
	{
		if(arr[a+i]>arr[b+j])
			c[k++]=arr[b+j++];
		else c[k++]=arr[a+i++];
	}
	if(i<n)
	{
		while(i<n)
			c[k++]=arr[a+i++];
	}
	if(j<m)
	{
		while(j<m)
			c[k++]=arr[b+j++];
	}
	for(int x=0;x<k;x++)
	{
		arr[a++]=c[x];
	}
}

void mergesort(int arr[],int lo,int hi)
{
	if(lo==hi)
		return;
	int mid=(lo+hi)/2;
	mergesort(arr,lo,mid);
	mergesort(arr,mid+1,hi);
	merge(arr,lo,mid-lo+1,mid+1,hi-mid);
}

int main(int argc, char *argv[])
{
	FILE *filep=fopen(argv[1],"r");
	if(!filep)
		{perror("File open failed: ");
			return 0;
		}

	int arr[1000];int i=0;
	
	while(!feof(filep))
		fscanf(filep,"%d",&arr[i++]);

	mergesort(arr,0,i-1);
	printf("Array in sorted order is: \n");
	for(int j=0;j<i;j++)
		printf("%d\n",arr[j]);


	return 0;

}
