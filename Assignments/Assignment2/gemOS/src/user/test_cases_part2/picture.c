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

    // Create child j	
	int pid = fork();
    if (pid < 0) {
        printf ("Somehow not able ro create process!!!\n");
        exit (-1);
    }
	if (!pid) {      
		sleep(10);

		//read 500 bytes
		char *read_buffer[501];
		ret_code = read (fd[0], read_buffer, 500);
		if (ret_code < 0) {
			printf ("J: Reading from the pipe is failed!!!\n");
			return -1;
		}
		read_buffer[ret_code] = '\0';
		printf ("J:%d\n", ret_code);
		sleep(200);
		sleep(1000);

		//write 2196 bytes
		ret_code=write(fd[1],"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",2196);
		if (ret_code < 0) {
			printf ("J: Writing to the pipe is failed!!!\n");
			return -1;
		}
		printf ("J:%d\n", ret_code);

		sleep(2000);

		//write 200  bytes
		ret_code=write(fd[1],"tKUCWNjn6uVKqSPbYSZEvgB8iWSh9AuCc6KmvaIhUjif8dWSH7h2gh8QnDhyCSHdMolzmvjwuZ7FM5E82JrD3PXnO9yPUVF3yC5QmNyHfVRshWIqWYFF6wNbYxB3vhrtYZx9Mpr1Tm0tpR7pJXORpvToBZL9ZdTSycBTWttyVxZnzumlyewQqh4i1J5tMRAtU3pvFo7B",200);
		if (ret_code < 0) {
			printf ("J: Writing to the pipe is failed!!!\n");
			return -1;
		}
		printf ("J:%d\n", ret_code);

		exit(1);	
	}

	//create child k
	pid=fork();
	if(!pid){
		sleep(10);

		//read 200 bytes
		char *read_buffer1[201];
		ret_code = read (fd[0], read_buffer1, 200);
		if (ret_code < 0) {
			printf ("K: Reading from the pipe is failed!!!\n");
			return -1;
		}
		read_buffer1[ret_code] = '\0';
		printf ("K:%d\n", ret_code);

		//write 1000 bytes
		ret_code=write(fd[1],"q3dTN12dd1LOZWbSIszQGVjG2Rs5QKu8YiQj76BqYkBZREDBhiIb4pqoKR2ss34Lxyanemmf3nrnPyrMoaQ5Vjs7DQdHIusEY9MaM7wBqUmLLmRLDl7eBimDWg7bcoXKOqS0HhsSbTXFNmB7jufN8Eg3nR9C46H7LHwgSCKhQEDiNDkWqAlyWgAjHuR1Xeta4PFuWR3F6RfAvCnnRaoqiSlTWSKoilttQbf3ZVow0lKzm99iAZf7LBizWInDcKDweeNyZwxD1Vpt1dW8JPJu5d0XMTRZulmtfY0xsUI3zf4kBF1QqhgwuFqczpjxwxfWUs65xkghHjlniUErr1Dhxk9RpjdiZY7w7hoeHWdFbhNINsDiKfk80sr7qJSQzBJbU6amyzsT5ZyLHFTB26V0TbLqaOAONFAvfCLxb1xBmeVLokJOovPjTMrT0chPAaZ19XWH8YT8VXq743MRAdHJieIAU08A0dQ1m4hM2XtK6sSQukyRAwRRjbT9AuTFBJWd1SyHj1GP7Y6qFCvvmkG4k5yXpyKrQjcpwJwQGdFTdgL8Kr1u6ppsL7LoXZnAUfsbHSE4kT8fVQXc2mmsehjWHwt0qAuz51Qr77zApqUgF7AhKaJAVDIrWIrlSpSyzf6ui2jTbDin3ycOev4L2Bb3ApfVhYipPEbiGKUkuzpEwI4AEXcEAfYAysKTsFEsv4EX1DzSFEubsrOfiZCYvXrPrNIHiyUxATgAXLPlTHxNwYUso5pUxtpezcRG6HXSoQLEgDHr1WnayELnmqvG0m94NJXvCKujGqkXozkLIjVQCs1VPrpvCeolUMJ3I8lwrCVe5QvWmEMw2UA1Acx5Oj8Dks5FxIE95k1hmYBshtt7gQKroL3JrhFqCqu5tTFqyrLefMtnMcyxIKm0L2nEiSC8iwPmOSeKrAW4ZHjZYj0pbs2AM15uOxcoXWss62hPGZfmOsSE14HruXHugP1GHIdwOLqpivJmR6XrdG0B0bKt",1000);
		if (ret_code < 0) {
			printf ("K: Writing to the pipe is failed!!!\n");
			return -1;
		}
		printf ("K:%d\n", ret_code);
		sleep(200);

        //flush
        ret_code=flush_ppipe(fd);
        printf("K:%d\n",ret_code);

		//read 3000 bytes
		char* read_buffer2[3001];
		ret_code = read (fd[0], read_buffer2, 3000);
		if (ret_code < 0) {
			printf ("K: Reading from the pipe is failed!!!\n");
			return -1;
		}
		read_buffer1[ret_code]='\0';
		printf ("K:%d\n", ret_code);

		sleep(1500);

		//read 2196 bytes
		char* read_buffer3[2197];
		ret_code = read (fd[0], read_buffer3, 2196);
		if (ret_code < 0) {
			printf ("K: Reading from the pipe is failed!!!\n");
			return -1;
		}
		read_buffer1[ret_code]='\0';
		printf ("K:%d\n", ret_code);

		sleep(1000);

		exit(1);
	}
    
	//write 1000 bytes
	ret_code=write(fd[1],"q3dTN12dd1LOZWbSIszQGVjG2Rs5QKu8YiQj76BqYkBZREDBhiIb4pqoKR2ss34Lxyanemmf3nrnPyrMoaQ5Vjs7DQdHIusEY9MaM7wBqUmLLmRLDl7eBimDWg7bcoXKOqS0HhsSbTXFNmB7jufN8Eg3nR9C46H7LHwgSCKhQEDiNDkWqAlyWgAjHuR1Xeta4PFuWR3F6RfAvCnnRaoqiSlTWSKoilttQbf3ZVow0lKzm99iAZf7LBizWInDcKDweeNyZwxD1Vpt1dW8JPJu5d0XMTRZulmtfY0xsUI3zf4kBF1QqhgwuFqczpjxwxfWUs65xkghHjlniUErr1Dhxk9RpjdiZY7w7hoeHWdFbhNINsDiKfk80sr7qJSQzBJbU6amyzsT5ZyLHFTB26V0TbLqaOAONFAvfCLxb1xBmeVLokJOovPjTMrT0chPAaZ19XWH8YT8VXq743MRAdHJieIAU08A0dQ1m4hM2XtK6sSQukyRAwRRjbT9AuTFBJWd1SyHj1GP7Y6qFCvvmkG4k5yXpyKrQjcpwJwQGdFTdgL8Kr1u6ppsL7LoXZnAUfsbHSE4kT8fVQXc2mmsehjWHwt0qAuz51Qr77zApqUgF7AhKaJAVDIrWIrlSpSyzf6ui2jTbDin3ycOev4L2Bb3ApfVhYipPEbiGKUkuzpEwI4AEXcEAfYAysKTsFEsv4EX1DzSFEubsrOfiZCYvXrPrNIHiyUxATgAXLPlTHxNwYUso5pUxtpezcRG6HXSoQLEgDHr1WnayELnmqvG0m94NJXvCKujGqkXozkLIjVQCs1VPrpvCeolUMJ3I8lwrCVe5QvWmEMw2UA1Acx5Oj8Dks5FxIE95k1hmYBshtt7gQKroL3JrhFqCqu5tTFqyrLefMtnMcyxIKm0L2nEiSC8iwPmOSeKrAW4ZHjZYj0pbs2AM15uOxcoXWss62hPGZfmOsSE14HruXHugP1GHIdwOLqpivJmR6XrdG0B0bKt",1000);
	if (ret_code < 0) {
		printf ("I: Writing to the pipe is failed!!!\n");
		return -1;
	}
    printf ("I:%d\n", ret_code);
	
	sleep(100);
	
	//flush
	ret_code=flush_ppipe(fd);
	printf("I:%d\n",ret_code);

	//closes read end
	printf("I:%d\n",close(fd[0]));
	
	//flush
	ret_code=flush_ppipe(fd);
	printf("I99:%d\n",ret_code);

	sleep(10000);

    // Simple return.
	return 0;

}
