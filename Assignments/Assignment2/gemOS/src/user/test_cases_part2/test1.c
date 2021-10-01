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

    //fork
    int pid = fork();
    if (pid < 0) {
        printf ("Somehow not able ro create process!!!\n");
        exit (-1);
    }
    if(!pid){
        //write 2096 bytes
        ret_code=write(fd[1],"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",2096);
        if (ret_code < 0) {
            printf("Write to persistent pipe failed!!!\n");
            return -1;
        }
        printf ("%d\n", ret_code);

        sleep(30);
        //read 5000 bytes
        char read_buffer2[5001];
        ret_code=read(fd[0],read_buffer2,5000);
        if (ret_code < 0) {
            printf("Read to persistent pipe failed!!!\n");
            return -1;
        }
        read_buffer2[ret_code]='\0';
        printf("%d\n",ret_code);
        exit(1);
    }
    sleep(30);
    //write 2000 bytes
    ret_code=write(fd[1],"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",2000);
    if (ret_code < 0) {
        printf("Write to persistent pipe failed!!!\n");
        return -1;
    }
	printf ("%d\n", ret_code);

    //read 5000 bytes
    char read_buffer[5001];
    ret_code=read(fd[0],read_buffer,5000);
    if (ret_code < 0) {
        printf("Read to persistent pipe failed!!!\n");
        return -1;
    }
	read_buffer[ret_code] = '\0';
    printf ("%d\n", ret_code);
	return 0;

}
