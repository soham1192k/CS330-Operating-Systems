#include<ppipe.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<entry.h>
#include<file.h>

// **********************************************************************************************************************************************************************

// Per process information for the ppipe.
struct ppipe_info_per_process {
    int useful;
    int pid;
    int is_read_open,is_write_open;
    int read_pos;
    int isfull;
};

// **********************************************************************************************************************************************************************

// Global information for the ppipe.
struct ppipe_info_global {
    char *ppipe_buff;       // Persistent pipe buffer: DO NOT MODIFY THIS.
    int cnt;
    int write_pos;
    int global_read;
    int data;
    int prev_pid;
};

// **********************************************************************************************************************************************************************

// Persistent pipe structure.
// NOTE: DO NOT MODIFY THIS STRUCTURE.
struct ppipe_info {
    struct ppipe_info_per_process ppipe_per_proc [MAX_PPIPE_PROC];
    struct ppipe_info_global ppipe_global;
};

// **********************************************************************************************************************************************************************

// Function to allocate space for the ppipe and initialize its members.
struct ppipe_info* alloc_ppipe_info() {

    // Allocate space for ppipe structure and ppipe buffer.
    struct ppipe_info *ppipe=(struct ppipe_info*)os_page_alloc(OS_DS_REG);
    char* buffer=(char*)os_page_alloc(OS_DS_REG);

    if(ppipe==NULL) return NULL;
    if(buffer==NULL) return NULL;

    // Assign ppipe buffer.
    ppipe->ppipe_global.ppipe_buff = buffer;

    struct exec_context* curr=get_current_ctx();

    //global fields
    ppipe->ppipe_global.cnt=1;
    ppipe->ppipe_global.write_pos=0;
    ppipe->ppipe_global.global_read=0;
    ppipe->ppipe_global.data=0;
    ppipe->ppipe_global.prev_pid=-1;

    //local fields
    ppipe->ppipe_per_proc[0].useful=1;
    ppipe->ppipe_per_proc[0].pid=curr->pid;
    ppipe->ppipe_per_proc[0].is_read_open=1;
    ppipe->ppipe_per_proc[0].is_write_open=1;
    ppipe->ppipe_per_proc[0].read_pos=0;
    ppipe->ppipe_per_proc[0].isfull=0;

    // Return the ppipe.
    return ppipe;
}

// **********************************************************************************************************************************************************************

// Function to free ppipe buffer and ppipe info object.
void free_ppipe (struct file *filep) {
    os_page_free(OS_DS_REG, filep->ppipe->ppipe_global.ppipe_buff);
    os_page_free(OS_DS_REG, filep->ppipe);
} 

// **********************************************************************************************************************************************************************

// Fork handler for ppipe.
int do_ppipe_fork (struct exec_context *child, struct file *filep) {
    
    /**
     *  TODO:: Implementation for fork handler
     *  Return 0 on success.
     *  Incase of any error return -EOTHERS.
     */

    if(child==NULL) return -EOTHERS;
    if(filep==NULL||filep->type!=PPIPE||filep->ref_count==0) return -EOTHERS;

    int parent_pid=get_current_ctx()->pid;

    int pid=child->pid;
    
    int curr_ind_par=-1;

    for(int i=0;i<MAX_PPIPE_PROC;i++){
        if(filep->ppipe->ppipe_per_proc[i].pid==parent_pid) {curr_ind_par=i;break;}
    }

    if(curr_ind_par==-1) return -EOTHERS;

    if(pid!=filep->ppipe->ppipe_global.prev_pid&&filep->ppipe->ppipe_global.cnt+1>MAX_PPIPE_PROC) return -EOTHERS;

    //increase number of processes referring to the pipe by 1
    if(pid!=filep->ppipe->ppipe_global.prev_pid){
        filep->ppipe->ppipe_global.cnt++;
    }

    //check max_pipe_proc condition
    //CHANGED CODE
    // if(pid!=filep->ppipe->ppipe_global.prev_pid&&filep->ppipe->ppipe_global.cnt>MAX_PPIPE_PROC){
    //     return -EOTHERS;
    // }

    if(pid!=filep->ppipe->ppipe_global.prev_pid){
    
        int curr_ind_child=-1;
        
        for(int i=0;i<MAX_PPIPE_PROC;i++){
            if(filep->ppipe->ppipe_per_proc[i].useful==0){
                curr_ind_child=i;
                break;
            }
        }

        if(curr_ind_child==-1) return -EOTHERS;
        
        filep->ppipe->ppipe_per_proc[curr_ind_child].useful=1;
        filep->ppipe->ppipe_per_proc[curr_ind_child].is_write_open=filep->ppipe->ppipe_per_proc[curr_ind_par].is_write_open;
        filep->ppipe->ppipe_per_proc[curr_ind_child].is_read_open=filep->ppipe->ppipe_per_proc[curr_ind_par].is_read_open;
        filep->ppipe->ppipe_per_proc[curr_ind_child].pid=pid;
        filep->ppipe->ppipe_per_proc[curr_ind_child].read_pos=filep->ppipe->ppipe_per_proc[curr_ind_par].read_pos;
        filep->ppipe->ppipe_per_proc[curr_ind_child].isfull=filep->ppipe->ppipe_per_proc[curr_ind_par].isfull;

        filep->ppipe->ppipe_global.prev_pid=pid;
    }
    
    else filep->ppipe->ppipe_global.prev_pid=-1;

    // Return successfully.
    return 0;
}

