#include<stdio.h>

void main(void)
{
	char ch;
	printf("Enter character:");
	scanf("%c",&ch);
	
	if(ch<=90 && ch>=65)
	{
		printf("%c is UPPERCASE ALPHABET\n",ch);
	}
	else if(ch<=122 && ch>=97)
	{
		printf("%c is LOWERCASE ALPHABET\n",ch);
	}
	else if(ch>=48 && ch<=57)
	{
		printf("%c is DIGIT",ch);
	}
	else if(ch==32)
	{
		printf(" SPACE was pressed\n");
	}
	else if(ch==9)
	{
		printf(" TAB was pressed\n");
	}
	else if(ch==10)
	{
		printf(" NEW LINE\n");
	}
	else
	{
		printf("OTHER OPTION ");
	}
	
}
