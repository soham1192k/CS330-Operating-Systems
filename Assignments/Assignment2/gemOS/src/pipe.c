#include<pipe.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<entry.h>
#include<file.h>

// **********************************************************************************************************************************************************************

// Per process info for the pipe.
struct pipe_info_per_process {
    int useful;
    int pid;
    int is_read_open,is_write_open;
};

// **********************************************************************************************************************************************************************

// Global information for the pipe.
struct pipe_info_global {
    char *pipe_buff;    // Pipe buffer: DO NOT MODIFY THIS.
    int read_pos,write_pos;
    int cnt;
    int buffer_offset;
    int prev_pid;
};

// **********************************************************************************************************************************************************************

// Pipe information structure.
struct pipe_info {
    struct pipe_info_per_process pipe_per_proc [MAX_PIPE_PROC];
    struct pipe_info_global pipe_global;
};

// **********************************************************************************************************************************************************************

// Function to allocate space for the pipe and initialize its members.
struct pipe_info* alloc_pipe_info () {
	
    // Allocate space for pipe structure and pipe buffer.
    struct pipe_info *pipe = (struct pipe_info*)os_page_alloc(OS_DS_REG);
    char* buffer = (char*) os_page_alloc(OS_DS_REG);

    if(pipe==NULL) return NULL;
    if(buffer==NULL) return NULL;

    // Assign pipe buffer.
    pipe->pipe_global.pipe_buff = buffer;

    struct exec_context* current=get_current_ctx();

    //fill global info fields
    pipe->pipe_global.read_pos=0;
    pipe->pipe_global.write_pos=0;
    pipe->pipe_global.cnt=1;
    pipe->pipe_global.prev_pid=-1;
    pipe->pipe_global.buffer_offset=0;

    // fill per process info fields
    pipe->pipe_per_proc[0].pid=current->pid;
    pipe->pipe_per_proc[0].is_read_open=1;
    pipe->pipe_per_proc[0].is_write_open=1;
    pipe->pipe_per_proc[0].useful=1;

    // Return the pipe.
    return pipe;

}

// **********************************************************************************************************************************************************************

// Function to free pipe buffer and pipe info object.
void free_pipe (struct file *filep) {

    os_page_free(OS_DS_REG, filep->pipe->pipe_global.pipe_buff);
    os_page_free(OS_DS_REG, filep->pipe);

}

// **********************************************************************************************************************************************************************

// Fork handler for the pipe.
int do_pipe_fork (struct exec_context *child, struct file *filep) {

    /**
     *  TODO:: Implementation for fork handler
     *  Return 0 on success.
     *  Incase of any error return -EOTHERS.
     */

    if(child==NULL) return -EOTHERS;
    if(filep==NULL||filep->type!=PIPE||filep->ref_count==0) return -EOTHERS;
    
    int parent_pid=get_current_ctx()->pid;

    int pid=child->pid;
    
    int curr_ind_par=-1;

    for(int i=0;i<MAX_PIPE_PROC;i++){
        if(filep->pipe->pipe_per_proc[i].pid==parent_pid) {curr_ind_par=i;break;}
    }

    if(curr_ind_par==-1) return -EOTHERS;

    if(pid!=filep->pipe->pipe_global.prev_pid&&filep->pipe->pipe_global.cnt+1>MAX_PIPE_PROC) return -EOTHERS;

    //increase number of processes referring to the pipe by 1
    if(pid!=filep->pipe->pipe_global.prev_pid){
        filep->pipe->pipe_global.cnt++;
    }

    //check max_pipe_proc condition
    if(filep->pipe->pipe_global.cnt==MAX_PIPE_PROC) return -EOTHERS;

    if(pid!=filep->pipe->pipe_global.prev_pid){
        
        int curr_ind_child=-1;
        
        for(int i=0;i<MAX_PIPE_PROC;i++){
            if(filep->pipe->pipe_per_proc[i].useful==0){
                curr_ind_child=i;
                break;
            }
        }

        if(curr_ind_child==-1) return -EOTHERS;

        filep->pipe->pipe_per_proc[curr_ind_child].useful=1;
        filep->pipe->pipe_per_proc[curr_ind_child].is_write_open=filep->pipe->pipe_per_proc[curr_ind_par].is_write_open;
        filep->pipe->pipe_per_proc[curr_ind_child].is_read_open=filep->pipe->pipe_per_proc[curr_ind_par].is_read_open;
        filep->pipe->pipe_per_proc[curr_ind_child].pid=pid;
        
        filep->pipe->pipe_global.prev_pid=pid;
    }
    
    else filep->pipe->pipe_global.prev_pid=-1;

    // Return successfully.
    return 0;

}

