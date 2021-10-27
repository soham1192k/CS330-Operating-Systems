#include<ulib.h>

void do_end_handler()
{
	printf("In End Handler\n");
}

int fn_3()
{
  printf("In fn_3()\n");
	return 0;
}

int fn_2()
{
	printf("In fn_2(). Calling fn_3().\n");
  fn_3();
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
  printf("Backtrace Testcase\n");
  printf("breakpointed: fn_1 and fn_3, end_bp_enabled: fn_1\n");
  printf("main() calls fn_1, fn_1 then calls fn_2, fn_2 calls fn_3\n");
  printf(" ###### \n");

	printf("end_handler address: %x \n",do_end_handler);
  printf("Main address: %x \n", main);
  printf("fn_1 address: %x \n", fn_1);
  printf("fn_2 address: %x \n", fn_2);
  printf("fn_3 address: %x \n ###### \n", fn_3);

  int cpid, btsize;
  long ret = 0;
  unsigned long btbuf[8];

  ret = become_debugger(do_end_handler);

  cpid = fork();

  if (cpid < 0) {
    printf("Error in fork\n");
  } else if (cpid == 0) {
		printf("In Child main calling fn_1()\n");
		fn_1();
	} else {
    ret = set_breakpoint(fn_1, 1);
		ret = set_breakpoint(fn_3, 0);

    printf("In Parent main(). Set Breakpoints.\n");

    // fn_1 entry
    ret = wait_and_continue();

    printf("In Parent main(). Child in fn_1() beginning.\n");

    btsize = backtrace((void *)btbuf);
    for (int i = 0; i < btsize; ++i) {
      printf("BackTrace %d / %d : %x\n", i, btsize, btbuf[i]);
    }

    // fn_1 entry
    ret = wait_and_continue();

    printf("In Parent main(). Child in fn_3() beginning.\n");

    btsize = backtrace((void *)btbuf);
    for (int i = 0; i < btsize; ++i) {
      printf("BackTrace %d / %d : %x\n", i, btsize, btbuf[i]);
    }

    // fn_1 entry
    ret = wait_and_continue();

    printf("In Parent main(). Child in fn_1() end.\n");

    btsize = backtrace((void *)btbuf);
    for (int i = 0; i < btsize; ++i) {
      printf("BackTrace %d / %d : %x\n", i, btsize, btbuf[i]);
    }

    // for exit
		ret = wait_and_continue();

    printf("In Parent main(). Child exited.\n");
  }

  return 0;
}
