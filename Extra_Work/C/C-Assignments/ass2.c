#include<stdio.h>
#include<stdlib.h>

void IncByOne(void);



static int global_count;

void main(void)
{
printf("Inside main value: %d \n",global_count);

IncByOne();


IncByOne();

}

void IncByOne()
{
	global_count++;
	printf("increment  by one..: %d \n",global_count);
}