// **********************************************************************************************************************************************************************

// Function to close the pipe ends and free the pipe when necessary.
long pipe_close (struct file *filep) {

    /**
     *  TODO:: Implementation of Pipe Close
     *  After successful close, it return 0.
     *  Incase of any error return -EOTHERS.
     *
     */

    if(filep==NULL||filep->type!=PIPE||filep->ref_count==0) return -EOTHERS;

    int ret_value;

    int curr=-1;
    int pid=get_current_ctx()->pid;

    for(int i=0;i<MAX_PIPE_PROC;i++){
        if(filep->pipe->pipe_per_proc[i].pid==pid){
            curr=i;break;
        }
    }

    if(curr==-1) return -EOTHERS;

    //close the read  or write end based on its mode
    if(filep->mode==O_READ) filep->pipe->pipe_per_proc[curr].is_read_open=0;
    if(filep->mode==O_WRITE) filep->pipe->pipe_per_proc[curr].is_write_open=0;

    if(filep->pipe->pipe_per_proc[curr].is_read_open==0&&filep->pipe->pipe_per_proc[curr].is_write_open==0){
        filep->pipe->pipe_global.cnt--;
        filep->pipe->pipe_per_proc[curr].useful=0;
    }

    //if only 1 process refers to the pipe and both its ends are closed,free everything
    if(filep->pipe->pipe_global.cnt==0){
        free_pipe(filep);
    }

    // Close the file and return.
    ret_value = file_close (filep);         // DO NOT MODIFY THIS LINE.

    return 0;
}

// **********************************************************************************************************************************************************************

// Check whether passed buffer is valid memory location for read or write.
int is_valid_mem_range (unsigned long buff, u32 count, int access_bit) {

    //UPDATE VM AREA CHECK

    /**
     *  TODO:: Implementation for buffer memory range checking
     *  If range is valid then return 1.
     *  Incase range is not valid or have some permission issue return -EBADMEM.
     */

    struct exec_context* curr=get_current_ctx();

    //starting and ending address of the buffer
    unsigned long buff_start=buff;
    unsigned long buff_end=buff+count-1;

    int ret_value = 1;   
    if(access_bit==1){

        //check for read
        int read_flag=0;
        int hold;

        struct vm_area *head=curr->vm_area;

        //check if it lies in vm_area
        while(head!=NULL){
            unsigned long curr_start=head->vm_start;
            unsigned long curr_end=head->vm_end;
            hold=(head->access_flags&1);
            if(hold==1&&buff_start>=curr_start&&buff_end<=curr_end){
                read_flag=1;
            }

            head=head->vm_next;
            if(read_flag==1) break;
        }
        
        //check if it lies in mm_segment 
        for(int i=0;i<MAX_MM_SEGS;i++){

            hold=(curr->mms[i].access_flags&1);

            if(hold==1&&buff_start>=curr->mms[i].start&&buff_end<curr->mms[i].next_free){
                read_flag=1;
            }

            if(i==MAX_MM_SEGS-1&&hold==1&&buff_start>=curr->mms[i].start&&buff_end<=curr->mms[i].end){
                read_flag=1;
            }
            
            if(read_flag==1) break;
        }

        if(read_flag==0) return -EBADMEM;
    }

    else if(access_bit==2){

        //check for write
        int write_flag=0;
        int hold;
        
        struct vm_area *head=curr->vm_area;

        //check if it lies in vm_area
        while(head!=NULL){
            unsigned long curr_start=head->vm_start;
            unsigned long curr_end=head->vm_end;
            hold=(head->access_flags&2);
            if(hold==2&&buff_start>=curr_start&&buff_end<=curr_end){
                write_flag=1;
            }

            head=head->vm_next;
            if(write_flag==1) break;
        }

        for(int i=0;i<MAX_MM_SEGS;i++){

            hold=(curr->mms[i].access_flags&2);

            if(hold==2&&buff_start>=curr->mms[i].start&&buff_end<curr->mms[i].next_free){
                write_flag=1;
            }

            if(i==MAX_MM_SEGS-1&&hold==2&&buff_start>=curr->mms[i].start&&buff_end<=curr->mms[i].end){
                write_flag=1;
            }

            if(write_flag==1) break;
        }

        if(write_flag==0) return -EBADMEM;
    }

    return ret_value;
}

// **********************************************************************************************************************************************************************

