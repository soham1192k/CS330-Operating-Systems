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
		printf ("Pipe write is failed with code %d !!!\n", ret_code);
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
		printf ("Pipe read is failed with code %d !!!\n", ret_code);
		return -1;
	} else {
        // Expected result should be 5 and buffer would be "hello".
		printf ("%d\n", ret_code);
        printf ("%s\n", read_buff);
	}

	// Close the read end and try to read from the pipe.
	ret_code = close (fd[0]);
	if (ret_code < 0) {
		printf ("Close on pipe read end is failed with code %d!!!\n", ret_code);
		return -1;
	}
    // Expected result should be 0.
    printf ("%d\n", ret_code);

	ret_code = read (fd[0], &main, 5);
	if (ret_code < 0) {
        // Expected result should be -1 (-EINVAL).
		printf ("%d\n", ret_code);
	} else {
		printf ("Read end is not closed somehow with ret code %d!!!\n", ret_code);
		return -1;
	}

	// Close the write end and try to write to the pipe.
	ret_code = close (fd[1]);
	if (ret_code < 0) {
		printf ("Close on pipe write end is failed with ret code %d!!!\n", ret_code);
		return -1;
	}
    // Expected result should be 0.
    printf ("%d\n", ret_code);

	ret_code = write (fd[1], read_buff, 5);
	if (ret_code < 0) {
        // Expected result should be -1 (-EINVAL).
		printf ("%d\n", ret_code);
	} else {
		printf ("Write end is not closed somehow with ret code %d!!!\n", ret_code);
		return -1;
	}

	// Finally simple return.
	return 0;

}
