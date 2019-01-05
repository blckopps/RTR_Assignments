#include<stdio.h>


enum days
{
	SUNDAY=0,MONDAY,TUESDAY,WEDNESDAY,THURSDAY,FRIDAY,SATURDAY
	
};

void main(void)
{
	int day;
	printf("Enter number from 0 to 6:");
	scanf("%d",&day);
	
	switch(day)
	{
	case 1:
		printf("\n");
	break;
	
	case 2:
		printf("TODAY is MONDAY\n");
	break;
	
	case 3:
		printf("TODAY is TUESDAY\n");
	break;
	
	case 4:
		printf("TODAY is WEDNESDAY\n");
	break;
	
	case 5:
		printf("TODAY is THURSDAY\n");
	break;
	
	case 6:
		printf("TODAY is FRIDAY\n");
	break;
	
	
	case 0:
		printf("TODAY is SATURDAY\n");
	break;
	}
	
}
