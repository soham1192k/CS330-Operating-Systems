#ifndef __ENTRY_H_
#define __ENTRY_H_
#include <types.h>
#include <context.h>

#define SYSCALL_EXIT      1
#define SYSCALL_GETPID    2
#define SYSCALL_EXPAND    4
#define SYSCALL_SHRINK    5
#define SYSCALL_ALARM     6
#define SYSCALL_SLEEP     7
#define SYSCALL_SIGNAL    8
#define SYSCALL_CLONE     9
#define SYSCALL_FORK      10
#define SYSCALL_STATS     11
#define SYSCALL_CONFIGURE 12
#define SYSCALL_PHYS_INFO   13
#define SYSCALL_DUMP_PTT    14
#define SYSCALL_CFORK       15
#define SYSCALL_MMAP        16
#define SYSCALL_MUNMAP      17
#define SYSCALL_MPROTECT    18
#define SYSCALL_PMAP        19
#define SYSCALL_VFORK      20
#define SYSCALL_GET_USER_P 21
#define SYSCALL_GET_COW_F 22
#define SYSCALL_OPEN        23
#define SYSCALL_READ        24
#define SYSCALL_WRITE       25
#define SYSCALL_PIPE        26
#define SYSCALL_DUP         27
#define SYSCALL_DUP2        28
#define SYSCALL_CLOSE       29
#define SYSCALL_LSEEK       30


#define SYSCALL_MAKE_THREAD_READY 31
#define SYSCALL_WAIT_FOR_THREAD 32

//Error numbers. must be used by appending a unary ,minus

#define EINVAL 1
#define EAGAIN 2
#define EBUSY 3
#define EACCES 4
#define ENOMEM 5


#define MAX_WRITE_LEN 1024
#define MAX_EXPAND_PAGES 1024

struct os_stats{
                u64 swapper_invocations;
                u64 context_switches;
                u64 lw_context_switches;
                u64 ticks;
                u64 page_faults;
                u64 cow_page_faults;
                u64 syscalls;
                u64 used_memory;
                u64 num_processes;
                u64 num_vm_area;
                u64 mmap_page_faults;
                u64 user_reg_pages; // used to check copy-on-write 
                u64 file_objects;
};
extern struct os_stats *stats;
struct os_configs{
                u64 global_mapping;
                u64 apic_tick_interval;
                u64 debug;
                u64 adv_global; 
};

extern struct os_configs *config;

extern long do_syscall(int syscall, u64 param1, u64 param2, u64 param3, u64 param4);
extern int handle_div_by_zero(struct user_regs *regs);
extern int handle_page_fault(struct user_regs *regs);
extern void do_exit(u8);
extern long do_write(struct exec_context *ctx, u64 address, u64 length);
#endif //__ENTRY_H
