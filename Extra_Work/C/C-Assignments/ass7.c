#include<stdio.h>

void main()
{

int num;
printf("Enter number:");
scanf("%d",&num);

for(int i=1;i<=10;i++)
{
	printf("%d * %d =%d\n",i,num,i*num);
}

}
