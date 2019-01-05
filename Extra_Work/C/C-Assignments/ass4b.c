#include<stdio.h>

void main()
{
	
	int num1,num2;
	
	printf("Enter two numbers");
	scanf("%d %d",&num1,&num2);
	
	printf(" %d & %d  =%d \n",num1,num2,num1 & num2);
	
	printf(" %d && %d  =%d \n",num1,num2,num1 && num2);
	
	printf(" %d |  %d =%d \n",num1,num2,num1|num2);
	
	printf(" %d || %d =%d \n",num1,num2,num1||num2);
	
	printf(" !%d   =%d \n",num1,!num1);
	
	printf(" !%d   =%d \n",num2,!num2);
	
	
	printf(" ~%d   =%d \n",num1,~num1);
	
	printf(" ~%d   =%d \n",num2,~num2);
	
	printf(" %d ^ %d =%d \n",num1,num2,num1^num2);
	
	printf(" <<2 of %d   =%d \n",num1,num1<<2);
	
	printf(" <<2 of %d   =%d \n",num2,num2<<2);
}
/*
	   (a&b)
	 (a&&b)
	 (a|b)
	 (a||b)
	 !a , !b
	 ~a,~b
	 (a^b)	
	 a<<2,b<<1
	 a>>2,b>>1
	 */
