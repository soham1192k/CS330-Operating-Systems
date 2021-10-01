#ifndef __PIPE_H_
#define __PIPE_H_

#include "types.h"
#include "context.h"
#include "file.h"


#define MAX_PIPE_SIZE 4096
#define MAX_PIPE_PROC 16


int pipe_read(struct file *filep, char * buff, u32 count);
int pipe_write(struct file *filep, char * buff, u32 count);
long pipe_close(struct file *filep);
extern int create_pipe(struct exec_context *current, int *fd);

// Pipe fork handler.
extern int do_pipe_fork (struct exec_context *child, struct file *filep);

#endif
