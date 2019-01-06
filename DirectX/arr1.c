#include<stdio.h>
int main()
{
int i,j,n,*ptr;
printf("Enter 2d arr size: ");
scanf("%d",&n);

int arr[n];
ptr=arr;

for(i=0;i<n;i++)
{
	for(j=0;j<n;j++)
	{
		printf("enter number: ");
		scanf("%d",ptr);
		ptr++;
	}
}
ptr=arr;
printf("Enter arr is:\n");
for(i=0;i<n;i++)
{
	for(j=0;j<n;j++)
	{
		printf("%d",*(ptr));
		ptr++;
	}
	printf("\n");
}





}

