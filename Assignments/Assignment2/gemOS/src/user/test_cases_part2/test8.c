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

    //fork
    int pid=fork();
    if(pid<0){
        printf("ERROR IN FORK\n");
        return -1;
    }

    //process 2
    if(!pid){

        sleep(10);

        //read 2000 bytes
        char buff2[2001];
        printf("%d\n",_read(fd[0],buff2,2000,52));

        sleep(30);

        //write 3096 bytes
        char buff3[3097]="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";
        printf("%d\n",_write(fd[1],buff3,3096,58));

        sleep(10);

        //read 3000 bytes
        char buff5[3001];
        printf("%d\n",_read(fd[0],buff5,3000,64));

        sleep(20);

        printf("%d\n",flush_ppipe(fd));

        exit(1);

    }

    //write 3000 bytes
    char buff[3001]="kqVKV8uEXyove2bC6pOYNP9qkwRWW3stMjLepLc6f3zLt4HKEK4nXVYvvsbuG65CCgXOYdvAFKeaBPlmmfyzgil9hzl8qnoIKonxPH0apBoQ11ZBtaQMFQLXB4kPlN23mq8t9VcLXhinPYPuAHa1XkjEfext0Cm8mhF0flAudXL7ZGsyv9URJER24ILFkAHKHDDXHcEfliNWehFHimu1Lhpgri7oupqEAe39Kaujn64vDnBCb89DGO3T7wDFCNEBTxUIqdhUD2LwJAico5xVinQ8BuSyxLwOPIKgAKFiODee1jIf8j5rXkJLu0wXi1sgSHtGOggOAHbxeZegzLa1MzoNJ71LJuGbRan3BdxHiyLmMzwjwharui4WVNvpvVwFPqdprlzFkAiQwyYTMTNPBJDEB36j8eVCuhF0i3izquHT6AqsxiCNX1KUMhMG4UnUScveyir5OyeGcdwPOpubpVlKrWHjMaov3oLburJe9EIi5zHVMzpXVsFbRFtB4t1yRuCfZ4Oyef83tfSitQWBUq8hJHH2ppZz7Juq0m1dlguDhg0XlctXRGfUkuP2WgOAMo8TXsv38SwC15fqMVAz89YUEF9KfQIIqQSMMoFaas02MFlvy9nixRoqdItDEXdBZm8eRsQzPDhmbONWNmy4I2n5QJwsKl7fd3YSBugynemtauoXLovGPokkqMh1N1OwyehJvR8BhLxQKsj26T8sM701m18A0OJdJmXUgWQjTNABiTEhV3n6ldZtwCSkKC1HsH8ITUfpFYjfLO2Yrr6Pd6SjpuRh7PRCiQyDfVSpunGTlrkVo3DFCzaLJzXkHiQewq6qEpA2L6WWilsSEaObNMHVKqFKrEAVC3XJ0V0fCp4IREz2vU7XsxI0YdklJM8ooRJgRe7SYMHagKh8TbWcjfzCFZ3C7j4GzT1c7eboeFUHVWjskNEyP2pFrCQetqaDCbDCih1jXrkQ3xjwjpdWrhF2pOjdcgTsqeXrpFJYM5vITtUuCMHcldTxZxx4iEbp7xCn1SqIR2jf579fNIoxy8R9Jg8i2sGyif4RTnIvPHOGfJIN4MEFnUYl7kOW4zDVpXD33EpR1vIS0NyiV2ITvpkOmi8de0Dz4jlmDYC5IhoqsqJU0kDSDTu3jO5Kn5TGS4nvbJTI28qreFmmFzbQMSSQl8kDIpiwVYxgZudEgVVfSy5iz8UTc1OBR65IlOETo12M0MywJhUhvgGkXtBmhuzDGbW8ETod96gqJx52yYDue70s9qxZeT3R3YqJnzq4Ia0Q2Y8TB1sOKon8ROUGKUOzaSag95zqKlBl3dfbEaL0yjWUbcyF10AjHwv7rMu02sPVArL8S48MjJliF38bHWJVcPTzS6XfSSOhWTSYrihFxp12ykJeEW7i1oPyYHbMEu2Z4Kb6Set7zAlqWwh48L0LPLLql9tPXiIecQ8i4ZG1btNIsxmWP3ldW4t5kGE5DlsGrAbeGhQah9DPxpYZHs1eFQTiCYJiftlhwfpn1acbGs5ysZvE2qo4i1d6BYDEBBVKaqMcrNojYtWpzPzUCZpkW3Kgj1fUP7e3gkwY3QgLTBSIbS1tKKcPMtI1MQSsZHHfGLYE3SFotHTdbe8rYvY9goMzMyA0Ynei93p2C02YdnppcYOWdjVIakUMwjZai3vI7b5AavWWadu0AyccHX3PobdNFzzlOZ3HAeKF0sB7Dcn9yGL3GyTdSqWvQxHPvQUvPxFxBcAUs9SBsoZtM57XK239nJgzkmYHYOyDzMBxyN53T3qqJmeLInoOkLy5C4tbCZjaqevl24Smk4TALkhv8gU98ljV3CRoyPCg5JC8U6XOHztb3DRQK75ARzfAdULg68LQ1qvotRHZhCMYVHEKJTeoSCuOLtIQkZgY1s10PffExi1skN8rFWg1Wj97Ehq83a9nwFRz0W0iPlwNbOuBktoe61XJwoyU3AJDLgNTUnAlqZnYTMJnLqeZbb2uzATR4YH7Dta27Pj5MzE4JVs9XWIGo9ViMS3gKgPvt4TerhA3rxYrosTIuPJ5LtO16cT4tfF4ZjDsd5gFkbx0YH7OAaIglvJkqIvoQjA4k3qIpxA7r3MFHlbbTLmw2jba3WQSDbcMVmcnkNCaIJHZJ5V9iF8ZwSFmK6zuk4rCRHYaOFIsKmYFlMzI1W8htlLNnBfB953tVMpuPMx1pMvHDqAgLsGy2cgCEGmLCfxvR7FQe7OF82pSpom8caPTHTFEzTT3NMlBeu3iovNUVQRcTVv0eSWweBA8WjPCmzRnO2CZEXTxYHi5t66DiEBxIPTH036aTn06w7Ybww1iVM3XLvWgP0yJlxnqt5fxEXz4lrAQUtT5D8DCo4fUPZFACjQW15IJPfLenMSM7txzUHGshQS5gihSz82c9aUaGi3lfl1WOjt7Qs6SdlXxuWL3qZboFOXIEbfUZeX9vAKsnRewIlkx1wWs0aVhLMJVon4bGeIcXVmgE4izeWAUQWVD123VXVKxOeBHUes9r9QDl62INLZjmjOCubm62wgeBNnCyERzJ1QzajLxWqDrJlgXxKhAZcZmG8G8QkWviqfTcrL54IXB14aph473ndm47vqh5c4fEoNxH1NtCJRvk6JeDURmE9diaWhUJGLKZsbPHkstLibviy69SIFUYWEse85SudQy56T73V6YhOqTH81Bn5mubuF78Gq1xmpkCVnAldwSqB7LyPFcy6YqmPb61tT8YODIKw2G1dUOzHXFf6hrw9VyRNuPVkzt4YAqRT4OFTv9dGZczLZCgP6nDMCJCLUlFfv6Ch0tLQQXkAQsvWXe9AX8VHNuIH0P95zRC7dJxmtIZlJXg4bDcSQAISpadjNdaBqfLBmD1vA4Mg62Y1iyduEruD7tIGdVZdrwN17jxaM0dDGEp47YOFpgDCErJ2kc0mw3ovXDvHAZ6pRkPqEaAkaw8DC8vRlltspH8WtpCPfBbhMLKt2YslPnToFDw44yzPssXVcVhvn6JZBW";
    printf("%d\n",_write(fd[1],buff,3000,76));

    //read 2000 bytes
    char buff1[2001];
    printf("%d\n",_read(fd[0],buff1,2000,80));

    sleep(20);

    //flush
    ret_code=flush_ppipe(fd);
    printf("%d\n",ret_code);

    sleep(20);

    //read 4096 bytes
    char buff4[4097];
    printf("%d\n",_read(fd[0],buff4,4096,92));

    sleep(10);

    //write 100 bytes
    char buff6[101]="JoCKyTkC0Put9xOJAyoLV19vb3KqKeLxWjYBRPh0zsPFG4vmAicMef7zsxk5JhvgdnkVSR3TSxs3BO2YAe4zzehqfQAZaqN2hVod";
    printf("%d\n",_write(fd[1],buff6,100,98));

    //read 100 bytes
    char buff7[101];
    printf("%d\n",_read(fd[0],buff7,100,102));

    // Simple return.
	return 0;

}
