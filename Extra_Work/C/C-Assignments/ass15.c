#include<stdio.h>
void main()
{


	printf("--------------------------------------------------------------------------------------------------------------\n");
	printf("Data Type\t\tSize\t\tFormat Specifier\t\tRnage\n");
		printf("---------------------------------------------------------------------------------------------------------------\n");
		//Int
	int size_int = 8 * sizeof(int);
	char d=NULL;
	int from_int = -(1<<(size_int-1));
	int to_int = (1 << (size_int-1)) - 1;
	printf("Int \t\t\t %d \t\t\t  %c \t\t\t %d to %d\n",sizeof(int),d,from_int,to_int);

	//char
	int size_char = 8 * sizeof(char);
	char C=NULL;
	int from_char = -(1 << (size_char - 1));
	int to_char = (1 <<(size_char-1)) - 1;
	printf("char \t\t\t %d \t\t\t %c \t\t\t %d to %d\n", sizeof(char), C, from_char,to_char);

}