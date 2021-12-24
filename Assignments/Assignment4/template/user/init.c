#include<ulib.h>
#include<gthread.h>
/*Thread functions must be declared as static*/
static void *thfunc1(void *arg){
  int *ptr = (int *) arg;
  for(int ctr=0; ctr < *ptr; ++ctr){
    printf("[pid %d]Arg is %d\n", getpid(), *ptr);
  } 
  sleep(*ptr);
  *ptr += 100;
  exit(0);
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  void *stackp;
  int thpid;
  int tharg;
  // clonetc1: create a simple thread using clone 
  
  stackp = mmap(NULL, 8192, PROT_READ|PROT_WRITE, 0);
  if(!stackp || stackp == MAP_ERR){
        printf("Can not allocated stack\n");
        exit(0);
  }
  tharg = 10;
  thpid = clone(&thfunc1, ((u64)stackp) + 8192, &tharg);   // Returns the PID of the thread
  if(thpid <= 0){
       printf("Error creating thread!\n");
       exit(0);
  }
  make_thread_ready(thpid);
  printf("Created thread %d\n", thpid);
  sleep(20);    // Thread sleeps for 10 units, let us sleep for 20 units

  return 0;
}
