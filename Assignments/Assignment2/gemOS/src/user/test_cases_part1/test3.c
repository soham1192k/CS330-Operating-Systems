#include<ulib.h>


int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

    u32 pid;
    char buf1[20];
    char buf2[4095];
    int fd[2];

    if (pipe(fd) < 0) {
        printf("Create pipe failed!");
        return 0;
    }

    pid = fork();
    if (pid == 0) {
        char* buf = "Hello, I am fine!";
        close(fd[0]);
        printf("C: %d\n", write(fd[1], buf, 17));
        sleep(10);
        for (int i = 0; i < 4095; i++) {
            buf2[i] = '0';
        }
        printf("C: %d\n", write(fd[1], buf2, 4095));

        printf("C: %d\n", read(fd[0], buf1, 5));
        exit(0);
    }
    else {
        sleep(5);
        close(fd[1]);

        printf("P: %d\n", read(fd[0], buf1, 20));
        sleep(50);
        printf("P: %d\n", read(fd[0], buf2, 4095));

        printf("P: %d\n", write(fd[1], buf1, 10));
        buf1[20] = '\0';
        printf ("P: %s\n", buf1);
        close(fd[0]);
    }

    return 0;
}


