//set breakpoint on non breakpointed function present in backtrace, with flag=1
#include<ulib.h>
void do_end_handler()
{
	printf("In End Handler\n");
}
int fn_2(){
    printf("In fn_2.\n");
    return 0;
}
int fn_1()
{
	printf("In fn_1.Calling fn_2\n");
    fn_2();
	return 0;
}
int main(u64 arg1,u64 arg2,u64 arg3,u64 arg4,u64 arg5){
    printf("end_handler address: %x \n",do_end_handler);
    printf("Main address: %x \n", main);
    printf("fn_1 address: %x \n", fn_1);
    printf("fn_2 address:%x\n",fn_2);
    int cpid,btsize;
    long ret = 0;
    int i, bp_count;
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
        ret=set_breakpoint(fn_1,0);
        printf("RET:%d\n",ret);
        ret=set_breakpoint(fn_2,1);
        printf("RET:%d\n",ret);
        ret=wait_and_continue();
        printf("%x\n",ret);
        printf("BEGINNING OF FN_1\n");
        ret=wait_and_continue();
        printf("%x\n",ret);
        printf("BEGINNING OF FN_2\n");
        ret=set_breakpoint(fn_1,1);
        printf("CHANGED BREAKPOINT RET:%d\n",ret);
        ret=wait_and_continue();
        printf("%x\n",ret);
        printf("END OF FN_2\n");
        ret=wait_and_continue();
        printf("RET:%d\n",ret);
        printf("END OF CHILD\n");
    }
    return 0;
}