#include<stdio.h>

void main(void)
{
/*
	int num,count,sum=0;
	printf("how many numbers you want to enter");
	scanf("%d",&count);
	
	for(int i=1;i<=count;i++)
	{
		printf("enter %d number\n",i);
		scanf("%d",&num);	
		sum=sum+num;
	}
	
	printf("average:%d\n",sum/count);
	*/
	///floating numbers
	float num,sum=0;
	int count;
	printf("how many numbers you want to enter");
	scanf("%d",&count);
	
	for(int i=1;i<=count;i++)
	{
		printf("enter %d number\n",i);
		scanf("%f",&num);	
		sum=sum+num;
	}
	
	printf("average:%f\n",sum/count);
		

}
