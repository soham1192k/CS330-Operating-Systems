#include<types.h>
#include<context.h>
#include<file.h>
#include<lib.h>
#include<serial.h>
#include<entry.h>
#include<memory.h>
#include<fs.h>
#include<kbd.h>
#include<ppipe.h>
#include<pipe.h>
//Generic routines

static void delete_file(struct file *filep)
{
	os_page_free(OS_DS_REG ,filep);
	stats->file_objects--;
} 

struct file *alloc_file()
{
	struct file *file = (struct file *) os_page_alloc(OS_DS_REG); 
	file->fops = (struct fileops *) (file + sizeof(struct file)); 
	bzero((char *)file->fops, sizeof(struct fileops));
	file->ref_count = 1;
	file->offp = 0;
	stats->file_objects++;
	return file; 
}

void *alloc_memory_buffer()
{
	return os_page_alloc(OS_DS_REG); 
}

void free_memory_buffer(void *ptr)
{
	os_page_free(OS_DS_REG, ptr);
}


int do_file_fork (struct exec_context *child) {
	
	int fd_index;
	int ret_value = 0;

	for (fd_index = 0; fd_index < MAX_OPEN_FILES; ++fd_index) {

		if (child->files[fd_index] != NULL) {
		
            // Increment ref count.
            child->files[fd_index]->ref_count++;

            // Call pipe or ppipe fork handlers.
			if (child->files[fd_index]->type == PPIPE)
				ret_value = do_ppipe_fork(child, child->files[fd_index]);
            else if (child->files[fd_index]->type == PIPE)
                ret_value = do_pipe_fork(child, child->files[fd_index]);
		
        }

	}

	return ret_value;

}

void do_file_exit (struct exec_context *ctx) {

	int fd_index;
	for (fd_index=0; fd_index < MAX_OPEN_FILES; ++fd_index) {
		
        struct file *file = ctx->files[fd_index];
		if (file!= NULL) {

            // Call close handler.
            file->fops->close(file);

        }
		
        ctx->files[fd_index] = NULL;
	
    }

}


/* STDIN,STDOUT and STDERR Handlers */


/* read call corresponding to stdin */

static int do_read_kbd(struct file* filep, char * buff, u32 count)
{
	kbd_read(buff);
	return 1;
}

/* write call corresponding to stdout */

static int do_write_console(struct file* filep, char * buff, u32 count)
{
	struct exec_context *current = get_current_ctx();
	return do_write(current, (u64)buff, (u64)count);
}

long std_close(struct file *filep)
{
	filep->ref_count--;
	if(!filep->ref_count)
		delete_file(filep);
	return 0;
}

long file_close(struct file *filep)
{
        filep->ref_count--;
        if(!filep->ref_count)
                delete_file(filep);
        return 0;
}



struct file *create_standard_IO(int type)
{
	struct file *filep = alloc_file();
	filep->type = type;
	if(type == STDIN)
		filep->mode = O_READ;
	else
		filep->mode = O_WRITE;
	if(type == STDIN){
		filep->fops->read = do_read_kbd;
	}else{
		filep->fops->write = do_write_console;
	}
	filep->fops->close = std_close;
	return filep;
}

int open_standard_IO(struct exec_context *ctx, int type)
{
	int fd = type;
	struct file *filep = ctx->files[type];
	if(!filep){
		filep = create_standard_IO(type);
	}else{
		filep->ref_count++;
		fd = 3;
		while(ctx->files[fd])
			fd++; 
	}
	ctx->files[fd] = filep;
	return fd;
}

/*Regular file handlers to be written as part of the assignmemnt*/

/* read call corresponding to regular file */

static int do_read_regular(struct file *filep, char * buff, u32 count)
{
	struct inode * inode;
	u32 * offp;
	int read_size;

	offp = &(filep->offp);
	dprintk("inside do read regular\n");
	dprintk("offset in do regular read:%d\n",*offp);
	inode = filep->inode;
	dprintk("inode size %d  offset address inside do read regular:%x\n",inode->file_size, filep->offp, filep->inode);
	read_size = inode->read(inode, buff, count, offp);
	*offp += read_size;
	dprintk("read size inside do read regular:%d\n",read_size);
	return read_size;
}

/*write call corresponding to regular file */

static int do_write_regular(struct file *filep, char * buff, u32 count)
{
	struct inode * inode;
	u32 * offp;
	int write_size;
	dprintk("inside do write regular\n");
	offp = &(filep->offp);
	inode = filep->inode;
	write_size = inode->write(inode, buff, count,offp);
	if(write_size>=0){
		*offp += write_size;
	}
	return write_size; 
}

static long do_lseek_regular(struct file *filep, long offset, int whence)
{
	long rpos;
	struct inode *inode = filep->inode;
	if(!inode)
		return -EINVAL;  
	if(whence == SEEK_SET){
		rpos = offset;
	}else if(whence == SEEK_CUR || whence == SEEK_END){
		rpos = offset + filep->offp;
	}else{
		return -EINVAL;
	}
	if(rpos < 0 || rpos > inode->file_size)
		return -EINVAL;
	filep->offp = rpos;
	return rpos;
}

