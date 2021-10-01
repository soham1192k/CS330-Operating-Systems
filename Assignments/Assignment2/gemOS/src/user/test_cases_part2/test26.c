#include<ulib.h>


int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {
	
    // Array to store both read and write fds of the pipe.
	int fd[2];
	
	// Create pipe.
	int ret_code = ppipe(fd);
	
	// Check for any error in pipe creation.
	if (ret_code < 0) {

		printf("Pipe allocation failed!!!\n");
		return -1;
	
	}
    // Expected result is 0.
    printf ("I : %d\n", ret_code);

    // writing 1000
    char buff[1000];
    for(int i = 0;i<1000;i++){
        buff[i] = 'a';
    }

    ret_code = write (fd[1], buff, 1000);
	if (ret_code < 0) {
		
		printf ("Parent: Writing to the pipe is failed!!!\n");
		return -1;

	}
    // Expected return should be 1000.
    printf ("I: %d\n", ret_code);
    // Create child.	
	int pid = fork();
    if (pid < 0) {
        printf ("Somehow not able ro create process!!!\n");
        exit (-1);
    }

	if (!pid) {               // Child code.
		
		char *read_buffer[501];

        // Read from the pipe.
		ret_code = read (fd[0], read_buffer, 500);
		if (ret_code < 0) {
		
			printf ("Child: Reading from the pipe is failed!!!\n");
			return -1;

		}
        // Expected return will be 500 and buffer will be "hello".
        printf ("J : %d\n", ret_code);
        read_buffer[500] = '\0';
		printf ("J : %s\n", read_buffer);

        // Close both read and write ends.
        // Expected results should be 0
		
		ret_code = flush_ppipe(fd);
		if (ret_code < 0) {
		
			printf ("Child: Reading from the pipe is failed!!!\n");
			return -1;

		}
        // Expected return will be 0;
        printf ("J : %d\n", ret_code);
		
		sleep(100);
        
		ret_code = flush_ppipe(fd);
		if (ret_code < 0) {
		
			printf ("Child: Reading from the pipe is failed!!!\n");
			return -1;

		}
        // Expected return will be 0;
        printf ("J : %d\n", ret_code);

        // ret_code = read (fd[0], read_buffer, 500);
		// if (ret_code < 0) {
		
		// 	printf ("Child: Reading from the pipe is failed!!!\n");
		// 	return -1;

		// }
        // // Expected return will be 0 .

        // printf ("%d\n", ret_code);

        int pid2 = fork();

        if (pid2 < 0) {
            printf ("Somehow not able to create process!!!\n");
            exit (-1);
        }
        sleep(200);

        if(!pid2){ //child
			
            char *read_buffer_2[1001];

            // Read from the pipe.
            ret_code = read (fd[0], read_buffer_2,1000);
            if (ret_code < 0) {
            
                printf ("Child: Reading from the pipe is failed!!!\n");
                return -1;

            }
            // Expected return will be 5 and buffer will be "hello".
            printf ("K : %d\n", ret_code);
            read_buffer_2[1000] = '\0';
            printf ("K : %s\n", read_buffer_2);

			ret_code = flush_ppipe(fd);
			if (ret_code < 0) {
			
				printf ("Child: Reading from the pipe is failed!!!\n");
				return -1;

			}
			// Expected return will be 0;
			printf ("K : %d\n", ret_code);
			printf ("K: %d\n", close (fd[0]));
            exit(1);
        }
        // Exit.    
		sleep(300);
		char *read_buffer_2[5000];

        // Read from the pipe.
		ret_code = read (fd[0], read_buffer_2, 5000);
		if (ret_code < 0) {
		
			printf ("Child: Reading from the pipe is failed!!!\n");
			return -1;

		}
        // Expected return will be 500 and buffer will be "hello".
        printf ("J : %d\n", ret_code);

		ret_code = flush_ppipe(fd);
		if (ret_code < 0) {
		
			printf ("Child: Reading from the pipe is failed!!!\n");
			return -1;

		}
		// Expected return will be 0;
		printf ("J Flushing: %d\n", ret_code);
		exit (1);

	}
    sleep(50);
    // Write to the pipe.
    printf ("I: %d\n", close (fd[0]));
	sleep(150);

	char buff_3[3596];
    for(int i = 0;i<3596;i++){
        buff_3[i] = 'c';
    }

    ret_code = write (fd[1], buff_3, 3596);
	if (ret_code < 0) {
		
		printf ("Parent: Writing to the pipe is failed!!!\n");
		return -1;

	}
    // Expected return should be 3096.
    printf ("I : %d\n", ret_code);
    // Close pipe ends.
    // expected return will be 0.
	sleep(1200);


    ret_code = write (fd[1], buff, 10);
	if (ret_code < 0) {
		
		printf ("Parent: Writing to the pipe is failed!!!\n");
		return -1;

	}
    // Expected return should be 3096.
    printf ("I: %d\n", ret_code);

    printf ("I : %d\n", close (fd[0]));
    printf ("I : %d\n", close (fd[1]));

    // Simple return.
	return 0;

}