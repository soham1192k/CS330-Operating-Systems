#include<ulib.h>


int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {
	
    // Array to store both read and write fds of the persistent pipe.
	int fd[2];
	
	// Create persistent pipe.
	int ret_code = ppipe(fd);
	
	// Check for any error in ppipe creation.
	if (ret_code < 0) {

		printf("Pipe allocation failed!!!\n");
		return -1;
	
	}
    // Expected result is 0.
    printf ("%d\n", ret_code);

    // Create child.	
	int pid = fork();
    if (pid < 0) {
        printf ("Somehow not able ro create process!!!\n");
        exit (-1);
    }

	if (!pid) {               // Child code.
		
		char *read_buffer[6];

        // Expected result for the fds will be 3 and 4.
		printf ("%d\n", fd[0]);
        printf ("%d\n", fd[1]);
		sleep (5);

        // Read from the ppipe.
		ret_code = read (fd[0], read_buffer, 5);
		if (ret_code < 0) {
		
			printf ("Child: Reading1 from the pipe is failed!!!\n");
			return -1;

		}
        // Expected return will be 5 and buffer will be "hello".
        printf ("%d\n", ret_code);
		printf ("%s\n", read_buffer);
		
		ret_code = write (fd[1], "abcdefg", 7);
		if (ret_code < 0) {
		
			printf ("Child: Write to the pipe is failed!!!\n");
			return -1;

		}
        // Expected return will be 7.
        printf ("%d\n", ret_code);
		sleep (150);
        // Close both read and write ends.
        // Expected results should be 0.
        char *rea_buffer[25];
        ret_code = read (fd[0], rea_buffer, 100);
		if (ret_code < 0) {
		
			printf ("Child: Reading3 from the pipe is failed!!!\n");
			return -1;

		}
		rea_buffer[24] = '\0';
        // Expected return will be 5 and buffer will be "hello".
        printf ("%d\n", ret_code);
		printf ("%s\n", rea_buffer);
        
        
		printf ("%d\n", close (fd[0]));
        printf ("%d\n", close (fd[1]));
        
        // Exit.
		exit (1);

	}

    // Write to the ppipe.
	ret_code = write (fd[1], "helloindiaisgreat", 17);
	if (ret_code < 0) {
		
		printf ("Parent: Writing1 to the pipe is failed!!!\n");
		return -1;

	}
    // Expected return should be 17.
    printf ("%d\n", ret_code);
    
    // This is for order between parent and child.
	sleep (50);

    // Expected value of fds in parent will be 3 and 4.
    printf ("%d\n", fd[0]);
    printf ("%d\n", fd[1]);
	
	// Even if child has closed its end, parent can still write and read from
    // the ppipe.
	ret_code = write (fd[1], "world", 5);
	if (ret_code < 0) {
		
		printf ("Parent: Writing again to the pipe is failed!!!\n");
		return -1;

	}
    // Expected return will be 5.
    printf ("%d\n", ret_code);

	char *read_buffer[18];
	ret_code = read (fd[0], read_buffer, 17);
	if (ret_code < 0) {
		
		printf ("Parent: Reading from the pipe is failed!!!\n");
		return -1;

	}
    // Expected return will be 5 and buffer will be "hello".
    read_buffer[17] = '\0';
    printf ("%d\n", ret_code);
	printf ("%s\n", read_buffer);

    // Close ppipe ends.
    // expected return will be 0.
    printf ("%d\n", close (fd[0]));
    printf ("%d\n", close (fd[1]));

    // Simple return.
	return 0;

}