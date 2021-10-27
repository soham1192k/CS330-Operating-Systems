#include <debug.h>
#include <context.h>
#include <entry.h>
#include <lib.h>
#include <memory.h>

/*****************************HELPERS******************************************/
//backtrace helper function
void backtrace_helper(struct exec_context* parent, struct exec_context* child,int fromend) {

	int cnt=0;
	u64 base=child->regs.rbp,temp=child->regs.entry_rsp;

	u64 *arr=parent->dbg->trace;
	int end_ret=parent->dbg->retaddr_size-1;

	if(!fromend) arr[cnt++]=child->regs.entry_rip-1;

	while(*(u64*)temp!=END_ADDR) {

		if(*(u64*)temp==(u64)parent->dbg->end_handler){
			arr[cnt++]=parent->dbg->store_retaddr[end_ret--];
			temp=base+8;
			base=*(u64*)base;
		}

		else{
			arr[cnt++]=*(u64*)temp;
			temp=base+8;
			base=*(u64*)base;
		}

	} 

	parent->dbg->trace_size=cnt;
	return ;

}
/*
 * allocate the struct which contains information about debugger
 *
 */
struct debug_info *alloc_debug_info()
{
	struct debug_info *info = (struct debug_info *) os_alloc(sizeof(struct debug_info));
	if(info)
		bzero((char *)info, sizeof(struct debug_info));
	return info;
}

/*
 * frees a debug_info struct
 */
void free_debug_info(struct debug_info *ptr)
{
	if(ptr)
		os_free((void *)ptr, sizeof(struct debug_info));
}

/*
 * allocates a page to store registers structure
 */
struct registers *alloc_regs()
{
	struct registers *info = (struct registers*) os_alloc(sizeof(struct registers));
	if(info)
		bzero((char *)info, sizeof(struct registers));
	return info;
}

/*
 * frees an allocated registers struct
 */
void free_regs(struct registers *ptr)
{
	if(ptr)
		os_free((void *)ptr, sizeof(struct registers));
}

/*
 * allocate a node for breakpoint list
 * which contains information about breakpoint
 */
struct breakpoint_info *alloc_breakpoint_info()
{
	struct breakpoint_info *info = (struct breakpoint_info *)os_alloc(
		sizeof(struct breakpoint_info));
	if(info)
		bzero((char *)info, sizeof(struct breakpoint_info));
	return info;
}

/*
 * frees a node of breakpoint list
 */
void free_breakpoint_info(struct breakpoint_info *ptr)
{
	if(ptr)
		os_free((void *)ptr, sizeof(struct breakpoint_info));
}

/*
 * Fork handler.
 * The child context doesnt need the debug info
 * Set it to NULL
 * The child must go to sleep( ie move to WAIT state)
 * It will be made ready when the debugger calls wait
 */
void debugger_on_fork(struct exec_context *child_ctx)
{
	child_ctx->dbg = NULL;
	child_ctx->state = WAITING;
}
/******************************************************************************/
/* This is the int 0x3 handler
 * Hit from the childs context
 */
