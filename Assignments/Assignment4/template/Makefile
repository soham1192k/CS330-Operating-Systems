all: gemOS.kernel
SRCS = main.c lib.c idt.c kbd.c shell.c serial.c memory.c context.c entry.c apic.c schedule.c mmap.c cfork.c page.c fs.c file.c pipe.c clone_threads.c clone_threads_helper.c
OBJS = boot.o main.o lib.o idt.o kbd.o shell.o serial.o memory.o context.o entry.o apic.o schedule.o mmap.o cfork.o page.o  fs.o file.o pipe.o clone_threads.o clone_threads_helper.o
UOBJS = user/init.o user/lib.o user/gthread.o
CFLAGS  = -nostdlib -nostdinc -fno-builtin -fno-stack-protector -fpic -m64 -I./include -I../include 
LDFLAGS = -nostdlib -nodefaultlibs  -q -melf_x86_64 -Tlink64.ld
ASFLAGS = --64  
AS = as 
USER_CFLAGS  = -nostdlib -nostdinc -fno-builtin -fno-stack-protector -fpic -m64 -I./user/


%o: %c
	gcc -c $(CFLAGS) $< -o $@ 

user/%o:user/%c
	gcc -c $(USER_CFLAGS) $< -o $@ 

gemOS.kernel: $(OBJS) $(UOBJS)
	ld $(LDFLAGS) -o $@ $(OBJS) $(UOBJS)

.Phony: clean
clean:
	rm -f clone_threads_helper.o clone_threads.o gemOS.kernel user/*.o;