extern int do_regular_file_open(struct exec_context *ctx, char* filename, u64 flags, u64 mode)
{
	struct file *filep;
	struct inode * inode;
	u32 fd, i;

	if((flags & O_CREAT) == O_CREAT){
		inode = create_inode((char*)filename, mode);
		// the above call will return NULL if exists
		// check if NULL was returned due to file exist or
		// not enough memory
		inode = lookup_inode(filename);
		if(inode == NULL){
			// printk("should not come here\n");
			//it should never happen because just we created file.
			return -ENOMEM;
		}
		else{
			// file exists
		}
		goto file_alloc;
	}
	inode = lookup_inode(filename);
	if(inode == NULL){
		// printk("file does not exist\n");
		return -EINVAL; //file does not exist
	}
	
file_alloc:
	// remove the O_CREAT from flags before permission check
	if(flags & O_CREAT)
		flags ^= O_CREAT;
	if((inode->mode & flags) != flags){
		return -EACCES;
	}

	// Find a file descriptor
	for(i=3;i<MAX_OPEN_FILES;i++){
		if(ctx->files[i] == NULL){
			fd = i;
			dprintk("fd in open :%d\n",fd);
			break;
		}
	}
	if(i == MAX_OPEN_FILES)
	{
		return -ENOMEM;
	}
	filep = alloc_file();
	filep->type = REGULAR;
	filep->mode =  (flags == O_CREAT ? mode: flags);
	filep->offp = 0;
	filep->inode = inode;
	filep->fops->read = do_read_regular;
	filep->fops->write = do_write_regular;
	filep->fops->close = std_close;
	filep->fops->lseek = do_lseek_regular;
	filep->inode->open(inode);
	ctx->files[fd] = filep;

	return fd; 
}

// DUP and DUP2


/**
 * Implementation of dup system call.
 */

int fd_dup(struct exec_context *current, int oldfd)
{
	struct file *filep = current->files[oldfd];
	int ret_fd = -EINVAL; 
	if(filep)
	{
		// Lowest fd appart from omiting the standard file descriptor;
		int fd = 3;
		while(fd < MAX_OPEN_FILES)
		{
			if(!current->files[fd])
			{
				// Copying the file descriptor
				// current->of[fd] = copy_file_descriptor(current->of[oldfd]);

				current->files[fd] = filep;
				filep->ref_count++;
				ret_fd = fd;
				break;
			}
			fd++;
		}
	}
	return ret_fd;
}

/**
 * Implementation dup 2 system call;
 */
int fd_dup2(struct exec_context *current, int oldfd, int newfd)
{
	int ret_fd = -EINVAL; 
	struct file *filep = current->files[newfd];
	if(current->files[oldfd] != NULL)
	{
		// If old and new fd are same do nothing
		if(oldfd == newfd)
		{
			ret_fd = newfd;
		}else{
			if(filep)
				filep->fops->close(filep);
			// copying the old fd content to the new fd
			// current->of[newfd] = copy_file_descriptor(current->of[oldfd]);
			current->files[newfd] = current->files[oldfd];
			current->files[newfd]->ref_count++;
			ret_fd = newfd;
		} 
	}
	return ret_fd;
}


int do_sendfile(struct exec_context *ctx, int outfd, int infd, long *offset, int count) {
	struct file *outfilep = ctx->files[outfd]; // for writing
	struct file *infilep = ctx->files[infd]; // for reading
	int read_size = 0;
	int write_size = 0;

	if(!infilep){
		return -EINVAL; //file is not opened
	}
	if((infilep->mode & O_READ) != O_READ){
		return -EACCES; //file is write only
	}

	void* buff = alloc_memory_buffer();
	u32 prev_offp;
	if(infilep->fops->read){
		if (offset != NULL) {
		    prev_offp = infilep->offp; // save prev offset
		    if (infilep->fops->lseek(infilep, *offset, SEEK_SET) == -EINVAL) {
			return -EINVAL;
		    }
		    read_size = infilep->fops->read(infilep, (char*)buff, count);
		    *offset = infilep->offp; // change offset arg value
		    infilep->offp = prev_offp; // restore offset
		}
		else {
		    read_size = infilep->fops->read(infilep, (char*)buff, count);
		}
		dprintk("buff inside read:%s\n",buff);
		dprintk("read size:%d\n",read_size);
	//	return read_size;
	}
	else {
	    return -EINVAL;
	}


	if(!outfilep){
		return -EINVAL; //file is not opened
	}
	if(!(outfilep->mode & O_WRITE)){
		return -EACCES; // file is not opened in write mode
	}
	if(outfilep->fops->write){
		write_size = outfilep->fops->write(outfilep, (char*)buff, count);
		dprintk("write size:%d\n",write_size);
		return write_size;
	}
	free_memory_buffer(buff);
	return -EINVAL;
}

