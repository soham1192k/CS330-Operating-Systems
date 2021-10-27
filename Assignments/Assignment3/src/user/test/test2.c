#include<ulib.h>
void do_end_handler()
{
	printf("In End Handler\n");
}
int fn_1()
{
	printf("In fn_1.\n");
	return 0;
}
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5){
    printf("end_handler address: %x \n",do_end_handler);
    printf("Main address: %x \n", main);
    printf("fn_1 address: %x \n", fn_1);
    int cpid;
	long ret = 0;
	int i, bp_count;
	struct breakpoint bp_info[MAX_BREAKPOINTS];	
    ret = become_debugger(do_end_handler);
    cpid=fork();
    if(cpid < 0){
		printf("Error in fork\n");
	}
	else if(cpid == 0){
		printf("In Child main calling fn_1()\n");
		fn_1();
	}
    else{
        ret=set_breakpoint(fn_1,1);
        printf("SET BRKPOINT fn1:%d\n",ret);
        ret=wait_and_continue();
        printf("RET:%x\n",ret);
        printf("AT fn_1 beginning\n");
        struct registers regs;
        info_registers(&regs);
        printf("Registers:\n");
		printf("\t RIP: %x\n", regs.entry_rip);
        printf("\t RSP: %x\n", regs.entry_rsp);
        printf("\t RBP: %x\n", regs.rbp);
        printf("\t RAX: %x\n", regs.rax);
        printf("\t RDI: %x\n", regs.rdi);
        printf("\t RSI: %x\n", regs.rsi);
        printf("\t RDX: %x\n", regs.rdx);
        printf("\t RCX: %x\n", regs.rcx);
        printf("\t RIP: %x\n", regs.r8);
        printf("\t RIP: %x\n", regs.r9);
        ret=wait_and_continue();
        printf("RET:%x\n",ret);
        printf("AT fn_1 end\n");
        info_registers(&regs);
        printf("Registers:\n");
		printf("\t RIP: %x\n", regs.entry_rip);
        printf("\t RSP: %x\n", regs.entry_rsp);
        printf("\t RBP: %x\n", regs.rbp);
        printf("\t RAX: %x\n", regs.rax);
        printf("\t RDI: %x\n", regs.rdi);
        printf("\t RSI: %x\n", regs.rsi);
        printf("\t RDX: %x\n", regs.rdx);
        printf("\t RCX: %x\n", regs.rcx);
        printf("\t RIP: %x\n", regs.r8);
        printf("\t RIP: %x\n", regs.r9);
        ret=wait_and_continue();
        printf("RET:%x\n",ret);
        printf("CHILD FINISHED\n");
    }
    return 0;
}