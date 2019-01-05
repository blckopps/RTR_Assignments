#include<stdio.h>

void main(void)
{
int num,fact=1;;

printf("Enter number");
scanf("%d",&num);
int num2=num;

while(num!=0)
{
	fact=fact*num;
	num--;
}
printf("FACTORIAL OF NUM %d is=%d",num2,fact);

}
