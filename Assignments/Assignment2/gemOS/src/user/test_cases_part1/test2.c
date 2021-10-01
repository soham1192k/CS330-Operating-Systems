#include<ulib.h>


int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

    u32 pid;
    int fd[2];

    if (pipe(fd) < 0) {
        printf("Create pipe failed!");
        return 0;
    }

    pid = fork();

    if (pid == 0) {
        sleep(5);
	char buf[4096];	
        
        printf("R by child: %d\n", read(fd[0], buf, 4096));
    }
    else {
        char buf[4096];

        for (int i = 0; i < 4096; i++) {
            buf[i] = '0';
        }

        printf("W by parent: %d\n", write(fd[1], buf, 4096));
        printf("R by parent: %d\n", read(fd[0], buf, 4096));
    }

    return 0;
}


