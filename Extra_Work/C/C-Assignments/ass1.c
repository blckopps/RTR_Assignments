#include<stdio.h>
#include<stdlib.h>

void IncByOne(void);
void IncByTwo(void);

int global_var;

void main(void)
{
printf("Inside main value: %d \n",global_var);

IncByOne();
IncByTwo();
IncByTwo();
IncByOne();

}

void IncByOne()
{
	global_var++;
	printf("increment  by one..: %d \n",global_var);
}

void IncByTwo()
{
	global_var++;
	++global_var;
	printf("increment  by Two..: %d \n",global_var);
}
