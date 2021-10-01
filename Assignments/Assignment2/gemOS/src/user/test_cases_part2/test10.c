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

    //write 3000 bytes
    char buff[3001]="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";
    printf("%d\n",_write(fd[1],buff,3000,76));

    //flush
    ret_code=flush_ppipe(fd);
    printf("%d\n",ret_code);

    char buff1[101];
    printf("%d\n",_read(fd[0],buff1,100,47));

    //flush
    ret_code=flush_ppipe(fd);
    printf("%d\n",ret_code);

    //flush
    ret_code=flush_ppipe(fd);
    printf("%d\n",ret_code);

    // Simple return.
	return 0;

}
