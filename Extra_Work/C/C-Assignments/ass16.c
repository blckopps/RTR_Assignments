#include<stdio.h>

void main(int argc,char *argv[],char *envp[])
{
int i=0;

	while((*envp)!='\0')
	{
		printf("%s\n",*envp);
		envp++;
	}	
	
	
	
}
