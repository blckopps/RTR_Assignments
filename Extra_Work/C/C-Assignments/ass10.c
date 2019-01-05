#include<stdio.h>

void main(void)
{
	int x,y;
	printf("Enter X and  Y co-ordinates:");
	scanf("%d %d",&x,&y);
	
	
	if(x>=0 && y>=0)
	{
		printf("X and Y are in First Quadrant\n");
	}
	else if(x<0 && y>=0)
	{
		printf("X and Y are in Second Quadrant\n");
	}
	else if(x<0 && y<0)
	{
			printf("X and Y are in Third Quadrant\n");
	}
	else
	{
		printf("X and Y are in Fourth Quadrant\n ");
	}
	
}