// **********************************************************************************************************************************************************************

// Function to close the ppipe ends and free the ppipe when necessary.
long ppipe_close (struct file *filep) {
    /**
     *  TODO:: Implementation of Pipe Close
     *  After successful close, it return 0.
     *  Incase of any error return -EOTHERS.                                                             
     */
    
    if(filep==NULL||filep->type!=PPIPE||filep->ref_count==0) return -EOTHERS;
    int ret_value;
    int curr=-1;
    int pid=get_current_ctx()->pid;

    for(int i=0;i<MAX_PPIPE_PROC;i++){

        if(filep->ppipe->ppipe_per_proc[i].pid==pid){
            curr=i;break;
        }

    }

    if(curr==-1) return -EOTHERS;

    //close the read  or write end based on its mode
    if(filep->mode==O_READ) filep->ppipe->ppipe_per_proc[curr].is_read_open=0;
    if(filep->mode==O_WRITE) filep->ppipe->ppipe_per_proc[curr].is_write_open=0;

    //if both ends are closed, reduce count and make useful 0
    if(filep->ppipe->ppipe_per_proc[curr].is_read_open==0&&filep->ppipe->ppipe_per_proc[curr].is_write_open==0){
        filep->ppipe->ppipe_global.cnt--;
        filep->ppipe->ppipe_per_proc[curr].useful=0;
    }

    //if no processes refer to the pipe then close the pipe
    if(filep->ppipe->ppipe_global.cnt==0){
        free_ppipe(filep);
    }

    // Close the file and return.
    ret_value = file_close (filep);         // DO NOT MODIFY THIS LINE.

    return ret_value;

}

// **********************************************************************************************************************************************************************

// Function to perform flush operation on ppipe.
int do_flush_ppipe (struct file *filep) {

    /**
     *  TODO:: Implementation of Flush system call
     *  In case of any error return -EOTHERS.
     */

    if(filep==NULL||filep->type!=PPIPE||filep->ref_count==0) return -EOTHERS;
    
    int pid=get_current_ctx()->pid;
    
    int pos=filep->ppipe->ppipe_global.global_read;

    int found=-1;
    int id=-1;

    while(1){
        
        for(int i=0;i<MAX_PPIPE_PROC;i++){
            
            if(filep->ppipe->ppipe_per_proc[i].is_read_open==1&&filep->ppipe->ppipe_per_proc[i].read_pos==pos){
                found=pos;id=i;
            }
        
        }
        
        if(found!=-1) break;

        pos=(pos+1)%MAX_PPIPE_SIZE;

        if(pos==filep->ppipe->ppipe_global.global_read) break;
    }

    if(found==-1){
        
        return 0;
        
    }

    int reclaimed_bytes=0;

    if(found<filep->ppipe->ppipe_global.global_read){

        reclaimed_bytes=MAX_PPIPE_SIZE-filep->ppipe->ppipe_global.global_read+found;

        filep->ppipe->ppipe_global.global_read=found;
    
    }

    else if(found>filep->ppipe->ppipe_global.global_read){

        reclaimed_bytes=found-filep->ppipe->ppipe_global.global_read;

        filep->ppipe->ppipe_global.global_read=found;

    }

    else{

        int flag=0;

        for(int i=0;i<MAX_PPIPE_PROC;i++){

            if(filep->ppipe->ppipe_per_proc[i].is_read_open==1&&filep->ppipe->ppipe_per_proc[i].isfull==0){
                flag=1;break;
            }

        }

        if(flag){

            int min_read=-1;
            int p=filep->ppipe->ppipe_global.global_read;

            while(1){
                for(int i=0;i<MAX_PPIPE_PROC;i++){
            
                    if(filep->ppipe->ppipe_per_proc[i].is_read_open==1&&filep->ppipe->ppipe_per_proc[i].read_pos==p&&filep->ppipe->ppipe_per_proc[i].isfull==0){
                        min_read=p;
                    }
            
                }
                
                if(min_read!=-1) break;
                p=(p+1)%MAX_PPIPE_SIZE;

            }
            
            if(min_read<filep->ppipe->ppipe_global.global_read){
                reclaimed_bytes=MAX_PPIPE_SIZE-filep->ppipe->ppipe_global.global_read+min_read;
            }

            else{
                reclaimed_bytes=min_read-filep->ppipe->ppipe_global.global_read;
            }

            filep->ppipe->ppipe_global.global_read=min_read;
        }        

        else{

            reclaimed_bytes=filep->ppipe->ppipe_global.data;
        
        }
    }

    filep->ppipe->ppipe_global.data-=reclaimed_bytes;
    
    if(reclaimed_bytes!=0){

        for(int i=0;i<MAX_PPIPE_PROC;i++){
            filep->ppipe->ppipe_per_proc[i].isfull=0;
        }
    }

    return reclaimed_bytes;
}

