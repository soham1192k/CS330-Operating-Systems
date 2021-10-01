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

    //write 100 bytes
    char buff[101]="dYupSCVFhVGoHyVYmgMDAzzUnf3XaMSnyiAWiPzPh70l80l1VBemhNZCpkAAejvU7krNOCjAGUDQfnMFZyK74EjSfHhARbmZtx83";
    printf("%d\n",_write(fd[1],buff,100,40));

    int pid=fork();
    if(pid<0){
        printf("ERROR IN FORK\n");
        return -1;
    }
    if(!pid){
        //read 5 bytes
        char buff2[6];
        printf("%d\n",_read(fd[0],buff2,5,50));

        exit(1);
    }

    //read 8 bytes
    char buff1[9];
    printf("%d\n",_read(fd[0],buff1,8,55));
    
    sleep(10);

    printf("%d\n",flush_ppipe(fd));

    // Simple return.
	return 0;

}
