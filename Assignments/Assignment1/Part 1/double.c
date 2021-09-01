//Headers
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<string.h>

//to check if the parameter is an integer number
int isNumber(char number[])
{
	int n=strlen(number);
    for(int i=0;i<n;i++){
        if(number[i]<'0'||number[i]>'9') return 0;
    }
    return 1;
}

//main method
int main(int argc,char* argv[]){

	//not enough arguments
	if(argc<2){
		printf("UNABLE TO EXECUTE\n");
		exit(0);
	}

	//to check if the last parameter is a integer number.
	if(!isNumber(argv[argc-1])){
		printf("UNABLE TO EXECUTE\n");
		exit(0);
	}

	long long val=atoll(argv[argc-1]);

	//to check if value is negative
	if(val<0){
		printf("UNABLE TO EXECUTE\n");
		exit(0);
	}
	//convert last element of argv[] to unsigned long long and multiply by 2
	unsigned long long x=2*atoll(argv[argc-1]);
	
	//if we have exactly 2 arguments, then just print x	
	if(argc==2) printf("%llu\n",x);	

	else{

		//if the current argument is not double root or square, we have an error condition
		if(strcmp(argv[1],"root")!=0&&strcmp(argv[1],"square")!=0&&strcmp(argv[1],"double")!=0){
			printf("UNABLE TO EXECUTE\n");
			exit(0);
		}

		//creating a new array, in which we will store the remaining arguments, which are to be passed along with exec
		char** tobePassed=malloc((argc)*sizeof(char*));

		//to store the result for the current computation
		char* temp=(char*)malloc(25*sizeof(char));

		//write the result to temp
		sprintf(temp,"%llu",x);

		//copy the arguments
		for(int i=0;i<argc-2;i++){
			tobePassed[i]=argv[i+1];
		}

		tobePassed[argc-1]=NULL;
		
		//copy the current result
		tobePassed[argc-2]=temp;

		//invoke exec
		if(execv(argv[1],tobePassed)){
			printf("UNABLE TO EXECUTE\n");
			exit(0);
		}

		free(tobePassed);
		free(temp);
	}
	return 0;

}
