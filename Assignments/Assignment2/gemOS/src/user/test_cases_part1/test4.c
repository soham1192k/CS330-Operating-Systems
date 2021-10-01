#include<ulib.h>


int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

    u32 pid;
    int fd[2];

    if (pipe(fd) < 0) {
        printf("Create pipe failed!");
        return 0;
    }

    close(fd[1]);
    pid = fork();
    if (pid == 0) {
        sleep(5);
	char* buf = "Hi!";
        printf("C write: %d\n", write(fd[1], buf, 4));
    }
    else {
        char* buf = "Hi!";       
	char buf1[10];

        printf("P write: %d\n", write(fd[1], buf, 4));
        printf("P read: %d\n", read(fd[0], buf1, 4));
    }

    return 0;
}


