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

	ret = become_debugger(do_end_handler);

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

  		bp_count = info_breakpoints(bp_info);

  		printf("Breakpoints:\n");
  		for(int i = 0; i < bp_count; i++){
  			printf("%d %d %x\n", bp_info[i].num, bp_info[i].end_breakpoint_enable, bp_info[i].addr);
  		}
    }

    for(int i = 0; i < recur_depth; ++i) {
      ret = wait_and_continue();

      printf("In Parent main(). Child in fn_1() end.\n");

  		bp_count = info_breakpoints(bp_info);

  		printf("Breakpoints:\n");
  		for(int i = 0; i < bp_count; i++){
  			printf("%d %d %x\n", bp_info[i].num, bp_info[i].end_breakpoint_enable, bp_info[i].addr);
  		}
    }

		// for exit
		ret = wait_and_continue();

    printf("In Parent main(). Child exited.\n");
	}

	return 0;
}
