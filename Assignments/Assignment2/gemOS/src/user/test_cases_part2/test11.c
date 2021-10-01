#include<ulib.h>
int main(u64 arg1,u64 arg2,u64 arg3,u64 arg4,u64 arg5){
    int fd[2];
    int p=pipe(fd);
    int cl=close(fd[0]);
    int pid=fork();
    if(!pid){
        printf("I AM CHILD\n");
        int c2=close(fd[0]);
        int c3=close(fd[1]);
        printf("c2=%d c3=%d\n",c2,c3);
        exit(1);
    }
    else{
        printf("I AM PARENT\n");
        int c4=close(fd[0]);
        printf("c4=%d\n",c4);
        int c5=close(fd[1]);
        printf("%d\n",c5);
    }
    return 0;
}