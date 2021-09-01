//headers
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<math.h>

//helper function to print error messages
void error(int e,char c){

    if(e==-1){
        //error during creation
        if(c=='c'){
            printf("Failed to complete creation operation\n");
            exit(-1);
        }
        //error during extraction
        else if(c=='d'||c=='e'){
            printf("Failed to complete extraction operation\n");
            exit(-1);
        }
        //error during listing
        else{
            printf("Failed to complete list operation\n");
            exit(-1);
        }

    }
}

//main logic
int main(int argc,char* argv[])
{

    if(strcmp("-c",argv[1])==0){ //create tar

        if(argc!=4){ //if number of arguments is not equal to 4, we have an error
            error(-1,'c');
        }

        int size=strlen(argv[2])+1+strlen(argv[3])+1;

        //actualpath stores the absolute path of the required tar file.
        char *actualpath=malloc(size);
        strcpy(actualpath,argv[2]);strcat(actualpath,"/");strcat(actualpath,argv[3]);

        int src_fd,dst_fd,n,err;

        //opening the required directory
        //Reference: Stack overflow
        struct dirent *pDirent;
        DIR *pDir;
        pDir=opendir(argv[2]);
        if(pDir==NULL){//if error while opening the directory
            error(-1,'c');
        }
        
        //dst_fd stores the file descriptor for the tar file, where we are supposed to write.
        dst_fd=open(actualpath,O_CREAT|O_WRONLY,0644);
        error(dst_fd,'c');

        int filecount=0;//variable to store the number of files whose contents are archived

        unsigned char buffer[4096];
        //we initially write "0" at the top of the tar file. We will come back to this later(to store filecount)
        sprintf(buffer,"%d",0);
        err=write(dst_fd,buffer,1);
        error(err,'c');
        sprintf(buffer,"%s","\n");
        err=write(dst_fd,buffer,strlen("\n"));
        error(err,'c');

        //iterating over the contents of the directory
        while((pDirent=readdir(pDir))!=NULL){
            
            //if file name is "." or ".." or same as the tar file, we continue
            if(strcmp(pDirent->d_name,".")==0||strcmp(pDirent->d_name,"..")==0||strcmp(pDirent->d_name,argv[3])==0){
                continue;
            }

            else{

                filecount++;
                int sz=strlen(argv[2])+1+strlen(pDirent->d_name)+1;

                //p stores the absolute path of each file in the directory
                char *p=malloc(sz);
                strcpy(p,argv[2]);strcat(p,"/");strcat(p,pDirent->d_name);

                //we open this file to read from it
                src_fd=open(p,O_RDONLY);
                error(src_fd,'c');

                //we store the size of this file, using lseek, and write this in the tar file,along with a new line
                int ending=lseek(src_fd,0,SEEK_END);
                int begin=lseek(src_fd,0,SEEK_SET);
                sprintf(buffer,"%d",ending-begin);
                err=write(dst_fd,buffer,1+(int)log10(ending-begin));
                error(err,'c');
                sprintf(buffer,"%s","\n");
                err=write(dst_fd,buffer,strlen("\n"));
                error(err,'c');

                //we write the file name in the tar file.(will help during extraction and listing)
                sprintf(buffer,"%s",pDirent->d_name);
                err=write(dst_fd,buffer,strlen(pDirent->d_name));
                error(err,'c');
                sprintf(buffer,"%s","\n");
                err=write(dst_fd,buffer,strlen("\n"));
                error(err,'c');

                //now we need to read the actual file contents
                //I have done this in chunks of 4096 bytes.
                while(1){
                    
                    //we try to read 4096 bytes
                    err=read(src_fd,buffer,4096);
                    error(err,'c');
                    n=err;
                    if(n==0) break;

                    //we write as many bytes as we have read
                    err=write(dst_fd,buffer,n);
                    error(err,'c');
                }
                //work done for this file, we can close the src_fd
                close(src_fd);
            }
        }

        //we close the directory
        closedir(pDir);

        //we ensure the fd points to the starting of the file, which had a 0 initially.
        //we replace this 0 with our filecount 
        int pos=lseek(dst_fd,0,SEEK_SET);
        sprintf(buffer,"%d",filecount);
        int val=1+(int)log10(filecount);
        err=write(dst_fd,buffer,val);
        error(err,'c');
        close(dst_fd);

         /**
         * TAR structure :
         * 
         * File count
         * File1 Size(in bytes)
         * File 1 Name
         * File 1 contents
         * File 2 Size
         * File 2 Name
         * File 2 Contents
         * .....
         */


        //end of creation operation
    }

    else if(strcmp("-d",argv[1])==0){ //extract tar

        if(argc!=3){ //if number of arguments is not 3, error
            error(-1,'d');
        }

        //folderpath stores the path of the directory
        char *folderpath=(char*)malloc(strlen(argv[2]));
        int pos=-1;
        for(int i=strlen(argv[2])-1;i>=0;i--){
            if(argv[2][i]=='/'){
                pos=i-1;
                break;
            }
        }
        for(int i=0;i<=pos;i++){
            folderpath[i]=argv[2][i];
        }

        //name contains the name of the tar file+"Dump"
        char *name=(char*)malloc(strlen(argv[2])+10);
        for(int i=pos+2;i<strlen(argv[2])-4;i++){
            name[i-pos-2]=argv[2][i];
        }

        //store contains the name of the tar file
        char *store=(char*)malloc((strlen(name)+4)*sizeof(char));
        strcpy(store,name);strcat(name,"Dump");    

        int size=strlen(folderpath)+1+strlen(name)+1;

        //loc contains the path of the new folder to be created while extraction   
        char *loc=(char*)malloc(size);
        strcpy(loc,folderpath);strcat(loc,"/");strcat(loc,name);
        error(mkdir(loc,0777),'d'); //create the directory with the required permissions
        
        int src_fd,dst_fd,err,n;

        //path contains the absolute path of the tar file
        char *path=(char*)malloc(strlen(folderpath)+1+strlen(store)+5);
        strcpy(path,folderpath);strcat(path,"/");strcat(path,store);strcat(path,".tar");

        //we open the tar file for reading
        src_fd=open(path,O_RDONLY);

        //first we read the filecount from the tar file
        //we keep reading character from the first line until we encounter a new line
        char* filecount=(char*)malloc(6);
        char* buffer=(char*)malloc(1*sizeof(char));
        err=read(src_fd,buffer,1);
        error(err,'d');
        strcpy(filecount,buffer);

        while(1){
            buffer=(char*)malloc(1*sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'d');
            //if we hit a newline,break
            if(buffer[0]=='\n'){
                free(buffer);
                break;
            }
            strcat(filecount,buffer);
        }
        int fcount=atoi(filecount);//converting the filecount to an integer
        
        //for each file in the tar file
        while(1){
            
            //to store the file size
            //keep reading till we encounter a new line
            char* filesize=(char*)malloc(11*sizeof(char));
            buffer=(char*)malloc(1*sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'d');
            if(err==0) break;
            strcpy(filesize,buffer);

            while(1){
            
                buffer=(char*)malloc(1*sizeof(char));
                err=read(src_fd,buffer,1);
                error(err,'d');
                if(buffer[0]=='\n'){
                    //if we hit a newline, break
                    free(buffer);
                    break;
                }
                strcat(filesize,buffer);
            }
            int fsize=atoi(filesize);//converting the file size to integer
            
            //to store the filename
            char* filename=(char*)malloc(21*sizeof(char));
            buffer=(char*)malloc(1*sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'d');
            strcpy(filename,buffer);

            while(1){
                buffer=(char*)malloc(1*sizeof(char));
                err=read(src_fd,buffer,1);
                error(err,'d');
                if(buffer[0]=='\n'){
                    //if we hit a new line, break
                    free(buffer);
                    break;
                }
                strcat(filename,buffer);
            }

            int remaining=fsize;

            //to store the path of the new file to be created inside the name+Dump folder
            char* DumpFilePath=(char*)malloc((strlen(loc)+1+strlen(filename))*sizeof(char));
            strcpy(DumpFilePath,loc);strcat(DumpFilePath,"/");strcat(DumpFilePath,filename);

            //open the file with O_CREAT|O_WRONLY flags
            dst_fd = open(DumpFilePath,O_CREAT|O_WRONLY,0644);

            //while we have unread bytes
            while(remaining>0){
            
                // char* buffer2 = (char*) malloc(4096 * sizeof(char));
                buffer=(char*)malloc(4096*sizeof(char));
                
                //if we have less than 4096 bytes to be read, read reamining number of bytes
                if(remaining<=4096){
                    err=read(src_fd,buffer,remaining);
                    remaining=0;
                }

                else{//else read 4096 bytes
                    err=read(src_fd,buffer,4096);
                    remaining-=4096;
                }

                error(err,'d');
                n=err;
                if(n==0) break;

                //write as many bytes as we read
                err=write(dst_fd,buffer,n);
                error(err,'d');
            }
        }
        //end of extraction operation
    }

    else if(strcmp("-e",argv[1])==0){ //extract single file

        if(argc!=4){//if we dont have exactly 4 parameters, error
            error(-1,'e');
        }

        struct dirent *pDirent;
        DIR *pDir;

        int pos=-1;
        for(int i=strlen(argv[2])-1;i>=0;i--){
            if(argv[2][i]=='/'){
                pos=i-1;break;
            }
        }

        //to store the path of the directory
        char *foldername=(char*)malloc(pos+10);
        for(int i=0;i<=pos;i++){
            foldername[i]=argv[2][i];
        }
        
        //open the directory
        pDir=opendir(foldername);
        if(pDir==NULL){
            error(-1,'e');
        }

        //p contains the path of the newly created folder during individual dump
        char *p=(char*)malloc(strlen(foldername)+17);
        strcpy(p,foldername);strcat(p,"/");strcat(p,"IndividualDump");

        //checking if such a folder already exists
        DIR* dir=opendir(p);
        if(dir){
            //if dir!=NULL, then it exists
            closedir(dir);
        }
        else{
            //otherwise create a directory with mode 0777
            error(mkdir(p,0777),'e');
        }
        
        int src_fd,dst_fd,err,n;
        int flag=0;

        //to store path of the tar file
        char *path=(char*)malloc(strlen(argv[2])+1);
        strcpy(path,argv[2]);

        //open the tar file for reading
        src_fd=open(path,O_RDONLY);

        //read the filecount 
        char* filecount=(char*)malloc(6);
        char* buffer = (char*) malloc(1*sizeof(char));
        err=read(src_fd,buffer,1);
        error(err,'e');
        strcpy(filecount,buffer);

        while(1){

            char* buffer = (char*) malloc(1*sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'e');
            if(buffer[0]=='\n'){
                break;
            }
            strcat(filecount,buffer);
        }
        int fcount=atoi(filecount);//convert filecount to integer

        //for every file which has been archived
        for(int i=1;i<=fcount;i++){

            //read the filesize
            char* filesize=(char*)malloc(11*sizeof(char));
            char* buffer = (char*) malloc(1*sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'e');
            strcpy(filesize,buffer);

            while(1){

                char* buffer = (char*) malloc(1*sizeof(char));
                err=read(src_fd,buffer,1);
                error(err,'e');
                if(buffer[0]=='\n'){
                    free(buffer);
                    break;
                }
                strcat(filesize,buffer);
            }
            int fsize=atoi(filesize);//converting filesize to integer

            //read the filename
            char *fname=(char*)malloc(21*sizeof(char));
            buffer=(char*)malloc(1*sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'e');
            strcpy(fname,buffer);

            while(1){

                char* buffer=(char*)malloc(1*sizeof(char));
                err=read(src_fd,buffer,1);
                error(err,'e');
                if(buffer[0]=='\n'){
                    free(buffer);
                    break;
                }
                strcat(fname,buffer);
            } 
            
            //if filename matches the filename passed from console
            if(strcmp(fname,argv[3])==0){

                flag=1;
                //to store the finalpath of the file to be created
                char *finalpath=(char*)malloc((strlen(p)+1+strlen(argv[3]))*sizeof(char));
                strcpy(finalpath,p);strcat(finalpath,"/");strcat(finalpath,argv[3]);
                dst_fd=open(finalpath,O_CREAT|O_WRONLY,0644);
                error(dst_fd,'e');
            }

            else flag=0;
            
            int remaining=fsize;
            
            //read the contents of the file
            while(remaining>0){

                buffer=(char*)malloc(4096*sizeof(char));
                if(remaining<=4096){
                    err=read(src_fd,buffer,remaining);
                    remaining=0;
                }

                else{
                    err=read(src_fd,buffer,4096);
                    remaining-=4096;
                }

                error(err,'e');
                n=err;
                if(n==0) break;

                //if flag is 1, that means the file name matches the argument. We should write only in that case
                if(flag){
                    err=write(dst_fd,buffer,n);
                    error(err,'e');
                }
            }
            if(flag) break;
        }

        if(flag==0){//if flag is 0, that means that no such file was present.
            printf("%s\n","No such file is present in tar file.");
            exit(0);
        }

        close(src_fd);
        closedir(pDir);
        //end of extraction operation
    }

    else if(strcmp("-l",argv[1])==0){ //list the contents

        if(argc!=3){ //if number of arguments is not 3,error
            error(-1,'l');
        }

        //storing the folderpath
        char *folderpath=malloc(strlen(argv[2]));
        int pos=-1;

        for(int i=strlen(argv[2])-1;i>=0;i--){
            if(argv[2][i]=='/'){
                pos=i-1;
                break;
            }
        }
        for(int i=0;i<=pos;i++){
            folderpath[i]=argv[2][i];
        }

        //name of the tar file
        char *name=malloc(strlen(argv[2])+10);

        for(int i=pos+2;i<strlen(argv[2])-4;i++){
            name[i-pos-2]=argv[2][i];
        }

        char *store=(char*)malloc((strlen(folderpath)+1+12+1)*sizeof(char));
        strcpy(store,folderpath);strcat(store,"/tarStructure");

        int src_fd,dst_fd,err,n;

        //open the newly created file
        dst_fd=open(store,O_CREAT|O_WRONLY,0644);

        //path contains path of the tar file 
        char *path=malloc(strlen(folderpath)+1+strlen(name)+5);
        strcpy(path,folderpath);strcat(path,"/");strcat(path,name);strcat(path,".tar");

        //open the tar file to read from it
        src_fd=open(path,O_RDONLY);

        char* tarsize=(char*)malloc(12*sizeof(char));

        //to store and write the size of the tar file
        int tarsizeVal=lseek(src_fd,0,SEEK_END);
        sprintf(tarsize,"%d",tarsizeVal);
        err=write(dst_fd,tarsize,1+(int)log10(tarsizeVal));
        error(err,'l');
        err = write(dst_fd,"\n",1);
        error(err,'l');

        //resetting the fd to point to the beginning of the tar file
        lseek(src_fd,0,SEEK_SET);

        //to store filecount
        char* filecount=(char*)malloc(6*sizeof(char));
        char* buffer=(char*)malloc(1*sizeof(char));
        err=read(src_fd,buffer,1);
        error(err,'l');
        strcpy(filecount,buffer);

        while(1){

            char* buffer=(char*)malloc(1*sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'l');
            if(buffer[0]=='\n'){
                free(buffer);
                break;
            }
            strcat(filecount,buffer);
        }

        //writing the filecount to the tarstructure file
        int fcount=atoi(filecount);
        sprintf(filecount,"%d",fcount);
        err = write(dst_fd,filecount,strlen(filecount));
        error(err,'l');
        err = write(dst_fd,"\n",1);
        error(err,'l');

        //for each file
        while(1){
            
            //to store filesize
            char* filesize=(char*)malloc(15*sizeof(char));
            char* buffer=(char*)malloc(1*sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'l');
            if(err==0) break;
            strcpy(filesize,buffer);

            while(1){
            
                char* buffer=(char*)malloc(1*sizeof(char));
                err=read(src_fd,buffer,1);
                error(err,'l');
                if(buffer[0]=='\n'){
                    free(buffer);
                    break;
                }
                strcat(filesize,buffer);
            }
            int fsize=atoi(filesize);//converting the filesize to integer

            //to store filename
            char* filename=(char*)malloc(20*sizeof(char));
            buffer=(char*)malloc(1*sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'l');
            strcpy(filename,buffer);
            
            while(1){
                
                char* buffer=(char*)malloc(1*sizeof(char));
                err=read(src_fd,buffer,1);
                error(err,'l');
                if(buffer[0]=='\n'){
                    free(buffer);
                    break;
                }
                strcat(filename,buffer);
                // free(buffer);
            }

            //to store and write the size of the file
            int otherinfoLen=(strlen(filename)+1+strlen(filesize)+1);
            char* otherinfo=(char*)malloc(otherinfoLen*sizeof(char));
            strcpy(otherinfo,filename);strcat(otherinfo," ");strcat(otherinfo,filesize);strcat(otherinfo,"\n");
            err = write(dst_fd,otherinfo,otherinfoLen);
            error(err,'l');

            int remaining=fsize;

            //reading the contents of the file
            while(remaining>0){
                buffer =(char*)malloc(4096*sizeof(char));

                if(remaining<=4096){
                    err=read(src_fd,buffer,remaining);
                    remaining=0;
                }

                else{
                    err=read(src_fd,buffer,4096);
                    remaining-=4096;
                }

                error(err,'l');
                n=err;
                if(n==0) break;
            }
        }
    }
    else{
        //flag mismatch
        printf("ERROR-WRONG FLAG\n");
        exit(-1);
    }
    return 0;
}
