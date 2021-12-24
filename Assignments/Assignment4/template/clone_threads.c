#include<clone_threads.h>
#include<entry.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<mmap.h>

void copyFields(struct exec_context *new_ctx,struct exec_context *ctx){
  
	for(int i=0;i<MAX_MM_SEGS;i++) (new_ctx->mms)[i]=(ctx->mms)[i];
	
	for(int i=0;i<CNAME_MAX;i++) (new_ctx->name)[i]=(ctx->name)[i];
	
	for(int i=0;i<MAX_SIGNALS;i++) new_ctx->sighandlers[i]=ctx->sighandlers[i];
		
	for(int i=0;i<MAX_OPEN_FILES;i++)	new_ctx->files[i]=ctx->files[i];
	
	new_ctx->ppid=ctx->pid;  
	new_ctx->pgd=ctx->pgd;
	new_ctx->vm_area=ctx->vm_area;
	new_ctx->used_mem=ctx->used_mem;
	new_ctx->pending_signal_bitmap=ctx->pending_signal_bitmap;
	new_ctx->ticks_to_alarm=ctx->ticks_to_alarm;
	new_ctx->ticks_to_sleep=ctx->ticks_to_sleep;
	new_ctx->alarm_config_time=ctx->alarm_config_time;
	new_ctx->regs=ctx->regs;

	return;
}

void fix_registers(struct exec_context *new_ctx,void* arg,void* stack,void* th_func){

	new_ctx->regs.entry_rsp=(u64)stack;
	new_ctx->regs.rbp=(u64)stack;
	new_ctx->regs.rdi=(u64)arg;
	new_ctx->regs.entry_rip=(u64)th_func;

	return;
}

long do_clone(void *th_func, void *user_stack, void *user_arg) 
{
  
  struct exec_context *new_ctx = get_new_ctx();
  struct exec_context *ctx = get_current_ctx();

  u32 pid = new_ctx->pid;
  
  if(!ctx->ctx_threads){  // This is the first thread
          ctx->ctx_threads = os_alloc(sizeof(struct ctx_thread_info));
          bzero((char *)ctx->ctx_threads, sizeof(struct ctx_thread_info));
          ctx->ctx_threads->pid = ctx->pid;
  }
     
 /* XXX Do not change anything above. Your implementation goes here*/
  struct thread* unused_thread = find_unused_thread(ctx);
  if(!unused_thread) return -1;

  unused_thread->parent_ctx=ctx;
  unused_thread->status=TH_USED;
  unused_thread->pid=pid;
  
  setup_child_context(new_ctx);
  copyFields(new_ctx,ctx);
  fix_registers(new_ctx,user_arg,user_stack,th_func);

  new_ctx->type = EXEC_CTX_USER_TH;    // Make sure the context type is thread
  new_ctx->state = WAITING;            // For the time being. Remove it as per your need.

  return pid; 
}

void page_table_walk_private_fault(u32 pgd,u64 addr,u64 flags,u64 internal_flags){

	u64 pfn;
	
	u64* temp=(u64*)osmap(pgd);

	if(temp[(addr&PGD_MASK)>>PGD_SHIFT]&0x1){
		pfn=temp[(addr&PGD_MASK)>>PGD_SHIFT]>>PTE_SHIFT;
	}
	else{
		pfn=(u64)os_pfn_alloc(OS_PT_REG);
		temp[(addr&PGD_MASK)>>PGD_SHIFT]=(pfn<< PTE_SHIFT)|internal_flags;
	}
	temp=(u64*)osmap(pfn);

	if(temp[(addr&PUD_MASK)>>PUD_SHIFT]&0x1){
		pfn=temp[(addr&PUD_MASK)>>PUD_SHIFT]>>PTE_SHIFT;
	}
	else{
		pfn=(u64)os_pfn_alloc(OS_PT_REG);
		temp[(addr&PUD_MASK)>>PUD_SHIFT]=(pfn<<PTE_SHIFT)|internal_flags;
	}
	temp=(u64*)osmap(pfn);

	if(temp[(addr&PMD_MASK)>>PMD_SHIFT]&0x1){
		pfn=temp[(addr&PMD_MASK)>>PMD_SHIFT]>>PTE_SHIFT;
	}
	else{
		pfn=(u64)os_pfn_alloc(OS_PT_REG);
		temp[(addr&PMD_MASK)>>PMD_SHIFT]=(pfn<<PTE_SHIFT)|internal_flags;
	}
	temp=(u64*)osmap(pfn);

	pfn=(u64)os_pfn_alloc(USER_REG);
	temp[(addr&PTE_MASK)>>PTE_SHIFT]=(pfn<<PTE_SHIFT)|flags;

	return;
}

/*This is the page fault handler for thread private memory area (allocated using 
 * gmalloc from user space). This should fix the fault as per the rules. If the the 
 * access is legal, the fault handler should fix it and return 1. Otherwise it should
 * invoke segfault_exit and return -1*/
