#ifndef __PPIPE_H_
#define __PPIPE_H_

#include "types.h"
#include "context.h"
#include "file.h"


#define MAX_PPIPE_SIZE 4096
#define MAX_PPIPE_PROC 16


int ppipe_read(struct file *filep, char * buff, u32 count);
int ppipe_write(struct file *filep, char * buff, u32 count);
long ppipe_close(struct file *filep);
extern int create_persistent_pipe(struct exec_context *current, int *fd);

extern int do_flush_ppipe (struct file *filep);
extern int do_ppipe_fork (struct exec_context *child, struct file *filep);

#endif
