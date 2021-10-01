#include<ulib.h>


int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

    u32 pid;
    char buf1[4096];
    char buf2[4096];
    int fd[2];

    if (pipe(fd) < 0) {
        printf("Create pipe failed!");
        return 0;
    }

    for (int i = 0; i < 4096; i++) {
        buf1[i] = '0';
    }

    // Testing wrap-around functionality
    printf("W: %d\n", write(fd[1], buf1, 3000));
    printf("R: %d\n", read(fd[0], buf2, 4096));
    printf("R: %d\n", read(fd[0], buf2, 4096));
    printf("W: %d\n", write(fd[1], buf1, 2000));
    printf("R: %d\n", read(fd[0], buf2, 4096));
    printf("W: %d\n", write(fd[1], buf1, 4096));
    printf("R: %d\n", read(fd[0], buf2, 4096));

    // Testing illegal read-write access
    printf("W: %d\n", write(fd[0], buf1, 100));
    printf("R: %d\n", read(fd[1], buf2, 100));

    return 0;
}

