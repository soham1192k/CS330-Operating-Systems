#include <gthread.h>
#include <ulib.h>

static struct process_thread_info tinfo __attribute__((section(".user_data"))) = {};
/*XXX 
      Do not modifiy anything above this line. The global variable tinfo maintains user
      level accounting of threads. Refer gthread.h for definition of related structs.
 */

void exit_helper_function(){
	void* return_val;
	asm volatile("mov %%rax, %0" : "=r" (return_val):);
	gthread_exit(return_val);
	return;
}

/* Returns 0 on success and -1 on failure */
/* Here you can use helper system call "make_thread_ready" for your implementation */
int gthread_create(int *tid, void *(*fc)(void *), void *arg) {
        
	void *allotted_stack=mmap(NULL,TH_STACK_SIZE,PROT_READ|PROT_WRITE,0);
	
	//error check
	if(allotted_stack==NULL||allotted_stack==MAP_ERR) return -1;
	
	u64* allotted_stack_base=(u64*)(((u64)allotted_stack)+TH_STACK_SIZE-8);
	*allotted_stack_base=(u64)(&exit_helper_function);
	
	//call the actual clone syscall
	long thread_pid=clone(fc,(u64)allotted_stack_base,arg);

	//error check
	if(thread_pid<=0) return -1;

	int curr_idx=0;

	//loop over all threads and find some unused one
	int found=0;
	for(int i=0;i<MAX_THREADS;i++){

		if(tinfo.threads[i].pid==0){
			found=1;
			*tid=tinfo.num_threads;
			tinfo.threads[i].tid=tinfo.num_threads;
			tinfo.num_threads+=1;
			tinfo.threads[i].pid=thread_pid;
			tinfo.threads[i].status=TH_STATUS_ALIVE;
			tinfo.threads[i].stack_addr=allotted_stack;
			break;
		}

	}

	if(found==0) return -1;

	//use make thread ready helper function
	make_thread_ready(thread_pid);

	return thread_pid;
}

int gthread_exit(void *retval) {
	
	for(int i=0;i<MAX_THREADS;i++){
		if(tinfo.threads[i].pid==getpid()&&tinfo.threads[i].status!=TH_STATUS_SEMI_ALIVE){
			tinfo.threads[i].status=TH_STATUS_SEMI_ALIVE;
			tinfo.threads[i].ret_addr=retval;
			break;
		}
	}

	//make sure you call exit()
	exit(0);
}

void* gthread_join(int tid) {

	int curr_idx = 0;
	for(int i=0;i<MAX_THREADS;i++){
		if(tinfo.threads[i].tid==tid){
			curr_idx=i;
			break;
		}
	}

	int val=0;

	while(tinfo.threads[curr_idx].status!=TH_STATUS_SEMI_ALIVE){
		if(val<0) break;
		val=wait_for_thread(tinfo.threads[curr_idx].pid);
	}

	int ret=munmap(tinfo.threads[curr_idx].stack_addr,TH_STACK_SIZE);
	if(ret<0) return NULL;

	tinfo.num_threads-=1;
	tinfo.threads[curr_idx].tid=-1;
	tinfo.threads[curr_idx].pid=0;
	tinfo.threads[curr_idx].status=TH_STATUS_UNUSED;
	tinfo.threads[curr_idx].stack_addr=NULL;
	void* final=tinfo.threads[curr_idx].ret_addr;
	tinfo.threads[curr_idx].ret_addr=NULL; 

	return final;
}

/*Only threads will invoke this. No need to check if its a process
 * The allocation size is always < GALLOC_MAX and flags can be one
 * of the alloc flags (GALLOC_*) defined in gthread.h. Need to 
 * invoke mmap using the proper protection flags (for prot param to mmap)
 * and MAP_TH_PRIVATE as the flag param of mmap. The mmap call will be 
 * handled by handle_thread_private_map in the OS.
 * */
void* gmalloc(u32 size, u8 alloc_flag)
{
	void* starting_addr;
	
	u32 f=PROT_READ|PROT_WRITE;
	
	if(alloc_flag!=GALLOC_OTRDONLY&&alloc_flag!=GALLOC_OTRDWR&&alloc_flag!=GALLOC_OWNONLY)
		return NULL;
	else{

		if(alloc_flag==GALLOC_OWNONLY) f|=TP_SIBLINGS_NOACCESS;
		else if(alloc_flag==GALLOC_OTRDONLY) f|=TP_SIBLINGS_RDONLY;
		else if(alloc_flag==GALLOC_OTRDWR) f|=TP_SIBLINGS_RDWR;
		
		starting_addr=mmap(NULL,size,f,MAP_TH_PRIVATE);
	}
	
	int thread_index=-1;
	for(int i=0;i<MAX_THREADS;i++){
		if(getpid()==tinfo.threads[i].pid&&tinfo.threads[i].status!=TH_STATUS_SEMI_ALIVE){
			thread_index=i;
			break;
		}
	}

	if(thread_index==-1) return NULL;

	int priv_area_index=-1;
	for(int i=0;i<MAX_GALLOC_AREAS;i++){
		if(tinfo.threads[thread_index].priv_areas[i].owner==NULL){
			priv_area_index=i;
			break;
		}
	}

	if(priv_area_index==-1) return NULL;

	tinfo.threads[thread_index].priv_areas[priv_area_index].start=(u64)starting_addr;
	tinfo.threads[thread_index].priv_areas[priv_area_index].length=size;
	tinfo.threads[thread_index].priv_areas[priv_area_index].flags=f;
	tinfo.threads[thread_index].priv_areas[priv_area_index].owner=&tinfo.threads[thread_index];

	return starting_addr;
}

/*
   Only threads will invoke this. No need to check if the caller is a process.
*/
int gfree(void *ptr)
{
	if(!ptr) return -1;

	int thread_index=-1;
	for(int i=0;i<MAX_THREADS;i++){
		if(getpid()==tinfo.threads[i].pid&&tinfo.threads[i].status!=TH_STATUS_SEMI_ALIVE){
			thread_index=i;
			break;
		}
	}

	if(thread_index==-1) return -1;

	int priv_area_index=-1;
	for(int i=0;i<MAX_GALLOC_AREAS;i++){
		if(tinfo.threads[thread_index].priv_areas[i].start==(u64)ptr){
			priv_area_index=i;
			break;
		}
	}
	
	if(priv_area_index==-1) return -1;

	int retval=munmap(ptr,tinfo.threads[thread_index].priv_areas[priv_area_index].length);
	if(retval<0) return -1;

	tinfo.threads[thread_index].priv_areas[priv_area_index].start=0;
	tinfo.threads[thread_index].priv_areas[priv_area_index].length=0;
	tinfo.threads[thread_index].priv_areas[priv_area_index].flags=0;
	tinfo.threads[thread_index].priv_areas[priv_area_index].owner=NULL;

	return 0;
}
