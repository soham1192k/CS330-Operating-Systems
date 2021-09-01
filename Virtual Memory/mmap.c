#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/mman.h>
#include <unistd.h>

int main()
{
   char *ptr;
   void *ptr1;
   ptr = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);
   if(ptr == MAP_FAILED){
        perror("mmap");
        exit(-1);
   }
  printf("ptr = %p\n", ptr);
  strcpy(ptr, "hello cs330!");
  ptr1 = ptr + 4096;
  ptr1 = mmap(ptr1, 4096, PROT_READ, MAP_ANONYMOUS|MAP_PRIVATE|MAP_FIXED, 0, 0);
  printf("ptr1 = %p\n", ptr1);
  munmap((void *)ptr, 4096);
  return 0;
}