long int3_handler(struct exec_context *ctx)
{
	if(!ctx) return -1;

	struct exec_context* par = get_ctx_by_pid(ctx->ppid);
	struct registers p_regs;
	u64 addr = ctx->regs.entry_rip - 1;
	par->regs.rax=addr; 

	if((u64)par->dbg->end_handler!=addr){

		//first run of int3 handler(from breakpoint)
		struct breakpoint_info* temp=par->dbg->head;

		while(temp!=NULL){
			if(temp->addr==addr) break;
			temp=temp->next;
		}

		//copy registers
		p_regs.entry_rip = ctx->regs.entry_rip-1;
		p_regs.entry_cs = ctx->regs.entry_cs;
		p_regs.entry_rsp = ctx->regs.entry_rsp; 
		p_regs.entry_ss = ctx->regs.entry_ss;
		p_regs.entry_rflags = ctx->regs.entry_rflags;
		p_regs.r8 = ctx->regs.r8;
		p_regs.r9 = ctx->regs.r9;
		p_regs.r10 = ctx->regs.r10;
		p_regs.r11 = ctx->regs.r11;
		p_regs.r12 = ctx->regs.r12;
		p_regs.r13 = ctx->regs.r13;
		p_regs.r14 = ctx->regs.r14;
		p_regs.r15 = ctx->regs.r15;
		p_regs.rax = ctx->regs.rax;
		p_regs.rbx = ctx->regs.rbx;
		p_regs.rcx = ctx->regs.rcx;
		p_regs.rdx = ctx->regs.rdx;
		p_regs.rbp = ctx->regs.rbp;
		p_regs.rdi = ctx->regs.rdi;
		p_regs.rsi = ctx->regs.rsi;
		
		par->dbg->regs = p_regs;

		//save backtrace information
		backtrace_helper(par,ctx,0);

		if(temp->end_breakpoint_enable==1){

			par->dbg->call_stack[par->dbg->call_stack_size++] = addr;
			par->dbg->store_retaddr[par->dbg->retaddr_size++]= *(u64 *)(ctx->regs.entry_rsp);
			*(u64*)(ctx->regs.entry_rsp)=(u64)(par->dbg->end_handler);

			//simulate push rbp
			ctx->regs.entry_rsp-=8;
			*(u64*)(ctx->regs.entry_rsp)=ctx->regs.rbp;

		}

		else{
			//simulate push rbp
			ctx->regs.entry_rsp-=8;
			*(u64*)(ctx->regs.entry_rsp)=ctx->regs.rbp;
		}
	}
	else{

		//second run of int3 handler(from end of breakpointed function)
		par->dbg->call_stack[--par->dbg->call_stack_size]=0;

		ctx->regs.entry_rsp-=8;
		*(u64 *)(ctx->regs.entry_rsp) = par->dbg->store_retaddr[--par->dbg->retaddr_size];
		par->dbg->store_retaddr[par->dbg->retaddr_size]=0;

		//copy registers
		p_regs.entry_rip = ctx->regs.entry_rip-1;
		p_regs.entry_cs = ctx->regs.entry_cs;
		p_regs.entry_rsp = ctx->regs.entry_rsp; 
		p_regs.entry_ss = ctx->regs.entry_ss;
		p_regs.entry_rflags = ctx->regs.entry_rflags;
		p_regs.r8 = ctx->regs.r8;
		p_regs.r9 = ctx->regs.r9;
		p_regs.r10 = ctx->regs.r10;
		p_regs.r11 = ctx->regs.r11;
		p_regs.r12 = ctx->regs.r12;
		p_regs.r13 = ctx->regs.r13;
		p_regs.r14 = ctx->regs.r14;
		p_regs.r15 = ctx->regs.r15;
		p_regs.rax = ctx->regs.rax;
		p_regs.rbx = ctx->regs.rbx;
		p_regs.rcx = ctx->regs.rcx;
		p_regs.rdx = ctx->regs.rdx;
		p_regs.rbp = ctx->regs.rbp;
		p_regs.rdi = ctx->regs.rdi;
		p_regs.rsi = ctx->regs.rsi;

		par->dbg->regs = p_regs;

		//save backtrace information
		backtrace_helper(par,ctx,1);

		//simulate pushrbp
		ctx->regs.entry_rsp-=8;
		*(u64*)(ctx->regs.entry_rsp)=ctx->regs.rbp;	
	}

	ctx->state = WAITING;
	par->state = READY;
	schedule(par);

	return 0;
}

/*
 * Exit handler.
 * Deallocate the debug_info struct if its a debugger.
 * Wake up the debugger if its a child
 */
void debugger_on_exit(struct exec_context *ctx)
{
	if(!ctx) return;

	if(!ctx->dbg) { 
		get_ctx_by_pid(ctx->ppid)->state=READY;
		return;
	}
    else {

		struct breakpoint_info* curr=ctx->dbg->head;
		
		while(curr) {
			struct breakpoint_info* info=curr->next;
			curr->next=NULL;
			free_breakpoint_info(curr);
			curr=info;
		}

		free_debug_info(ctx->dbg);
		ctx->dbg=NULL;
		return;
	}
}

/*
 * called from debuggers context
 * initializes debugger state
 */
int do_become_debugger(struct exec_context *ctx, void *addr)
{
	if(!ctx) return -1;
	
	struct debug_info* dbg=ctx->dbg;

	dbg = alloc_debug_info();
	if(!dbg) return -1;

	dbg->head=NULL;
	dbg->breakpoint_num=0;
	dbg->end_handler=addr;
	dbg->breakpoint_count=0;
	dbg->trace_size=0;
	dbg->call_stack_size=0;
	dbg->retaddr_size=0;

	ctx->dbg=dbg;
	
	*(unsigned char*)addr=INT3_OPCODE;

	return 0;
}

/*
 * called from debuggers context
 */
