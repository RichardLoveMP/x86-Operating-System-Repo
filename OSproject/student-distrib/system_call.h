#ifndef _SYS_CALL_H
#define _SYS_CALL_H

#include "types.h"
#include "lib.h"
#include "ROfilesystem.h"
#include "terminal.h"
#include "x86_desc.h"
#include "ret_handler.h"



#define FILEDESCRIPTOR_SIZE 8
#define NUM_MAX_PROCESS 6
#define PROG_START_VIR_ADD 0x08048000
#define MAX_CHARACTERS 1024
#define FOURTY 40

#define USER_COMMAND_LENGTH 300
#define USER_PROGRAME_NAME 32
#define OFFSET_H 24

#define M1 0x7F
#define M2 0x45
#define M3 0x4C
#define M4 0x46

#define MINUS3 -3

#define STP 0x8000000
#define ENDP 0x400000
#define T23 0x23
#define T2B 0x2B
#define FIFTM 15
#define TSS 256

#define ENGT 8
#define FOUR 4

#define PARFN 33

#define USERR_DSEG 0x2B

#define THREE 3
typedef struct fops{
	int32_t (*_open)(const uint8_t*);
	int32_t (*_close)(int32_t);
	int32_t (*_read)(int32_t, void*,int32_t );
	int32_t (*_write)(int32_t, void*,int32_t);
} fops_t;


typedef struct file_descriptor{
	uint32_t busy;
    uint32_t inode;
	fops_t* ops_dispatch;
	uint32_t location;
} file_descriptor_t;

typedef struct pcb pcb_t;

struct pcb{
	uint8_t pid;
	uint32_t eip;
    uint32_t esp;
	uint32_t ebp;
	int state;
	pcb_t* next;
	pcb_t* prev;
	pcb_t* parent_pcb_ptr;
	file_descriptor_t fds[FILEDESCRIPTOR_SIZE];
	char program_name[USER_PROGRAME_NAME];
	char args[USER_COMMAND_LENGTH];
    int terminal_belong_to;

	uint32_t user_stat[10];

};


int getPidIdle();
int PidRelease(int pid);
pcb_t* get_PCB();
pcb_t* get_addr_pid(uint8_t pid);
pcb_t* getVisiblePCB();

extern int32_t halt(uint8_t status);
extern int32_t execute (const uint8_t* command);
extern uint8_t ece391bootup;

extern int32_t read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t write (int32_t fd,  void* buf, int32_t nbytes);
extern int32_t open (const uint8_t* filename);
extern int32_t close (int32_t fd);
extern int32_t getargs (uint8_t* buf, int32_t nbytes);
extern int32_t vidmap (uint8_t** screen_start);
extern int32_t set_handler (int32_t signum, void* handler_address);
extern int32_t sigreturn (void);
int32_t get_pcb_idle(pcb_t* para_pcb);
void initial_pcb (pcb_t* pcb);
pcb_t* getProcPCB(uint8_t pid);
void file_operation_dispatch(pcb_t* pcb, int i, uint32_t ino,uint32_t filep, uint32_t op_num);

extern void get_user_status(int32_t* statusbuf, tss_t* ctss);
void save_user_status(pcb_t* pcb, int32_t* statusbuf);

extern void load_user_status(pcb_t* pcb, tss_t* ctss);

#endif
