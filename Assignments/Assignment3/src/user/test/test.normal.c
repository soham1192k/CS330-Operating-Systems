#include<ulib.h>

void do_end_handler()
{
	printf("In End Handler\n");
}

int fn_2()
{
	printf("In fn2\n");
	return 0;
}

int fn_1()
{
	printf("In fn1\n");
	return 0;
}


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{

  printf("Normal Testcase\n fn_1 fn_2 both end_bp_enabled\n");
  printf("main() calls fn_1, exits, then calls fn_2\n ###### \n");

	printf("end_handler address: %x \n",do_end_handler);
  printf("Main address: %x \n", main);
  printf("fn_1 address: %x \n", fn_1);
  printf("fn_2 address: %x \n ###### \n", fn_2);

	int cpid;
	long ret = 0;
	int i, bp_count, rem1, rem2;
	struct breakpoint bp_info[MAX_BREAKPOINTS];	//store breakpoints info here

	ret = become_debugger(do_end_handler);

	cpid = fork();

	if(cpid < 0){
		printf("Error in fork\n");
	}
	else if(cpid == 0){
    printf("In Child main calling fn_1()\n");
		fn_1();
		printf("In Child main. fn_1 finished. Calling fn_2()\n");
		fn_2();
    printf("In Child main(). fn_2 finished\n");
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

		// end handler
		ret = wait_and_continue();


    printf("In Parent main(). Child in fn_1() end.\n");

		bp_count = info_breakpoints(bp_info);
		printf("Breakpoints:\n");
		for(int i = 0; i < bp_count; i++)
		{
			printf("%d %d %x\n", bp_info[i].num, bp_info[i].end_breakpoint_enable, bp_info[i].addr);
		}

		// fn_2
		ret = wait_and_continue();


    printf("In Parent main(). Child in fn_2() beginning.\n");

		rem1 = remove_breakpoint(fn_1);
		rem2 = remove_breakpoint(fn_2);
		printf("Remove Breakpoint fn_1 returned %d\n", rem1);
		printf("Remove Breakpoint fn_2 returned %d\n", rem2);

		bp_count = info_breakpoints(bp_info);

		printf("Breakpoints:\n");
		for(int i = 0; i < bp_count; i++)
		{
			printf("%d %d %x\n", bp_info[i].num, bp_info[i].end_breakpoint_enable, bp_info[i].addr);
		}

		// end handler
		ret = wait_and_continue();


    printf("In Parent main(). Child in fn_2() end.\n");

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
