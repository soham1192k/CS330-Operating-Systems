#include<ulib.h>
/*
Test 8:
    Closing read end before forking should affect both parent
    and child; None of them should be able to read and should return
    -EACCESS = -1
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
    close(fd[0]);
    pid = fork();
    if (pid == 0) {
        // Child
        sleep(5);
        char buf[50];
        printf("DBG: Child Pid After Fork: %d\n", getpid());
        printf("C read: %d\n", read(fd[0], buf, 5));
    }
    else {
        // Parent
	    char buf1[50];
     
        printf("DBG: Parent Pid After Fork: %d\n", getpid());
        printf("P read: %d\n", read(fd[0], buf1, 5));
    }

    return 0;
}


