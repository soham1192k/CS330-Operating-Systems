#include<ulib.h>
 
 
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {
 
   // Array to store read and write fds of the pipe.
   int fd[2];
 
   // Create the pipe.
   int ret_code = pipe (fd);
   if (ret_code < 0) {
       printf ("Pipe op is failed!!!\n");
       return -1;
   }
   // Expected return will be 0.
   printf ("%d\n", ret_code);
 
   // Expected values for these fds will be 3 and 4.
   printf ("%d\n", fd[0]);
   printf ("%d\n", fd[1]);
  
   // Write to the pipe.
   char write_buff[4096] = "hello";
   char read_buff[4096];
   ret_code = write (fd[1], write_buff, 100);
   if (ret_code < 0) {
       printf ("Pipe write is failed!!!\n");
       return -1;
   } else {
       // Expected result should be 5.
       printf ("%d\n", ret_code);
   }
 
   ret_code = read (fd[0], read_buff, 100);
   if (ret_code < 0) {
       printf ("Pipe read is failed!!!\n");
       return -1;
   } else {
       // Expected result should be 5.
       printf ("%d\n", ret_code);
   }
 
   ret_code = write (fd[1], write_buff, 4096);
   if (ret_code < 0) {
       printf ("Pipe write is failed!!!\n");
       return -1;
   } else {
       // Expected result should be 5.
       printf ("%d\n", ret_code);
   }
 
   ret_code = write (fd[1], write_buff, 50);
   if (ret_code < 0) {
       printf ("Pipe write is failed!!!\n");
       return -1;
   } else {
       // Expected result should be 5.
       printf ("%d\n", ret_code);
   }
   // Read from the pipe.
  
  
 
   // Finally simple return.
   return 0;
 
}
