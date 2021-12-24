#ifndef __CLONE_THREADS_H_
#define __CLONE_THREADS_H_

#include<types.h>
#include<context.h>

#define MAX_THREADS 4
#define MAX_PRIVATE_AREAS 4

#define TH_UNUSED 0
#define TH_USED 1

#define TP_SIBLINGS_NOACCESS 0x10  //sibling threads can not access
#define TP_SIBLINGS_RDONLY   0x20  //sibling threads can read
#define TP_SIBLINGS_RDWR     0x40  //sibling threads can read and write

#define PF_ERROR_PRESENT 0x1   // If this bit is set in error code => mapping is present
#define PF_ERROR_WR 0x2        // If this bit is set in error code => access is a write
#define PF_ERROR_USER 0x4      // If this bit is set in error code => access is from user mode

#define isThread(c) ((c)->type == EXEC_CTX_USER_TH)
#define isProcess(c) ((c)->type == EXEC_CTX_USER)

/*XXX   Do not change anything above this line*/
struct thread;

struct thread_private_map{
      struct thread *owner;
      u64 start_addr;
      u32 length;
      u32 flags;
};

struct thread{
       u32 pid;   //my pid
       u32 status; // used/not used
       struct exec_context *parent_ctx;
       struct thread_private_map private_mappings[MAX_PRIVATE_AREAS];
};

struct ctx_thread_info{
       int pid;   // The parent pid
       struct thread threads[MAX_THREADS]; 
};

extern long do_make_thread_ready(long pid); 
extern long do_wait_for_thread(long pid);
extern long do_clone(void *th_func, void *user_stack, void *user_arg);
extern void handle_thread_exit(struct exec_context *ctx, u8 normal);
extern int handle_thread_private_mmap(u64 addr, u32 length, u32 prot);
extern void handle_thread_private_unmmap(u64 addr, u32 length);
extern int handle_thread_private_fault(struct exec_context *current, u64 addr, int error_code);
extern int handle_private_ctxswitch(struct exec_context *current, struct exec_context *next);
extern void cleanup_all_threads(struct exec_context *ctx);
extern void segfault_exit(int pid, u64 rip, u64 addr);
struct thread* find_thread_from_pid(struct exec_context *ctx, int thread_pid);
extern struct thread* find_unused_thread(struct exec_context *ctx);
extern struct thread* find_thread_from_address(struct exec_context *ctx, u64 addr, u32 length, struct thread_private_map **thmap);
#endif
