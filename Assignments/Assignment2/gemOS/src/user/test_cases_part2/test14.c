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

	//write 3096 bytes
	char buff3[3097]="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";
	printf("W:%d\n",_write(fd[1],buff3,3096,58));

	printf("C:%d\n",close(fd[0]));
	printf("C:%d\n",close(fd[0]));

    // Simple return.
	return 0;

}
