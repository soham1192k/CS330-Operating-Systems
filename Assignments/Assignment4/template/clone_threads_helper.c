#include<clone_threads.h>
#include<entry.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<mmap.h>

/*
    XXX  You may or may not use any of these function. They either implement some callbacks for
    exit/mmap handling or provide some helpers. You *MUST NOT* modify the following implementation.
    IF YOU NEED AN ALTERNATE IMPLEMENTATION DEFINE IT IN clone_threads.c AND USE IT!
*/

//Call when an access is illegal
void segfault_exit(int pid, u64 rip, u64 addr){
     printk("Segfault for [%d] @%x for address %x. Exiting\n", pid, rip, addr);
     do_exit(0);
}

//Finds a unused thread for the parent context
struct thread* find_unused_thread(struct exec_context *ctx)
{
   int i;
   struct thread *th;
   struct ctx_thread_info *tinfo = ctx->ctx_threads;
   if(ctx->type != EXEC_CTX_USER || !tinfo)
          return NULL;
   
   for(i=0; i<MAX_THREADS; ++i){
        th = &tinfo->threads[i];
        if(th->status == TH_UNUSED)
             return th;
   } 
   return NULL;
}
//Finds a thread given its pid, ctx must be the parent context

struct thread* find_thread_from_pid(struct exec_context *ctx, int thread_pid)
{
   int i;
   struct thread *th;
   struct ctx_thread_info *tinfo = ctx->ctx_threads;
   if(ctx->type != EXEC_CTX_USER || !tinfo)
          return NULL;
   
   for(i=0; i<MAX_THREADS; ++i){
        th = &tinfo->threads[i];
        if(th->pid == thread_pid)
             return th;
   } 
   return NULL;
}

//Called when thread exits. normal=0 means its an exit not because of
//user space invocation
void handle_thread_exit(struct exec_context *ctx, u8 normal)
{
   int ctr;
   struct exec_context *parent = get_ctx_by_pid(ctx->ppid);
   struct thread *th = find_thread_from_pid(parent, ctx->pid);
   struct thread_private_map *thmap;
   if(!th){
               printk("BUG!!! Exiting thread is not accounted!\n");
               return;
   }   
   
   //We unmap the areas owned by the thread when it exits
   thmap = &th->private_mappings[0]; 
   for(ctr=0; ctr<MAX_PRIVATE_AREAS; ++ctr, thmap++){
        if(thmap->owner == th)
              vm_area_unmap(ctx, thmap->start_addr, thmap->length);
   }
   th->status = TH_UNUSED;
   if(!normal)
        ctx->state = EXITING;
   else
        ctx->state = UNUSED;
   parent->state = READY;
   dprintk("Thread %d is exiting\n", ctx->pid);
   return;
}

//Called when the process exits. Cleans up all threads of the process
void cleanup_all_threads(struct exec_context *ctx)
{
   int i;
   struct thread *th;	 
   struct ctx_thread_info *tinfo = ctx->ctx_threads;
   if(!isProcess(ctx) || !tinfo)
          return;
   
   for(i=0; i<MAX_THREADS; ++i){
        th = &tinfo->threads[i];
        if(th->status == TH_USED){
		 struct exec_context *thctx = get_ctx_by_pid(th->pid);
                 handle_thread_exit(thctx, 1);
	}
   }	
   ctx->state = UNUSED;   // Hack as its made ready while calling handle thread exit
}

//Find a 'free' thread private map meta-data struct
static struct thread_private_map *find_unused_map(struct thread *th)
{
 int ctr;
 struct thread_private_map *thmap = &th->private_mappings[0];
 for(ctr = 0; ctr < MAX_PRIVATE_AREAS; ++ctr, thmap++){
      if(!thmap->owner)
            return thmap;
 } 
 return NULL;
}

//Find the thread private map meta-data for the given address. The address should be
// an exact match. If length=0, length is not matched else it is matched with the 
// previously recored information.
// NOTE:  The range check is not done here. You should implement your version in 
// clone_threads.c to check if an address is in range of a thread_private_map
 
