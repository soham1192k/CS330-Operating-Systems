#include<ulib.h>
/*
Test 11:
    Checking whether subsequent closing of fds give errors
*/

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

    u32 pid;
    int fd[2];

    if (pipe(fd) < 0) {
        printf("Create pipe failed!");
        return 0;
    }
    char *write_buf = "loremipsumdolrsitamet";
    char read_buf[50];
    printf("W: %d\n", write(fd[1], write_buf, 20));
    printf("R: %d\n", read(fd[0], read_buf, 5));

    u32 ret = close(fd[0]);
    printf("Return Code for Read Close 1: %d\n", ret);
    printf("R: %d\n", read(fd[0], read_buf+5, 5));
    ret = close(fd[0]);
    printf("Return Code for Read Close 2: %d\n", ret);
    printf("R: %d\n", read(fd[0], read_buf+10, 5));
    
    return 0;
}


