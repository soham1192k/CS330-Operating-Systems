#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<wait.h>
int main(){
    int* pid;
    printf("We are at %p\n",sbrk(0));
    pid=(int*)sbrk(4096);
    *pid=fork();
    printf("Pid=%d\n",*pid);
    if(!pid){
        printf("Child:heap start after expand=%p\n",sbrk(-4096));
        printf("Child:pid=%d\n",*pid);
        exit(0);
    }
    else{
        wait(NULL);
        printf("Parent pid=%d\n",*pid);
    }
}