// **********************************************************************************************************************************************************************

// Read handler for the ppipe.
int ppipe_read (struct file *filep, char *buff, u32 count) {
    
    /**
     *  TODO:: Implementation of PPipe Read
     *  On successful read, return no of bytes read.
     *  Incase of Error return valid error code.
     *      -EACCES: In case access is not valid.
     *      -EINVAL: If read end is already closed.
     *      -EOTHERS: For any other errors.
     */

    if(!filep||filep->type!=PPIPE) return -EOTHERS;
    if(!(filep->mode&O_READ)) return -EACCES;
    if(buff==NULL) return -EOTHERS;

    int pid=get_current_ctx()->pid;

    int bytes_read=0;
    
    int curr=-1;

    for(int i=0;i<MAX_PPIPE_PROC;i++){
        if(pid==filep->ppipe->ppipe_per_proc[i].pid){
            curr=i;break;
        }
    }

    if(curr==-1) return -EOTHERS;

    if(filep->ppipe->ppipe_per_proc[curr].is_read_open==0) return -EINVAL;

    int k=filep->ppipe->ppipe_per_proc[curr].read_pos;

    if(filep->ppipe->ppipe_per_proc[curr].read_pos<filep->ppipe->ppipe_global.write_pos){
    
        while(bytes_read<count){

            if(k==filep->ppipe->ppipe_global.write_pos) break;
            buff[bytes_read]=filep->ppipe->ppipe_global.ppipe_buff[k];
            bytes_read++;
            k=k+1;

        }

        filep->ppipe->ppipe_per_proc[curr].read_pos=k;
    
        if(k==filep->ppipe->ppipe_global.global_read){
            filep->ppipe->ppipe_per_proc[curr].isfull=1;
        }
    }

    else if(filep->ppipe->ppipe_per_proc[curr].read_pos>filep->ppipe->ppipe_global.write_pos){

        while(bytes_read<count){

            if(k==filep->ppipe->ppipe_global.write_pos) break;
            buff[bytes_read]=filep->ppipe->ppipe_global.ppipe_buff[k];
            bytes_read++;
            k=(k+1)%MAX_PPIPE_SIZE;

        }

        filep->ppipe->ppipe_per_proc[curr].read_pos=k;

        if(k==filep->ppipe->ppipe_global.global_read){
            filep->ppipe->ppipe_per_proc[curr].isfull=1;
        }
    }

    else{

        if(filep->ppipe->ppipe_per_proc[curr].isfull==1){
            bytes_read=0;
        }

        else if(filep->ppipe->ppipe_global.data){

            while(bytes_read<count&&bytes_read<filep->ppipe->ppipe_global.data){
                buff[bytes_read]=filep->ppipe->ppipe_global.ppipe_buff[k];
                bytes_read++;
                k=(k+1)%MAX_PPIPE_SIZE;
            }

            filep->ppipe->ppipe_per_proc[curr].read_pos=k;

            if(k==filep->ppipe->ppipe_global.global_read){
                filep->ppipe->ppipe_per_proc[curr].isfull=1;
            }

        }

        else{

            bytes_read=0;

        }

    }

    return bytes_read;
}

// **********************************************************************************************************************************************************************

