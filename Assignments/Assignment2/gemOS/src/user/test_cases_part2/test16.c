#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {
    // Array to store read and write fds of the persistent pipe.
	int fd[2];
	// Create the persistent pipe.
	int ret_code = ppipe (fd);
	if (ret_code < 0) {
		printf ("Pipe op is failed!!!\n");
		return -1;
	}
    // Expected return will be 0.
    printf ("%d\n", ret_code);

    //write 4096 bytes
	ret_code = write (fd[1],"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", 4096);
    if (ret_code < 0) {
        printf("Write to persistent pipe failed!!!\n");
        return -1;
    }
    // Expected return will be 5.
	printf ("%d\n", ret_code);

    //read 4096 bytes
    char read_buffer[4097];
	ret_code = read (fd[0], read_buffer, 4096);
    if (ret_code < 0) {
        printf("Read to persistent pipe failed!!!\n");
        return -1;
    }
    // Expected return will be 5 and buffer will be "hello"    
	read_buffer[4096] = '\0';
    printf ("%d\n", ret_code);

    int pid = fork();
    if (pid < 0) {
        printf ("Somehow not able ro create process!!!\n");
        exit (-1);
    }

    if(!pid){
        char read_buffer2[201];
        ret_code = read (fd[0], read_buffer2, 200);
        if (ret_code < 0) {
            printf("Read to persistent pipe failed!!!\n");
            return -1;
        }
        // Expected return will be 5 and buffer will be "hello"    
        read_buffer[200] = '\0';
        printf ("%d\n", ret_code);
        exit(1);
    }
    sleep(30);
	// Finally simple return.
	return 0;

}