// Function to read given no of bytes from the pipe.
int pipe_read (struct file *filep, char *buff, u32 count) {

    //user buffer me write=>check write permission for "buff"
    /**
     *  TODO:: Implementation of Pipe Read
     *  Incase of Error return valid error code.
     *       -EACCES: In case access is not valid.
     *       -EINVAL: If read end is already closed.
     *       -EOTHERS: For any other errors.
     *
     */
    
    if(!filep||filep->type!=PIPE) return -EOTHERS;
    if(!(filep->mode&O_READ)) return -EACCES;
    if(buff==NULL) return -EOTHERS;

    int pid=get_current_ctx()->pid;

    int curr=-1;
    
    for(int i=0;i<MAX_PIPE_PROC;i++){
        if(filep->pipe->pipe_per_proc[i].pid==pid){
            curr=i;break;
        }
    }

    if(curr==-1) return -EOTHERS;

    if(filep->pipe->pipe_per_proc[curr].is_read_open==0) return -EINVAL;

    int ret=is_valid_mem_range((unsigned long)buff,count,2);
    if(ret==-EBADMEM) return -EBADMEM;

    int k=filep->pipe->pipe_global.read_pos;

    int bytes_read = 0;

    while(bytes_read<count){
        if(filep->pipe->pipe_global.buffer_offset==0) break;
        buff[bytes_read]=filep->pipe->pipe_global.pipe_buff[k];
        bytes_read++;
        k=(k+1)%MAX_PIPE_SIZE;
        filep->pipe->pipe_global.buffer_offset--;
    }

    filep->pipe->pipe_global.read_pos=k;

    return bytes_read;
}

// **********************************************************************************************************************************************************************

// Function to write given no of bytes to the pipe.
int pipe_write (struct file *filep, char *buff, u32 count) {

    /**
     *  TODO:: Implementation of Pipe Write
     *  On successful write, return no of written bytes.
     *  Incase of Error return valid error code.
     *       -EACCES: In case access is not valid.
     *       -EINVAL: If write end is already closed.
     *       -EOTHERS: For any other errors.
     *
     */
    
    if(!filep||filep->type!=PIPE) return -EOTHERS;
    if(!(filep->mode&O_WRITE)) return -EACCES;
    if(buff==NULL) return -EOTHERS;

    int pid=get_current_ctx()->pid;
    
    int curr=-1;
    for(int i=0;i<MAX_PIPE_PROC;i++){
        if(filep->pipe->pipe_per_proc[i].pid==pid){
            curr=i;break;
        }
    }

    if(curr==-1) return -EOTHERS;

    if(filep->pipe->pipe_per_proc[curr].is_write_open==0) return -EINVAL;
    
    int ret=is_valid_mem_range((unsigned long)buff,count,1);
    if(ret==-EBADMEM) return -EBADMEM;
    
    int bytes_written = 0;
    int k=filep->pipe->pipe_global.write_pos;
    while(bytes_written<count){
        if(filep->pipe->pipe_global.buffer_offset==MAX_PIPE_SIZE) break;
        filep->pipe->pipe_global.pipe_buff[k]=buff[bytes_written];
        bytes_written++;
        k=(k+1)%MAX_PIPE_SIZE;
        filep->pipe->pipe_global.buffer_offset++;
    }
    filep->pipe->pipe_global.write_pos=k; 

    return bytes_written;
}

// **********************************************************************************************************************************************************************

// Function to create pipe.
int create_pipe (struct exec_context *current, int *fd) {

    /**
     *  TODO:: Implementation of Pipe Create
     *  Incase of Error return valid Error code.
     *       -ENOMEM: If memory is not enough.
     *       -EOTHERS: Some other errors.
    */

    if(current==NULL) return -EOTHERS;

    //find two free file descriptors
    //Fill the valid file descriptor in *fd param.
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
    struct pipe_info* pipe_obj=alloc_pipe_info();
    if(!pipe_obj) return -ENOMEM;

    // Fill the fields for those file objects like type, fops, etc.
    end1->pipe=pipe_obj;
    end1->type=PIPE;
    end1->inode=NULL;
    end1->mode=O_READ;
    end1->ref_count=1;
    end2->pipe=pipe_obj;
    end2->type=PIPE;
    end2->inode=NULL;
    end2->mode=O_WRITE;
    end2->ref_count=1;
    end1->fops->read=pipe_read;
    end2->fops->write=pipe_write;
    end1->fops->close=pipe_close;
    end2->fops->close=pipe_close;

    current->files[fd[0]]=end1;
    current->files[fd[1]]=end2;

    // Simple return.
    return 0;
}

// **********************************************************************************************************************************************************************