// Write handler for ppipe.
int ppipe_write (struct file *filep, char *buff, u32 count) {

    /**
     *  TODO:: Implementation of PPipe Write
     *  On successful write, return no of written bytes.
     *  Incase of Error return valid error code.
     *      -EACCES: In case access is not valid.
     *      -EINVAL: If write end is already closed.
     *      -EOTHERS: For any other errors.
     *
     */

    if(!filep||filep->type!=PPIPE) return -EOTHERS;
    if(!(filep->mode&O_WRITE)) return -EACCES;
    if(buff==NULL) return -EOTHERS;

    int pid=get_current_ctx()->pid;


    int bytes_written=0;
    
    int curr=-1;
    for(int i=0;i<MAX_PPIPE_PROC;i++){
        if(pid==filep->ppipe->ppipe_per_proc[i].pid){
            curr=i;break;
        }
    }

    if(curr==-1) return -EOTHERS;

    if(filep->ppipe->ppipe_per_proc[curr].is_write_open==0) return -EINVAL;

    int k=filep->ppipe->ppipe_global.write_pos;
    
    if(filep->ppipe->ppipe_global.global_read<filep->ppipe->ppipe_global.write_pos){
        
        while(bytes_written<count){
            if(filep->ppipe->ppipe_global.global_read==k) break;
            filep->ppipe->ppipe_global.ppipe_buff[k]=buff[bytes_written];
            bytes_written++;
            k=(k+1)%MAX_PPIPE_SIZE;
        }
        
        filep->ppipe->ppipe_global.write_pos=k;
    
    }
    
    else if(filep->ppipe->ppipe_global.global_read>filep->ppipe->ppipe_global.write_pos){

        while(bytes_written<count){

            if(filep->ppipe->ppipe_global.global_read==k) break;
            filep->ppipe->ppipe_global.ppipe_buff[k]=buff[bytes_written];
            bytes_written++;
            k++;

        }
        
        filep->ppipe->ppipe_global.write_pos=k;

    }

    else{

        if(filep->ppipe->ppipe_global.data){
            bytes_written=0;
        }

        else{

            while(bytes_written<count&&bytes_written<MAX_PPIPE_SIZE){

                filep->ppipe->ppipe_global.ppipe_buff[k]=buff[bytes_written];
                bytes_written++;
                k=(k+1)%MAX_PPIPE_SIZE;

            }

            filep->ppipe->ppipe_global.write_pos=k;
        }
    
    }

    filep->ppipe->ppipe_global.data+=bytes_written;

    return bytes_written;
}

// **********************************************************************************************************************************************************************

// Function to create persistent pipe.
int create_persistent_pipe (struct exec_context *current, int *fd) {

    /**
     *  TODO:: Implementation of PPipe Create
     *  On success, return 0.
     *  Incase of Error return valid Error code.
     *      -ENOMEM: If memory is not enough.
     *      -EOTHERS: Some other errors.
     */

    if(current==NULL) return -EOTHERS;

    fd[0] = 0;
    while(current->files[fd[0]])
    {
        fd[0]++;
        if(fd[0]==MAX_OPEN_FILES) return -EOTHERS; 
    }
    fd[1]=fd[0]+1;
    if(fd[1]==MAX_OPEN_FILES) return -EOTHERS;
    while(current->files[fd[1]])
    {
        fd[1]++;
        if(fd[1]==MAX_OPEN_FILES) return -EOTHERS;
    }

    //create two file objects for both ends by invoking the alloc_file() function
    struct file* end1=alloc_file();
    if(!end1) return -ENOMEM;
    struct file* end2=alloc_file();
    if(!end2) return -ENOMEM;

    //create pipe_info object by invoking alloc_pipe_info() function 
    struct ppipe_info* pipe_obj=alloc_ppipe_info();
    if(!pipe_obj) return -ENOMEM;

    // Fill the fields for those file objects like type, fops, etc.
    end1->ppipe=pipe_obj;
    end1->type=PPIPE;
    end1->inode=NULL;
    end1->mode=O_READ;
    end1->ref_count=1;
    end2->ppipe=pipe_obj;
    end2->type=PPIPE;
    end2->inode=NULL;
    end2->ref_count=1;
    end2->mode=O_WRITE;
    end1->fops->read=ppipe_read;
    end1->fops->write=ppipe_write;
    end2->fops->read=ppipe_read;
    end2->fops->write=ppipe_write;
    end1->fops->close=ppipe_close;
    end2->fops->close=ppipe_close;

    current->files[fd[0]]=end1;
    current->files[fd[1]]=end2;

    // Simple return.
    return 0;
}

// **********************************************************************************************************************************************************************
