#ifndef __GTHREAD_H_
#define __GTHREAD_H_
#include<ulib.h>
   
#define MAX_THREADS 4
#define MAX_GALLOC_AREAS 4
#define TH_STACK_SIZE (1 << 14)


/*Flag to be used for mmap flags MAP_* */
#define MAP_TH_PRIVATE 0x4

/*Flags to be used for mmap system call for PROT_* */
#define TP_SIBLINGS_NOACCESS 0x10  //sibling threads can not access
#define TP_SIBLINGS_RDONLY   0x20  //sibling threads can read
#define TP_SIBLINGS_RDWR     0x40  //sibling threads can read and write

/*Flags to be used for allocation library calls*/
#define GALLOC_OWNONLY 0x0
#define GALLOC_OTRDONLY  0x1
#define GALLOC_OTRDWR 0x2


#define GALLOC_MAX (1<<20)
/*XXX Do not modify anything above*/

/*Add more thread status as required*/

#define TH_STATUS_UNUSED 0x0
#define TH_STATUS_ALIVE 0x1
#define TH_STATUS_SEMI_ALIVE 0x2

struct thread_info;
struct galloc_area{
              struct thread_info *owner;
              u64 start;
              u32 length;
              u32 flags;
};

struct thread_info {
		u8 tid;
		u8 pid;
                u8 status;
		void *ret_addr;
		void *stack_addr;
                struct galloc_area priv_areas[MAX_GALLOC_AREAS];
};

struct process_thread_info{
          struct thread_info threads[MAX_THREADS];
          int num_threads;
};

// The above structure definitions are sufficient to implement the functionalities.

extern int gthread_create(int *tid, void *(*fc)(void *), void *arg);
extern int gthread_exit(void *retval);
extern void* gthread_join(int tid);
/*size can be non-page aligned*/
extern void* gmalloc(u32 size, u8 alloc_flag); 
extern int gfree(void *ptr); 

#endif
