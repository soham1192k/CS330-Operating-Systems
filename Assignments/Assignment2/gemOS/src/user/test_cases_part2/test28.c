#include<ulib.h>

int _read(int fd,char* buff,int cnt,int ln){
    int ret_code;
    ret_code=read(fd,buff,cnt);
    if(ret_code<0){
        printf("%d ERROR IN READING!!\n",ln);
        return -1;
    }
    buff[ret_code]='\0';
    return ret_code;
}

int _write(int fd,char* buff,int cnt,int ln){
    int ret_code;
    ret_code=write(fd,buff,cnt);
    if(ret_code<0){
        printf("%d ERROR IN WRITING!!\n",ln);
        return -1;
    }
    return ret_code;
}

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

    int fd1[2];
	// Create persistent pipe.
	ret_code = ppipe(fd1);
	// Check for any error in ppipe creation.
	if (ret_code < 0) {
		printf("Pipe allocation failed!!!\n");
		return -1;
	}
    // Expected result is 0.
    printf ("%d\n", ret_code);


    printf("%d\n",close(fd[0]));

    printf("%d\n",close(fd1[1]));
	

    // Simple return.
	return 0;

}
