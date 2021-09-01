#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<string.h>

int isNumber(char number[])
{
	int n=strlen(number);
    for(int i=0;i<n;i++){
        if(number[i]<'0'||number[i]>'9') return 0;
    }
    return 1;
}

int main(int argc, char* argv[]){

	if(argc<2){
		printf("UNABLE TO EXECUTE\n");
		exit(0);
	}

	if(!isNumber(argv[argc-1])){
		printf("UNABLE TO EXECUTE\n");
		exit(0);
	}

	double val=atof(argv[argc-1]);
	
	if(val<0){
		printf("UNABLE TO EXECUTE\n");
		exit(0);
	}

	//computing the square root and applying the round function on it
	unsigned long long x = (unsigned long long)round(sqrt(atof(argv[argc-1])));
	
	if(argc==2) printf("%llu\n",x);
	
	else{

		if(strcmp(argv[1],"root")!=0&&strcmp(argv[1],"square")!=0&&strcmp(argv[1],"double")!=0){
			printf("UNABLE TO EXECUTE\n");
			exit(0);
		}

		char** tobePassed=malloc((argc)*sizeof(char*));
		
		char* temp=(char*)malloc(25*sizeof(char));
		
		sprintf(temp,"%llu",x);
		
		for(int i=0;i<argc-2;i++){
			tobePassed[i]=argv[i+1];
		}

		tobePassed[argc-1]=NULL;
		tobePassed[argc-2]=temp;

		if(execv(argv[1], tobePassed)){
        	printf("UNABLE TO EXECUTE\n");
			exit(0);
		}

		free(tobePassed);
		free(temp);
	}
	return 0;
}