// 1. set_breakpoint on non-breakpointed function present in backtrace with end breakpoint
// 2. set_breakpoint on non-breakpointed function present in backtrace with no end breakpoint
// 3. set_breakpoint on breakpointed function having no end breakpoint present in backtrace to add end breakpoint
// 4. set_breakpoint on breakpointed function having end breakpoint present in backtrace to remove end breakpoint
// 5. set_breakpoint on breakpointed function with same end breakpoint enabled option.
int do_set_breakpoint(struct exec_context *ctx, void *addr, int flag)
{
	if(!ctx||!(ctx->dbg)) return -1;
	
	struct debug_info* info=ctx->dbg;
	struct breakpoint_info* temp = ctx->dbg->head;
	
	//check if present in call stack
	int present_on_call_stack=0;
	
	for (int i=0;i<ctx->dbg->call_stack_size;i++)
	{	
		if(ctx->dbg->call_stack[i]==(u64)addr) present_on_call_stack=1;
	}

	//check if breakpoint already set
	int already_set = 0;
	
	while(temp){
		if(temp->addr==(u64)addr){
			already_set=1;
			break;
		}
		temp=temp->next;
	}

	//not already set
	if(!already_set){

		if(info->breakpoint_count==MAX_BREAKPOINTS) return -1;

		struct breakpoint_info *curr=alloc_breakpoint_info();
		if(!curr) return -1;

		info->breakpoint_num++;
		info->breakpoint_count++;
		
		curr->num=info->breakpoint_num;
		curr->end_breakpoint_enable=flag;
		curr->addr=(u64)addr;
		curr->next=NULL;

		
		*(unsigned char*)addr = INT3_OPCODE;

		if(!info->head) info->head=curr;
		
		else{
			temp=info->head;
			while(temp->next) temp=temp->next;
			temp->next=curr;
		}
	}
	else{
		
		//already set
		if(present_on_call_stack){

			if(temp->end_breakpoint_enable==1&&flag==0) return -1;
			
			else{
				temp->end_breakpoint_enable=flag;
				return 0;
			}
		}

		else{
			temp->end_breakpoint_enable=flag;
			return 0;
		}
	}

	return 0;
}


/*
 * called from debuggers context
 */
// 6. remove_breakpoint on breakpointed function having end breakpoint present in backtrace
// 7. remove_breakpoint on breakpointed function having no end breakpoint present in backtrace
int do_remove_breakpoint(struct exec_context *ctx, void *addr)
{
	if(!ctx||!(ctx->dbg)) return -1;

	struct breakpoint_info *temp=ctx->dbg->head;

	int already_set=0;
	
	while(temp){
		if(temp->addr==(u64)addr){
			already_set=1;
			break;
		}
		temp=temp->next;
	}

	if(!already_set) return -1;
	
	int present_on_call_stack=0;

	for(int i=0;i<ctx->dbg->call_stack_size;i++)
	{
		if(ctx->dbg->call_stack[i]==(u64)addr){
			present_on_call_stack=1;
			break;
		}	
	}

	if(present_on_call_stack&&temp->end_breakpoint_enable) return -1;
	
	*(unsigned char*)addr=PUSHRBP_OPCODE;
	ctx->dbg->breakpoint_count--;
	
	if(temp==ctx->dbg->head){
		ctx->dbg->head=temp->next;
		temp->next=NULL;
		free_breakpoint_info(temp);
	}

	else{
		
		struct breakpoint_info* itr=ctx->dbg->head;
		
		while(itr&&itr->next!=temp){
			itr=itr->next;
		}
		
		itr->next=temp->next;
		temp->next=NULL;
		free_breakpoint_info(temp);
	}

	return 0;
}

/*
 * called from debuggers context
 */
int do_info_breakpoints(struct exec_context *ctx, struct breakpoint *ubp)
{
	if(ctx==NULL||ctx->dbg ==NULL) return -1;

	int count=0;
	struct breakpoint_info* temp=ctx->dbg->head;

	while(temp){
		
		struct breakpoint brkpoint;

		brkpoint.end_breakpoint_enable=temp->end_breakpoint_enable;
		brkpoint.addr=temp->addr;
		brkpoint.num=temp->num;

		ubp[count++]=brkpoint;
		
		temp = temp->next;
	}

	return count;
}

/*
 * called from debuggers context
 */
int do_info_registers(struct exec_context *ctx, struct registers *regs)
{
	if(!ctx||!ctx->dbg) return -1;
	*regs=ctx->dbg->regs;
	return 0;
}

/*
 * Called from debuggers context
 */
int do_backtrace(struct exec_context *ctx, u64 bt_buf)
{
	if(!ctx||!ctx->dbg) return -1;

	u64* ptr=(u64*)bt_buf;

	for(int i=0;i<ctx->dbg->trace_size;i++) ptr[i]=ctx->dbg->trace[i];

	return ctx->dbg->trace_size;
}

/*
 * When the debugger calls wait
 * it must move to WAITING state
 * and its child must move to READY state
 */
s64 do_wait_and_continue(struct exec_context *ctx)
{
	if(!ctx||!ctx->dbg) return -1;
	
    struct exec_context *child_ctx;
	int found=0;

	for(int i=0;i<MAX_PROCESSES;i++){
		child_ctx=get_ctx_by_pid(i);
		if(child_ctx->ppid==ctx->pid) {found=1;break;}
	}

    if(found==0||!child_ctx||child_ctx->ppid!=ctx->pid) return CHILD_EXIT;

    ctx->state=WAITING;
    child_ctx->state=READY;
    schedule(child_ctx);

    return -1;
}






