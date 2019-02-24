#include<stdio.h>
#include<stdlib.h>

#define num_rows 3
#define num_col 3

int main(void)
{
        
        int **ptr=NULL;
      
       ptr=(int **)malloc(num_rows*sizeof(int*));
       
       for(int i=0;i<num_col;i++)
       {        
                ptr[i]=(int *)malloc(num_col*sizeof(int));
                if(ptr[i]==NULL)
                {
                        printf("malloc problem");
                        exit(0);
                }
       }
       
       
       
       
       //accept &display
       
       for(int i=0;i<num_rows;i++)
       {
                for(int j=0;j<num_col;j++)
                {
                        printf("Enter number [%d] [%d]:",i,j);
                        scanf("%d",(ptr[i]+j));        
                }
       
       }
       
       for(int i=0;i<num_rows;i++)
       {
                for(int j=0;j<num_col;j++)
                {
                        printf("%d ",*(ptr[i]+j));
                       
                }
                printf("\n");
       
       }
       
 return(0);       
}
