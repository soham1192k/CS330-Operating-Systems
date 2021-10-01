#ifndef __MSG_QUEUE_H_
#define __MSG_QUEUE_H_

#include <types.h>
#include <context.h>

#define MAX_TXT_SIZE 24 
#define MAX_MEMBERS 4 
#define BROADCAST_PID 0xffffffff 

struct message{

	u32 from_pid;
	u32 to_pid;
	
	// will be a null terminated string
	char msg_txt[MAX_TXT_SIZE];
};

struct msg_queue_info{
	
	u32 member_count;
	u32 member_pid[MAX_MEMBERS];

	// bitmask of blocked processes for members
	u32 blocked[MAX_MEMBERS];

	// msgs pending for each members 
	u32 msg_count[MAX_MEMBERS];

	// offsets for each members buffers
	u32 read_pos[MAX_MEMBERS];
	u32 write_pos[MAX_MEMBERS];
	
	// pointer to buffer
	struct message *msg_buffer;
};

struct msg_queue_member_info{
	u32 member_count;
	u32 member_pid[MAX_MEMBERS];
};

extern int do_create_msg_queue(struct exec_context *ctx);
extern void do_add_child_to_msg_queue(struct exec_context *child_ctx);
extern void do_msg_queue_cleanup(struct exec_context *ctx);
extern int do_msg_queue_get_member_info(struct exec_context *ctx, struct file *filep, struct msg_queue_member_info *info);
extern int do_msg_queue_send(struct exec_context *ctx, struct file *filep, struct message *msg);
extern int do_msg_queue_rcv(struct exec_context *ctx, struct file *filep, struct message *msg);
extern int do_get_msg_count(struct exec_context *ctx, struct file *filep);
extern int do_msg_queue_block(struct exec_context *ctx, struct file *filep, int pid);
extern int do_msg_queue_close(struct exec_context *ctx, int fd);
#endif
