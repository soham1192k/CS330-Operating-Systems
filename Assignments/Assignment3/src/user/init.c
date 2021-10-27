#include<ulib.h>

void do_end_handler()
{
	printf("In End Handler\n");
}

int fn_1(int recur_depth)
{
	printf("In fn1. Recursion Depth (inverted): %d\n", recur_depth);
  --recur_depth;
  if (recur_depth > 0)
    fn_1(recur_depth);
	return 0;
}


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int cpid;
	long ret = 0;
	int i, bp_count, recur_depth = 3;
	struct breakpoint bp_info[MAX_BREAKPOINTS];	//store breakpoints info here

	printf("Recursive Testcase\n fn_1 end_bp_enabled\n");
  printf("main() calls fn_1, fn_1 calls itself %d times\n", recur_depth);
  printf(" ###### \n");

	printf("end_handler address: %x \n",do_end_handler);
  printf("Main address: %x \n", main);
  printf("fn_1 address: %x \n", fn_1);
	int btsize;
	unsigned long btbuf[8];
	ret = become_debugger(do_end_handler);
	struct registers regs;
	cpid = fork();

	if(cpid < 0){
		printf("Error in fork\n");
	}
	else if(cpid == 0){
		printf("In Child main calling fn_1()\n");
		fn_1(recur_depth);
	}
	else{
		ret = set_breakpoint(fn_1, 1);

    printf("In Parent main(). Set Breakpoints.\n");

		for(int i = 0; i < recur_depth; ++i) {
      ret = wait_and_continue();

      printf("In Parent main(). Child in fn_1() beginning.\n");
		btsize = backtrace((void *)btbuf);
		for (int i = 0; i < btsize; ++i) {
		printf("BackTrace %d / %d : %x\n", i, btsize, btbuf[i]);
		}

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
		printf("\t R8: %x\n", regs.r8);
		printf("\t R9: %x\n", regs.r9);
  		
    }

    for(int i = 0; i < recur_depth; ++i) {
      ret = wait_and_continue();

      printf("In Parent main(). Child in fn_1() end.\n");

  		btsize = backtrace((void *)btbuf);
		for (int i = 0; i < btsize; ++i) {
		printf("BackTrace %d / %d : %x\n", i, btsize, btbuf[i]);
		}
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
		printf("\t R8: %x\n", regs.r8);
		printf("\t R9: %x\n", regs.r9);
    }

		// for exit
		ret = wait_and_continue();

    printf("In Parent main(). Child exited.\n");
	}

	return 0;
}
