#include<ulib.h>


int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

    // Array to store read and write fds of the persistent pipe.    
	int fd[2];
	
	// Create persistent pipe.
	int ret_code = ppipe(fd);
	
	// Check for any error in pipe creation.
	if (ret_code < 0) {

		printf("Persistent pipe allocation failed!!!\n");
		return -1;
	
	}
    // Expected return will be 0.
    printf ("%d\n", ret_code);
	
    // Write to the ppipe.
	char buffer[6] = "hello"; 
	ret_code = write (fd[1], buffer, 5);
    if (ret_code < 0) {

        printf("Write to persistent pipe failed!!!\n");
        return -1;

    }
    // Expected return will be 5.
	printf ("%d\n", ret_code);
	
    // Flush the ppipe at this point.
	ret_code = flush_ppipe (fd);
    if (ret_code < 0) {

        printf("Flush on persistent pipe failed!!!\n");
        return -1;

    }
    // Expected result will be 0.
	printf ("%d\n", ret_code);
	
    // Read from the ppipe.
	char read_buffer[6];
	ret_code = read (fd[0], read_buffer, 5);
    if (ret_code < 0) {

        printf("Read to persistent pipe failed!!!\n");
        return -1;

    }
    // Expected return will be 5 and buffer will be "hello"    
	read_buffer[5] = '\0';
    printf ("%d\n", ret_code);
    printf ("%s\n", read_buffer);

    // Flush the ppipe at this point.
	ret_code = flush_ppipe (fd);
    if (ret_code < 0) {

        printf ("Flush on persistent pipe failed!!!\n");
        return -1;

    }
    // Expected return will be 5.
    printf ("%d\n", ret_code);

    // Close its ends.
    // Expected return will be 0.
    printf ("%d\n", close (fd[0]));
    printf ("%d\n", close (fd[1]));
	
    // Simple return.
	return 0;

}
