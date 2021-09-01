#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{ 
   int i;	
   unsigned long *x;
   void *ptr = sbrk(8192);  //expand by 8KB  
   void *ptr1;
   if(ptr == (void *)-1){
        printf("sbrk failed\n");
   }
   x = (unsigned long*) ptr;
   for(i=0; i<1024; ++i){
       x[i] = i;	   
   }
   ptr1 = sbrk(-4096); // shrink by 4KB   
   
   if(ptr1 == (void *)-1){
        printf("sbrk2 failed\n");
   }	
   printf("%ld\n", x[512]);
   printf("%ld\n", ptr - ptr1);
   return 0;
}
