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
	if (!pid) {              
        sleep(30);
		char *read_buffer2[501];
		ret_code = read (fd[0], read_buffer2, 500);
		if (ret_code < 0) {
			printf ("Child: Reading from the pipe is failed!!!\n");
			return -1;
		}
        printf ("%d\n", ret_code);
		
        //flush
        ret_code=flush_ppipe(fd);
        printf("%d\n",ret_code);

        exit(1);
	}

    //write 2000 bytes
	ret_code = write (fd[1], "MYCSy8HagRnOQqTgmg1yd95FBaNfK4onuhqwmZJ9bTAEUHFT8sDuAzlRK01oIwPccqki3nrJBNGLA1VYZE8Olyju5kTO0L182zwckeGRZVIF6KRrmlumGlJbmRhsKhZqHGreUGmm7CkXA2NYp4zsLLYxTJw1TaTdrgFmqDUYRcFE6j5PBXmJWIrIVPQmOeBuX1IswnXKIbYNxRPjcoOT0OqlZQEVQWoUmWkvz95HA2gH2tOZPcoOqKi8CRPuTyGzR3GqxtHvwiWurplJ6mRoQPxG0p2S2ZxPg61TNNZSIh2Oz3U0kYQQ6Is6XbH9faOZhFRxuMU6V3onzOKgtuRJMluN1ZyNssrXy30ctoPUC0QKqpd1YY0sNGEcf84Iu7q5M8rBjo9qQewFCllXL5I5nrrouk1o9pmSJWovdj36Sx4YtUaxgajFKx1KlnMxomLnmb4Qpot2292vAdbDqNbX8a3i47sNeN83QhBsM0UImu1kMZHeKAslID6VRwBFfI5OIDaOTKw2ikoztCv9O4y4rnzKpDBoXfTDOUAPpPkNSjapgyDonPctGYcmlSaRKpqbOYhyQnKAizDUcC3dn8rtUMDmD8qTsAcO3alP6VRefUQEHU04xlTyc80sQFSf3YHveOfw0UWJcg9Gm5zqP59odXYf6AYsw0IDCzqMJUnAVktygOt0oMS7U4UkaTub4aYx1q1EMDqrZTQT6Fo2an3SYK4WtXtIOA8rpKL9RdBI9zV9WMQOZg4ukqOeJpJi0bfdVhD2LloQXXdtsk5XavdbCCRIVn9YpUDgwwpNzYkegBFk9Z0fPasTxjFfeZEQO2b2HxsgnszQ4NfMcToFb7V0XtZfU3fKLnJl2TRWlpIvbhQQnr5ADvVscYOfWo2B195CK3rk0LD6rgqNeHlEFeer5FqISLeuEPIKfIbg4SbeETGzCeBzf7oW4xXQiRplGSAyEzF09opyIp6A4PCp5S3YzqAVyZKX67JTfKcddw7HhTIbOIGtZJ5ojLiNhWAlgQHQOKfMh5geCEHP0wdaFjSm93yY9JO1YNFTNRgUt7auYAbi52Qb8zEEfoywkrKfVj8E7b36A5hy9GvJbsZmMudtDKIerg8WREBactN3qrsBcW2wkMCI5btYn7PB8p7uumuLBe3FcEV74vg0H37tM4T66FhjGQpkxtgo607o9i4WKIa5mJC9kBU3t8AaQDVcy6NuC7N0YsoNkMgsKp9qCQMveTyIDMXdEYq5WSL9dOFrtoyrYTzl8dGlutyP8RdDRX59AySVnp2XDS0VZiq2TmEYIkU72OOUWgTLIg0KqorS2pTvl837mXeY0K9nrO3XHJN5REDOj7D5R8KI1Mg2mbjWkuE6EU85pPn2mDIZhLIUAdiMQ7vbGEeeuTH2oslHLnrQJbQaYJe2Eyh99FvhpwBzH7cdsPQJWVXwicCbGCWWjLqAptOFikHdlPra6r3XwYi8BqLFf4gZXPY5BsBq5o58n6BoeJDYKVBbYvYEKmNNCVMLByZifhKGbqRvLX2t6RuWVTke29hYNyrttQB4T9oNxqCA5rmTQUT96Wn8tmOysMum2h6AaWHsaLTzW114fw0pgkUVB1Z14KfLCq56Xdnx4bmZCKuxhgSesaRdnaPGYo8blFw0wqPRlutdntLquvfSUKAeXmgnOCQidfrQI5fq84dA8N9UN9P42esBRgjGeigGcv6x1ZHNMv6ZIhvcZOQiVcvfBkd7gc6KL0B2RX6RsYYyxy5OeuoT0o0UJSqQJVoDKfdWMi0IURrW3EuD0T1EXjva4pYSYMyjSwVOAeMg7N6vAXMRkm46ltPKf7DEZ4EQIWeI81mxXsh7oS4RHwpCkA5MFL71z3KVoX4ukSwEhA9ncgbcfNd7OHPqOf5OaAnahKTPP5CJAcxnzZJElyAcpUYQkSas9E1rWhtV8eHAHIj6undnnmt5e2kjF12T3LiQuDfqtK3GTKhqYiHN4lBUq9UMiBFtPFb4yyD1",2000);
	if (ret_code < 0) {
		printf ("Parent: Writing to the pipe is failed!!!\n");
		return -1;
	}
    printf ("%d\n", ret_code);

    //read 1000 bytes
    char *read_buffer[1001];
    ret_code = read (fd[0], read_buffer, 1000);
	if (ret_code < 0) {
		printf ("Parent: Reading from the pipe is failed!!!\n");
		return -1;
	}
    read_buffer[1000] = '\0';
    printf ("%d\n", ret_code);

    // Simple return.
	return 0;

}
