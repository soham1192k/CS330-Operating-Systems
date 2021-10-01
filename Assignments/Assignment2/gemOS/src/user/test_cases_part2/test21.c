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

    //write 1000 bytes
    char buff[1001]="cjsEqHAWRlcdOgx0SdgH30Sk6FGIoqcVdc7SZyG7wtBGVQvcQRrkWA7DlPtYo1N7uxA7xtYfjm2IEUkH5CQo1Qk6w3m7eMEobbCgheHVIYcYEJFs4hXzBVJdVy4Iyw3K0oLndy5hkPsNpPTwgCPwTATZ1P5e4sOgiWzd45hrdozutjD5isaMV5ynK6YaFK56rOPFA1xlAJPpojzuFgjO4OCeTjo5sBjSWxhvmiNNAhq93maTrIoGVdToNVirpRigtW5ZMLFVN28VgB0irlIRRvvql5cODmip39jMKdzFmO22TQVgApVwnn3XuOrTjCO3XtAD2WyBcFcVE0vGMx5SBkCvfMFmGZ4zPpQ0ItRYQuBrbzUdSgi5e3ETGlsfXOEPK3S8lsdd48t4qSkj4rRDbQUdt6WxRiLnG3vpNISE76IBHfzCSu8Vm2c17h2BYNIzYeDuJgtbN4R6GrNBz67tlGqFA1jdNvUMyNJfqQdkiW1f0GKww4L7YkT6Og2gCpqhFr44umVYL5b4JMuVh3wnFhFvTJwkomSIQeRBVDX3Zdfjw0LD6KDqkT3sUvG3REH1JgdI2Dtlk1aPqf1dbTgrp2DlmsHsUqIE1QH2JSYwgANxBuwkiiMmBpAQxmVMs64Qcd3CaWEkYE27mk95MD56ntikV1xNAbaFLIiNCLBdsCKmqubf4o1BfDDadmZu4AziUCSHbjXYUIizI720udsSsXoyJyPgDrKk4LaT6Eqae4EhHcrFNqiZIZUShZLsP6pOlOla5kz9EZ4nc5Jvg1o8uPiScRgRxQgwjc8qny9Z9dUyz5CRcXqmOdCjiRsrl8h3ULq23ZpdCfYjYgFEJ4HLotOvjzBbDp52n26jykEbFekINl0vLh3N16WEbm2env1dQDVCSNg3nMFcdbqtfPV0VcWZYgwAtA4kIHL9xfbafJiQLoFuXbrCmGb8Lhrkhl1yAjJhDyZ8iz3famLJ22t3uryg8caSXfA2ZOKzwDxj";
    printf("1 Write:%d\n",_write(fd[1],buff,1000,40));

    //read 1000 bytes
    char buff1[1001];
    printf("1 Read:%d\n",_read(fd[0],buff1,1000,44));

    //flush
    printf("Flush:%d\n",flush_ppipe(fd));

    //fork
    int pid=fork();
    if(!pid){
        sleep(10);
        char buff4[3197];
        printf("2 READ:%d\n",_read(fd[0],buff4,3196,54));
        sleep(150);
        exit(1);
    }

    //fork
    pid=fork();
    if(!pid){
        sleep(50);
        char buff5[1001];
        printf("3 READ:%d\n",_read(fd[0],buff5,1000,63));
        sleep(150);
        exit(1);
    }

    //write 4096 bytes
    char buff2[4097]="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";
    printf("1 Write:%d\n",_write(fd[1],buff2,4096,63));

    //Read 4096 bytes
    char buff3[4097];
    printf("1 Read:%d\n",_read(fd[0],buff3,4096,66));

    sleep(100);

    //flush
    printf("FLUSH:%d\n",flush_ppipe(fd));

    // Simple return.
	return 0;

}
