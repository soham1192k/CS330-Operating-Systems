#include<stdio.h>
#include<stdlib.h>
int main(void)
{
    FILE *fpt = fopen("file.c", "r");
    if(!fpt){
	    fprintf(stderr, "could not open file\n");
	    exit(-1); 
    }   
    while(!feof(fpt)){
      char buff[128];
      if(fread(buff, 128, 1, fpt) <= 0)
        break;
    }
    fclose(fpt);
}
