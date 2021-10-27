#include<ulib.h>

void do_end_handler()
{
	printf("In End Handler\n");
}

int fn_2()
{
	printf("In fn_2\n");
	return 0;
}

int fn_1()
{
	printf("In fn_1(). Calling fn_2()\n");
  fn_2();
	return 0;
}


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  printf("Nested Testcase\n fn_1 fn_2 both end_bp_enabled\n");
  printf("main() calls fn_1, fn_1 then calls fn_2\n ###### \n");

	printf("end_handler address: %x \n",do_end_handler);
  printf("Main address: %x \n", main);
  printf("fn_1 address: %x \n", fn_1);
  printf("fn_2 address: %x \n ###### \n", fn_2);

	int cpid;
	long ret = 0;
	int i, bp_count;
	struct breakpoint bp_info[MAX_BREAKPOINTS];	//store breakpoints info here

	ret = become_debugger(do_end_handler);

	cpid = fork();

	if(cpid < 0){
		printf("Error in fork\n");
	}
	else if(cpid == 0){
		printf("In Child main calling fn_1()\n");
		fn_1();
	}
	else{
		ret = set_breakpoint(fn_1, 1);
		ret = set_breakpoint(fn_2, 1);

    printf("In Parent main(). Set Breakpoints.\n");

		// fn_1
		ret = wait_and_continue();

    printf("In Parent main(). Child in fn_1() beginning.\n");

		bp_count = info_breakpoints(bp_info);

		printf("Breakpoints:\n");
		for(int i = 0; i < bp_count; i++)
		{
			printf("%d %d %x\n", bp_info[i].num, bp_info[i].end_breakpoint_enable, bp_info[i].addr);
		}

		// fn_2
		ret = wait_and_continue();

    printf("In Parent main(). Child in fn_2() beginning.\n");

		bp_count = info_breakpoints(bp_info);
		printf("Breakpoints:\n");
		for(int i = 0; i < bp_count; i++)
		{
			printf("%d %d %x\n", bp_info[i].num, bp_info[i].end_breakpoint_enable, bp_info[i].addr);
		}

		// fn_2 end_handler
		ret = wait_and_continue();

    printf("In Parent main(). Child in fn_2() end.\n");

		bp_count = info_breakpoints(bp_info);

		printf("Breakpoints:\n");
		for(int i = 0; i < bp_count; i++)
		{
			printf("%d %d %x\n", bp_info[i].num, bp_info[i].end_breakpoint_enable, bp_info[i].addr);
		}

		// fn_1 end handler
		ret = wait_and_continue();

    printf("In Parent main(). Child in fn_1() end.\n");

		bp_count = info_breakpoints(bp_info);
		printf("Breakpoints:\n");
		for(int i = 0; i < bp_count; i++)
		{
			printf("%d %d %x\n", bp_info[i].num, bp_info[i].end_breakpoint_enable, bp_info[i].addr);
		}

		// for exit
		ret = wait_and_continue();

    printf("In Parent main(). Child exited.\n");
	}

	return 0;
}
