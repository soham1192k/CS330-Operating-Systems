#include<ulib.h>
/*
Test 10:
    Checking access from wrong ends
    Should return -EACCES = -4
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
    printf("Write from wrong end: %d\n", write(fd[0], write_buf, 20));
    printf("Read from wrong end: %d\n", read(fd[1], read_buf, 20));
    
    return 0;
}