static struct thread_private_map *find_thmap(struct thread *th, u64 address, u32 length)
{
 int ctr;
 struct thread_private_map *thmap = &th->private_mappings[0];
 for(ctr = 0; ctr < MAX_PRIVATE_AREAS; ++ctr, thmap++){
      if(thmap->owner && thmap->start_addr == address && (!length || thmap->length == length))
               return thmap;
 } 
 return NULL;
}
/*
 Assume that addr and length pair will be same across map and unmap.
 'prot' is a bit-wise OR of all  PROT_* flags defined in include/mmap.h 
 Called internally when an mmap with MAP_TH_PRIVATE is invoked from the 
 user space.
*/

int handle_thread_private_mmap(u64 addr, u32 length, u32 prot)
{
  struct exec_context *current = get_current_ctx();
  struct exec_context *parent; 
  struct thread *th;
  struct thread_private_map *thmap;
  
  if(!isThread(current))    //this type of mmap allowed only from threads
       return -1;
  parent = get_ctx_by_pid(current->ppid);

  th =  find_thread_from_pid(parent, current->pid); 
  if(!th || th->pid != current->pid){
        return -1;
  }
  thmap = find_unused_map(th);
  if(!thmap)
       return -1;
  // Now note down the mapping details  
  thmap->start_addr = addr;
  thmap->length = length;
  thmap->flags = prot;
  thmap->owner = th;
  return 0;
}

struct thread* find_thread_from_address(struct exec_context *ctx, u64 addr, u32 length, struct thread_private_map **thmap)
{
   int i;
   struct thread *th;
   struct thread_private_map *map = NULL;
   
   struct ctx_thread_info *tinfo = ctx->ctx_threads;

   if(!isProcess(ctx) || !tinfo)
          return NULL;
   
   for(i=0; i<MAX_THREADS; ++i){
        th = &tinfo->threads[i];
        if(th->status == TH_USED && (map = find_thmap(th, addr, length))){
               *thmap = map;
               return th;        
        }
   } 
 return NULL;
}
/*
*  The owner thread or the process can unmap, others should not be 
*  allowed to unmap. The length and address should be an exact match
*  This is called internally to realize gfree.
*/

void handle_thread_private_unmmap(u64 addr, u32 length)
{
  struct thread *th;
  struct exec_context *current = get_current_ctx();
  struct exec_context *parent = get_ctx_by_pid(current->ppid);
  struct thread_private_map *thmap = NULL;
  
  if(isProcess(current)){
        th = find_thread_from_address(current, addr, length, &thmap);
        if(!th) // It is not of our interest
              return;
  }else{
        th =  find_thread_from_pid(parent, current->pid); 
        if(!th || th->pid != current->pid)
               segfault_exit(current->pid, current->regs.entry_rip, addr);
  }
  if(!thmap && !(thmap = find_thmap(th, addr, length)))
          segfault_exit(current->pid, current->regs.entry_rip, addr);
  if(thmap->owner != th) 
          segfault_exit(current->pid, current->regs.entry_rip, addr);
  thmap->owner = NULL;
  /*No need to cleanup page table as the unmap anyway will do that*/
  return;
}

/*
   Returns 0 if all is well. Else returns -EINVAL
   must be called before gthread_create returns.
 */
long do_make_thread_ready(long pid) {
     struct exec_context *ctx = get_ctx_by_pid(pid);
     if(ctx->type != EXEC_CTX_USER_TH || ctx->state == EXITING || ctx->state == UNUSED)
            return -EINVAL;
     ctx->state = READY;
     return 0;
}

/*
   Wait for thread is called by parent to implement gthread_join.
 */

long do_wait_for_thread(long pid)
{
  struct exec_context *thctx = get_ctx_by_pid(pid);
  struct exec_context *ctx = get_current_ctx();
  if(thctx->type != EXEC_CTX_USER_TH || thctx->state == UNUSED || thctx->state == EXITING || thctx->ppid != ctx->pid) {
        if(thctx->state == EXITING)
             thctx->state = UNUSED;
        return -EINVAL;
  }
  ctx->regs.rax = 0;
  ctx->state = WAITING;
  thctx = pick_next_context(ctx);
  schedule(thctx);  
  return 0;
}