int handle_thread_private_fault(struct exec_context *current, u64 addr, int error_code)
{

	struct exec_context *parent=current;
	if(current->type==EXEC_CTX_USER_TH) parent=get_ctx_by_pid(current->ppid);

	struct thread_private_map *map=NULL;

	for(int i=0;i<MAX_THREADS;i++)
	{
		if(parent->ctx_threads->threads[i].status==TH_USED){
	
			struct thread_private_map *curr=&parent->ctx_threads->threads[i].private_mappings[0]; 
			for(int j=0;j<MAX_PRIVATE_AREAS;j++){
				if(curr->owner){
					if(curr->start_addr<=addr&&addr<=curr->start_addr+curr->length-1){
						map=curr;
						break;	
					}
				}
				curr++;
			}
		}
	}

	//error condition
	if(!map){
		segfault_exit(current->pid, current->regs.entry_rip, addr);
		return -1;
	}

  	u64 flags;

	if(map->owner->pid==current->pid||parent->ctx_threads->pid==current->pid||(map->flags&TP_SIBLINGS_RDWR)) flags=0x7;
	else if(map->flags&TP_SIBLINGS_RDONLY) flags=0x5;
	else if(map->flags&TP_SIBLINGS_NOACCESS){
		segfault_exit(current->pid,current->regs.entry_rip,addr);
		return -1;
	}
  
	if((error_code==0x4)||((error_code==0x6)&&(flags&0x2))) page_table_walk_private_fault(current->pgd,addr,flags,0x7);
	else
	{
		segfault_exit(current->pid, current->regs.entry_rip, addr);
		return -1;
	}

  	return 1;
}

void page_table_walk_ctxswitch(u32 pgd,u64 addr,u64 flags){

	u64 pfn;
	u64* temp=(u64*)osmap(pgd);

	if(temp[(addr&PGD_MASK)>>PGD_SHIFT]&0x1){
		pfn=temp[(addr&PGD_MASK)>>PGD_SHIFT]>>PTE_SHIFT;
		temp=(u64*)osmap(pfn);
	}
	else return;

	if(temp[(addr&PUD_MASK)>>PUD_SHIFT]&0x1){
		pfn=temp[(addr&PUD_MASK)>>PUD_SHIFT]>>PTE_SHIFT;
		temp=(u64*)osmap(pfn);
	}
	else return;

	if(temp[(addr&PMD_MASK)>>PMD_SHIFT]&0x1){
		pfn=temp[(addr&PMD_MASK)>>PMD_SHIFT]>>PTE_SHIFT;
		temp=(u64*)osmap(pfn);
	}
	else return;

	if(temp[(addr&PTE_MASK)>>PTE_SHIFT]&0x1){
		pfn=temp[(addr&PTE_MASK)>>PTE_SHIFT]>>PTE_SHIFT;
		temp[(addr&PTE_MASK)>>PTE_SHIFT]=(pfn<<PTE_SHIFT)|flags;
	}

	return;
}

/*This is a handler called from scheduler. The 'current' refers to the outgoing context and the 'next' 
 * is the incoming context. Both of them can be either the parent process or one of the threads, but only
 * one of them can be the process (as we are having a system with a single user process). This handler
 * should apply the mapping rules passed in the gmalloc calls. */


int handle_private_ctxswitch(struct exec_context *current, struct exec_context *next)
{
  
   /* your implementation goes here*/
	struct exec_context* parent=current;
	if(current->type==EXEC_CTX_USER_TH) parent=get_ctx_by_pid(current->ppid);

	u64 flags=0;
	
	for(int i=0;i<MAX_THREADS;i++){

		if(parent->ctx_threads->threads[i].status==TH_USED){
			
			for(int j=0;j<MAX_PRIVATE_AREAS;j++){

				if(parent->ctx_threads->threads[i].private_mappings[j].owner){
					
					if(parent->ctx_threads->threads[i].private_mappings[j].owner->pid==next->pid||parent->ctx_threads->pid==next->pid) flags=0x7;
					else if(parent->ctx_threads->threads[i].private_mappings[j].flags&TP_SIBLINGS_RDWR) flags=0x7;
					else if(parent->ctx_threads->threads[i].private_mappings[j].flags&TP_SIBLINGS_NOACCESS) flags=0x1;
					else if(parent->ctx_threads->threads[i].private_mappings[j].flags&TP_SIBLINGS_RDONLY) flags=0x5;

					for(u64 vaddr=0;vaddr<parent->ctx_threads->threads[i].private_mappings[j].length;vaddr+=4096){
						page_table_walk_ctxswitch(next->pgd,parent->ctx_threads->threads[i].private_mappings[j].start_addr+vaddr,flags);
						asm volatile("invlpg (%0);" ::"r" (parent->ctx_threads->threads[i].private_mappings[j].start_addr+vaddr) : "memory");
					}
				}	
			}
		}
	}

   return 0;	
}

