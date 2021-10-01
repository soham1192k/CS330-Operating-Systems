#include<ulib.h>
/*
Test 9:
    Closing read end after forking for parent only; 
    Parent shouldn't be able to read, 
    while child will be able to read

    Later read end is also closed for child;
    Then, child should also return -EACCESS while trying to read
*/

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

    u32 pid;
    int fd[2];

    if (pipe(fd) < 0) {
        printf("Create pipe failed!");
        return 0;
    }
    printf("DBG: Current Parent Pid: %d\n", getpid());
    char *buffer = "loremipsumdolrsitamet";
    printf("P write (before fork): %d\n", write(fd[1], buffer, 20));
    pid = fork();
    if (pid == 0) {
        // Child
        sleep(5);
        char buf[50];
        printf("C read: %d\n", read(fd[0], buf, 5));
        close(fd[0]);
        printf("C read: %d\n", read(fd[0], buf, 5));
    }
    else {
        // Parent
	    char buf1[50];
        close(fd[0]);
        printf("P read: %d\n", read(fd[0], buf1, 5));
    }

    return 0;
}


