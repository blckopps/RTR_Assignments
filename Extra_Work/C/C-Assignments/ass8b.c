#include<stdio.h>

void main(void)
{

	int num1,num2;
		printf("enter  first number:");
		scanf("%d",&num1);	
		
		printf("enter  second number:");
		scanf("%d",&num2);	
		
	int z=(num1>num2)?num1:num2;	
	printf("Greater number is = %d\n",z);
	
}
