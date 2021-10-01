#include<ulib.h>


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

    // Array to store read and write fds of the pipe.
    int fd[2];

    // Create the pipe.
    int ret_code = pipe (fd);
    if (ret_code < 0) {
        printf ("Pipe op is failed!!!\n");
        return -1;
    }
    // Expected return will be 0.
    printf ("%d\n", ret_code);

    // Expected values for these fds will be 3 and 4.
    printf ("%d\n", fd[0]);
    printf ("%d\n", fd[1]);
    
    // Write to the pipe.
    char write_buff[6] = "hello";
    ret_code = write (fd[1], write_buff, 5);
    if (ret_code < 0) {
        printf ("Pipe write is failed!!!\n");
        return -1;
    } else {
        // Expected result should be 5.
        printf ("%d\n", ret_code);
    }

    // Read from the pipe.
    char read_buff[6];
    ret_code = read (fd[0], read_buff, 5);
    read_buff[5] = '\0';
    if (ret_code < 0) {
        printf ("Pipe read is failed!!!\n");
        return -1;
    } else {
        // Expected result should be 5 and buffer would be "hello".
        printf ("%d\n", ret_code);
        printf ("%s\n", read_buff);
    }
	ret_code = write (fd[1], write_buff, 4);
    if (ret_code < 0) {
        printf ("Pipe write is failed!!!\n");
        return -1;
    } else {
        // Expected result should be 5.
        printf ("%d\n", ret_code);
    }
	ret_code = write (fd[1], write_buff, 4);
        printf ("%d\n", ret_code);


	ret_code = read (fd[0], read_buff, 3);
    read_buff[3] = '\0';
    if (ret_code < 0) {
        printf ("[2]Pipe read is failed!!!\n");
        return -1;
    } else {
        // Expected result should be 5 and buffer would be "hello".
	printf("======================\n\n");
        printf ("%d\n", ret_code);
        printf ("[3]%s\n", read_buff);
	ret_code = write (fd[1], write_buff, 6);
	    ret_code = read (fd[0], read_buff, 5);
        printf ("[4]%s\n", read_buff);
	ret_code = write (fd[1], write_buff, 2);
	ret_code = write (fd[1], write_buff, 2);
	ret_code = write (fd[1], write_buff, 2);
	    ret_code = read (fd[0], read_buff, 5);
        printf ("[5]%s\n", read_buff);
	    ret_code = read (fd[0], read_buff, 5);
	    printf("[6][0]ret code : %d " , ret_code);
	printf("\n\n==========================");
    }


// Close the read end and try to read from the pipe.
    ret_code = close (fd[0]);
    if (ret_code < 0) {
        printf ("Close on pipe read end is failed!!!\n");
        return -1;
    }
    // Expected result should be 0.

    ret_code = read (fd[0], &main, 5);
    if (ret_code < 0) {
        // Expected result should be -1 (-EINVAL).
        printf ("%d\n", ret_code);
    } else {
        printf ("Read end is not closed somehow!!!\n");
        return -1;
    }

    int fd2[2];
    int ret_code2 = pipe(fd2);
    if(ret_code2<0)
    {
        printf("PIPE op2 is failed");
        return -1;
    }
    printf("RET CODE 2 = %d\n",ret_code2);
    printf ("%d\n", fd2[0]);
    printf ("%d\n", fd2[1]);
    
    // Close the write end and try to write to the pipe.
    ret_code = close (fd[1]);
    if (ret_code < 0) {
        printf ("Close on pipe write end is failed!!!\n");
        return -1;
    }
    // Expected result should be 0.
    printf ("%d\n", ret_code);

    char write_buff2[6] = "shubh";
    ret_code2 = write (fd2[1], write_buff2, 5);
    if (ret_code2 < 0) {
        printf ("Pipe write is failed!!!\n");
        return -1;
    } else {
        // Expected result should be 5.
        printf ("WRITE %d\n", ret_code);
    }

    char read_buff2[15];
    ret_code2 = read (fd2[0], read_buff2, 10);
    read_buff2[5] = '\0';
    if (ret_code < 0) {
        printf ("Pipe read is failed!!!\n");
        return -1;
    } else {
        // Expected result should be 5 and buffer would be "hello".
        printf ("%d\n", ret_code2);
        printf ("READ BUFF 2%s\n", read_buff2);
    }

    ret_code2 = write (fd2[1], write_buff2, 5);
    if (ret_code2 < 0) {
        printf ("Pipe write is failed!!!\n");
        return -1;
    } else {
        // Expected result should be 5.
        printf ("WRITE %d\n", ret_code);
    }

    ret_code2 = read (fd2[0], read_buff2, 2);
    read_buff2[2] = '\0';
    if (ret_code < 0) {
        printf ("Pipe read is failed!!!\n");
        return -1;
    } else {
        // Expected result should be 5 and buffer would be "hello".
        printf ("%d\n", ret_code2);
        printf ("READ BUFF 2%s\n", read_buff2);
    }



    ret_code = write (fd[1], read_buff, 5);
    if (ret_code < 0) {
        // Expected result should be -1 (-EINVAL).
        printf ("%d\n", ret_code);
    } else {
        printf ("Write end is not closed somehow!!!\n");
        return -1;
    }

    // Finally simple return.
    return 0;

}