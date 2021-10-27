#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <types.h>
#include <context.h>

#define MAX_BACKTRACE	64
#define MAX_BREAKPOINTS 8

#define INT3_OPCODE 	0xCC
#define PUSHRBP_OPCODE 	0x55
#define END_ADDR 	0x10000003B

#define CHILD_EXIT	0

/*
 * node of breakpoint list
 * for kernel use
 */
struct breakpoint_info{
	u32 num;		       // breakpoint number
	u64 addr;		       // address on which breakpoint is set
	struct breakpoint_info *next;  // pointer to next node in breakpoint list
	int end_breakpoint_enable;     // tells if there should be a end breakpoint
};

/*
 * stores information about registers
 * of the debugged process
 */

struct registers{

	u64 r15;
	u64 r14;
	u64 r13;
	u64 r12;
	u64 r11;
	u64 r10;
	u64 r9;
	u64 r8;
	u64 rbp;
	u64 rdi;
	u64 rsi;
	u64 rdx;
	u64 rcx;
	u64 rbx;
	u64 rax;
	u64 entry_rip;
	u64 entry_cs;
	u64 entry_rflags;
	u64 entry_rsp;
	u64 entry_ss;
};

/*
 * stores information about debugger
 * NULL for all other processes
 */
struct debug_info{
	
	u32 breakpoint_count;		// number of breakpoints present
	struct breakpoint_info *head;   // head of breakpoint list
	void *end_handler;
	u32 breakpoint_num;

	struct registers regs;
	
	u64 trace[MAX_BACKTRACE];
	int trace_size;

	u64 call_stack[MAX_BACKTRACE];
	int call_stack_size;
	
	u64 store_retaddr[MAX_BACKTRACE];
	int retaddr_size;
};

/*
 * contains information about a breakpoint
 * that can be passed to user space
 */
struct breakpoint{
        int num;        //breakpoint number
        int end_breakpoint_enable;     //enabled or disabled
        u64 addr;       //address on which breakpoint is set
};

extern long int3_handler(struct exec_context *ctx);
extern void debugger_on_fork(struct exec_context *child_ctx);
extern void debugger_on_exit(struct exec_context *ctx);
extern int do_become_debugger(struct exec_context *ctx, void *addr);
extern int do_set_breakpoint(struct exec_context *ctx, void *addr, int flag);
extern int do_remove_breakpoint(struct exec_context *ctx, void *addr);

extern int do_info_breakpoints(struct exec_context *ctx, struct breakpoint *bp);
extern int do_info_registers(struct exec_context *ctx, struct registers *reg);
extern int do_backtrace(struct exec_context *ctx, u64 bt);
extern s64 do_wait_and_continue(struct exec_context *ctx);

// for testing
extern void print_breakpoints(struct breakpoint_info *head);
#endif
