#include<stdio.h>

void main(void)
{
int num,fact=1;;

printf("Enter number");
scanf("%d",&num);


for(int i=1;i<=num;i++)
{
	fact=fact*i;
}
printf("FACTORIAL OF %d is: %d\n",num,fact);
}
