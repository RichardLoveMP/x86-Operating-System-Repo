#include "system_call.h"

#include "rtc.h"
#include "ROfilesystem.h"
#include "terminal.h"
#include "paging.h"
#include "lib.h"
#include "pit.h"
#include "lib.h"
#include "x86_desc.h"
#include "scheduler.h"


#define TWOOOO 2

#define MB8 0x800000
#define SIZE128MB 0x8000000
#define KB8 0x2000
#define SIZE4KB 0x1000
#define KERNEL_PHYSICAL 0x400000
#define VIDEO_BASE 0xB8
#define VIDEO 0xB8000
#define SIZE4MB 0x400000

#define RTC_DISPATCH 1
#define DIR_DISPATCH 2
#define FILE_DISPATCH 3
#define LOOP_DATA 10
#define VIDMAP_FOUR 4
#define SYSCALL_EXE_FOUR 4

#define SYSCALL_LOOP_SIZE 3

#define ADDRESS_BIAS_8MB     0x800000
#define ADDRESS_BIAS_8KB     0x2000
#define GET_PID_MASK 0x1FFF

#define PSIZZEE 1024*4

#define SHIFT12 12

#define SYSCALL_DISPATCH_SIZE 4

#define PIT_FREQ 	75

#define PAGE_VGA_BACKUP_ADDRESS 0xB9

#define CLOSE_ST 2
#define CLOSE_ED 8

#define USER_STAT_NUM 10

uint8_t ece391bootup=1;
uint8_t haltall=0;


char command_name[USER_COMMAND_LENGTH];
char user_args[USER_COMMAND_LENGTH];
char user_program_name[USER_PROGRAME_NAME];




fops_t rtc_ops = {open_rtc, close_rtc,read_rtc, write_rtc};
fops_t file_ops = {file_open, file_close, file_read, file_write};
fops_t dir_ops = {directory_open, directory_close, directory_read, directory_write};
fops_t term_ops = {terminal_open, terminal_close, terminal_read, terminal_write};

int check_dispatch[SYSCALL_DISPATCH_SIZE] = {M1,M2,M3,M4};

static uint8_t processes_availability[NUM_MAX_PROCESS]={0};
static dentry_t current_exe; 
/* 
 *  halt
 *   DESCRIPTION: The halt system call terminates a proess
 *   INPUTS: status
 *   OUTPUTS: 
 *   RETURN VALUE:
 *   SIDE EFFECTS: NONE
 */
int32_t halt(uint8_t status) {
	int32_t ret = -1;
	uint32_t dat; 
	/* get the pcb */
	pcb_t *pcb = get_PCB();
	
	/*remove from run queue*/
	remove_arbitrary(pcb);
	pcb->state = PCB_STATE_SLEEPING;

	pcb_t *parent = pcb->parent_pcb_ptr;
	if (parent != NULL) {
		parent->state = PCB_STATE_RUNNING;
	}
	

	cli();

	/* set free pid */
	PidRelease(pcb->pid);
	// if its pid equals to zero
	if(pcb->pid == 0){	// halt the process
		terminal[pcb->terminal_belong_to].isbootup=0;
		ece391bootup = 1;
		haltall=1;
		printf("You can't terminate the last terminal on First Terminal!\n");
		sti();
		execute((uint8_t*)"shell");
	}
	else if(parent==NULL){
		haltall=1;	// hait it
		terminal[pcb->terminal_belong_to].isbootup=0;
		open_terminal_index = pcb->terminal_belong_to;	// open terminal index
		printf("You can't terminate the last terminal on %d th Terminal!\n",pcb->terminal_belong_to);
		sti();
 		execute((uint8_t*)"shell");
 	}

	/*if(parent->terminal_belong_to != pcb->terminal_belong_to){
		printf("halt in %s\n",pcb->program_name);
		isbootup[pcb->terminal_belong_to]=0;
		open_terminal_index = parent->terminal_belong_to;
		haltall=1;
		//printf("Cannot Halt Terminal %d\n",pcb->terminal_belong_to);
		//sti();
 		//execute((uint8_t*)"shell");
		//terminal_switch(parent->terminal_belong_to);
 	}*/

	// call close file in the system call
	int closenum = CLOSE_ST;
	for(; closenum < CLOSE_ED; closenum++)
		close(closenum);

	terminal[pcb->terminal_belong_to].apid = parent->pid;
	//parent->state = PCB_STATE_RUNNING;

	// set tss
	dat = (MB8 - KB8 * (parent->pid));
	SET_TS(&tss,dat);

	page_table_array[parent->pid].page_table[VIDEO_BASE].page_base_address = 
    page_table_array[pcb->pid].page_table[VIDEO_BASE].page_base_address;

	// make the CR3 setting
	CR3_setting((uint32_t*)page_directory_array[parent->pid].page_directory);

	asm volatile(	  		
		"movl %0, %%esi;"   
	:	
	:	"g"((uint32_t)status)
	:	"edi","ebp","esp"
	);
	/* asm volatile language */
	asm volatile(
		"pushl 	%%ebp;"
		"movl 	%%esp,%%ebp;"
		"movl 	%0, %%ebx;"
		"movl	%1, %%esp;"
		"movl	%2, %%ebp;"
		"pushl	%%ebx;"

		"sti;"

		"ret;"
	:
	:	"g"(parent->eip),"g"(parent->esp),"g"(parent->ebp)
	:	"ebx","esp","ebp"	
	);

	return ret;
}

/* 
 *  execute
 *   DESCRIPTION: execute a user program or shell (syscall)
 *   INPUTS: command which will be executed
 *   OUTPUTS: NONE
 *   RETURN VALUE: Correct - 0 Exit Incorrect: 1-256
 *   SIDE EFFECTS: change the related data structure
 */
int32_t execute(const uint8_t* command){
	//if(!strncmp(command, "ls",2)) return -1;
	//printf("our exuecte command is %s\n",command);
	if(command == NULL)
		return -1;

	if(check_address_available(command,strlen((const int8_t*)command)) == -1)
		return -1;
	
	char file_sup[FOURTY];
	uint32_t dat;
	inode_t* inode_of_execute;
	int i = 0;
	int flag = 0;
	uint32_t ce;	// child eip
	int last_position = 0;
	//rtc_init_with_frequency(32);
	//int j = 0;
	//copy user space variable command into kernal space
	char command_copy[USER_COMMAND_LENGTH];
	memset(command_copy, 0, USER_COMMAND_LENGTH);
	memcpy(command_copy, (char*)command, strlen((char*)command));

	// idle the pcb
    int pid_idle = getPidIdle();
    if(pid_idle == -1)
    	return -1;

    dat = MB8-pid_idle*KB8;

	pcb_t *pcb = get_PCB();	// get the current pcb

	// new pcb
	pcb_t *npcb =  getProcPCB(pid_idle);	// get the process pcb
	npcb->pid = pid_idle;
	

//	cli();
	if(pid_idle != 0 && whichterminal == -1) {	//if the pcb is idle
		npcb->parent_pcb_ptr = pcb;
	}
	if (pid_idle != 0 && open_terminal_index!=-1) {
		npcb->parent_pcb_ptr = NULL;
	}
	
	//if (pcb->terminal_belong_to != processing_terminal) {
	//	npcb->parent_pcb_ptr = NULL;
	//}
	
	if(pid_idle==0 )	// if the pcb is not idle
		npcb->parent_pcb_ptr = NULL;


	// initial the pcb
	initial_pcb(npcb);

	if(ece391bootup==1 || open_terminal_index!=-1){
		memcpy((void*)command_copy, (void*)command, strlen((int8_t*)command));
		ece391bootup = TWOOOO;
	}
	else{
		if(!copy_from_user((void*)command_copy, (void*)command, strlen((int8_t*)command))){
			PidRelease(pid_idle);
			return -1;
		}
	}

	if(pid_idle==0){
		terminal[pid_idle].isbootup = 1;
	}

	else if(open_terminal_index!=-1){
		terminal[open_terminal_index].isbootup = 1;
	}

	//initialize command with zero
	memset(command_name, 0, USER_COMMAND_LENGTH);
	memset(user_args, 0, USER_COMMAND_LENGTH);
	memset(user_program_name, 0, USER_PROGRAME_NAME);
	memset(npcb->args, 0, USER_COMMAND_LENGTH);
	memset(npcb->program_name, 0, USER_PROGRAME_NAME);

	// get the file's latest position
	last_position = get_user_program_name(user_program_name, (const char*)command_copy);
	strcpy(npcb->program_name, user_program_name);
	//printf("get program names: %s|\n", (char*)user_program_name);
	// get the last position
	if (last_position!=strlen((const char*)command_copy))
		memcpy(user_args, command_copy+last_position, strlen((const char*)command_copy)-(last_position));
	//printf("user_args: %s|\n", (char*)user_args);
	int start = 0;
	while (user_args[start]==' ') {
		start++;
	}

	memcpy(npcb->args, user_args+start, strlen(user_args)-start);
	//printf("npcb->args: %s|\n", (char*)npcb->args);
	int temp = strlen(npcb->args)-1;
	while (npcb->args[temp]=='\0' || npcb->args[temp]==' ') {
		if (npcb->args[temp]==' ') {
			npcb->args[temp]='\0';
		}
		temp--;
		if(temp < 0)
			break;
	}
	
	//printf("get args: %s|\n", (char*)npcb->args);

	//reading file info
	if((read_dentry_by_name((uint8_t*)user_program_name, &current_exe) == -1) || (read_data(current_exe.inode_num, 0, (uint8_t*)file_sup, FOURTY) == -1)){
		PidRelease(pid_idle);
		return -1;
	}

	/* check the file */
	for(i = 0; i < SYSCALL_LOOP_SIZE; i++){
		if(file_sup[i] != check_dispatch[i]){	// check the file
			PidRelease(pid_idle);	// if something is wrong, set it free
			return -1;
		}
	}

	// check the execute process
	ce =*((uint32_t*)(file_sup+OFFSET_H));

	//enqueue npcb
	npcb->state = PCB_STATE_RUNNING;
	enqueue(npcb);


	// scheduler process

	if (npcb->parent_pcb_ptr != NULL) {
		npcb->parent_pcb_ptr->state = PCB_STATE_SLEEPING;
	}

	if(ece391bootup == TWOOOO || ece391bootup == 1) {
		pit_init(PIT_FREQ);
		ece391bootup = 0;
	}


	// copy the page directory and page table array

	if(open_terminal_index==-1 && pid_idle!=0){
		memset((void*)page_directory_array[pid_idle].page_directory,0,PSIZZEE);
		memset((void*)page_table_array[pid_idle].page_table,0,PSIZZEE);
	}

	//reset open_terminal_index
	if(pid_idle==0){
		npcb->terminal_belong_to=0;
	}
	else if(open_terminal_index!=-1){
		npcb->terminal_belong_to = open_terminal_index;	//here
		//npcb->parent_pcb_ptr = NULL;
	}
	else{
		npcb->terminal_belong_to = pcb->terminal_belong_to;
	}



	/* mapping the paging */
	SETPAGE(pid_idle);


	// back up the video mem
	page_table_array[pid_idle].page_table[PAGE_VGA_BACKUP_ADDRESS].page_base_address = (VIDEO + (npcb->terminal_belong_to + 1)*SIZE4KB) >> SHIFT12;
	page_table_array[pid_idle].page_table[PAGE_VGA_BACKUP_ADDRESS].present = 1;
	page_table_array[pid_idle].page_table[PAGE_VGA_BACKUP_ADDRESS].user_or_supervisor = 0;
	page_table_array[pid_idle].page_table[PAGE_VGA_BACKUP_ADDRESS].read_or_write = 1;
	CR3_setting((uint32_t*)page_directory_array[pid_idle].page_directory);

	  //update active process
  terminal[npcb->terminal_belong_to].apid = npcb->pid;

	//resetting open_terminal_index and preparing video memory
	if(open_terminal_index!=-1 || pid_idle==0){
		//clear video memory
		if(haltall == 0){
			int dx = terminal[pcb->terminal_belong_to].terminal_cursor[0];	// update the terminal cursor
			int dy = terminal[pcb->terminal_belong_to].terminal_cursor[1];
			terminal_clear();
			terminal[pcb->terminal_belong_to].terminal_cursor[0] = dx;	// update the terminal cursor
			terminal[pcb->terminal_belong_to].terminal_cursor[1] = dy;
		}else{
			haltall=0;
		}
		// update the open terminal index
		open_terminal_index = -1;
	}
 

	// find the inode of the execute
	inode_of_execute = (inode_t*)(bootblockptr+current_exe.inode_num+1);

	// get the read data 
	if(read_data(current_exe.inode_num, 0, (void*) PROG_START_VIR_ADD, inode_of_execute->length))
		flag = 1;

	// set tss value
	SET_TSS(&tss,dat);

	//set DS, save esp, ebp for halting
	asm volatile(
		"movw %w2, %%ds;"   	
		"movl %%esp, %0;" 
		"movl %%ebp, %1;" 
		"pushl %2;"
		: "=g" (pcb->esp), "=g" (pcb->ebp)		
		: "r" (USERR_DSEG)
		: "memory"       
	); 
	
	asm volatile(
		"pushl %0;"
		"pushfl;"
		"pushl %1;"
		"pushl %2;"
		"pushl %2;"
		:
		:"r"((SIZE128MB+SIZE4MB-4)), "r" (T23), "r" (ce)	// Data segment
		:"memory", "cc"
	);
	
	
	ret_handler(&(pcb->eip));	// handle the ret function
	
	
	uint32_t stat;	// return the status

	asm volatile(
	
		"movl %%esi, %0;"
		:"=r" (stat)
		:
		:"esi"
	);
	// if something is wrong 
	if(stat == FIFTM)
		return TSS;
	return 0;
}


/* 
 *  read
 *   DESCRIPTION: read the file described by descriptor to the buffer
 *   INPUTS: file descriptor, buffer and bytes to be read
 *   OUTPUTS: how many bytes are read successfully
 *   RETURN VALUE: return value of the target process
 *   SIDE EFFECTS: NONE
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){

	if(fd < 0 || fd > ENGT)
		return -1;
	if(fd==1)//not read for stdout
		return -1;

	if(buf == NULL)
		return -1;
	//get pcb pointer
	pcb_t *pcb = get_PCB();
	if(pcb->fds[fd].busy==0)
		return -1;

	char k_buf[MAX_CHARACTERS];	//create temporay buf 
	memset(k_buf, 0, MAX_CHARACTERS);

	int32_t ret = pcb->fds[fd].ops_dispatch->_read(fd, k_buf, nbytes);
	copy_to_user(buf, k_buf, nbytes);
	
	//printf("read: ----%s,\n%d|||\n",buf,ret);
	return ret;
}

/* 
 *  write
 *   DESCRIPTION: write the file described by descriptor to the buffer
 *   INPUTS: file descriptor, buffer and bytes to be written
 *   OUTPUTS: how many bytes are written successfully
 *   RETURN VALUE: return value of the target process
 *   SIDE EFFECTS: NONE
 */
int32_t write(int32_t fd,  void* buf, int32_t nbytes){
	//printf("cnt is %d",nbytes);
	//printf("\nwrite: ----\n%s|||\n",buf);
	if(fd<0 || fd > ENGT )
		return -1;

	if(fd==0)  	//cannot write to stdin
		return -1;
	if(buf == NULL)
		return -1;
	//get pcb pointer
	pcb_t *pcb = get_PCB();

	if(pcb->fds[fd].busy==0)
		return -1;

	char k_buf[MAX_CHARACTERS];	//create temporay buf 
	memset(k_buf, 0, MAX_CHARACTERS);
	copy_from_user(k_buf, buf, nbytes);
	int32_t ret = pcb->fds[fd].ops_dispatch->_write(fd, k_buf, nbytes);

	//printf("write: ----\n%s\n|||\n",buf);
	return ret;
}

/* 
 *  open
 *   DESCRIPTION: Open a file described by filename
 *   INPUTS: pointer to filename
 *   OUTPUTS: -1 for fail, 0 for correct
 *   RETURN VALUE: -1 or correct
 *   SIDE EFFECTS: NONE
 */
int32_t open(const uint8_t* filename){
		// parameter set
		uint32_t i;	// possible index of PCB
		uint32_t j;	// possible index
	
		int rtc_flag = 0;	//detect whether it belongs to rtc operation
		int dir_flag = 0;	//detect whether it belongs to directory operation

		//get pcb pointer
		pcb_t *pcb = get_PCB();	/* get current pcb pointer */
		
		/* find an idle location for PCB */
		i = get_pcb_idle(pcb);

		//if there is no idle process control block 
		if(i>=ENGT)
			return -1;
		
		/* if the filename begins with point */
		if((uint8_t) filename[0] == '.'){	
			file_operation_dispatch(pcb,i,0,0,DIR_DISPATCH);	
			dir_flag = 1;	/* directory flag = 1 */
		}
		/* if it belongs to directory operation, return 1 */
		if(dir_flag)
			return i;

		/* judge whether it belongs to the rtc */
		for(j = 0; j < THREE; j++){
			if((uint8_t)filename[0] != 'r'){	/* first character */
				break;
			}
			if((uint8_t)filename[1] != 't'){	/* second character */
				break;
			}
			if((uint8_t)filename[2] != 'c'){	/* third character */
				break;
			}

			/* rtc_flag */
			rtc_flag = 1;

			/* get the rtc operations */	
			file_operation_dispatch(pcb,i,0,-1,RTC_DISPATCH);
			break;
		}

		/* rtc flag is used */
		if(rtc_flag)
			return i;

		/* file test */
		dentry_t file_dentry;
		int ret;
		// if fail return 
		ret = read_dentry_by_name((uint8_t*)filename,&file_dentry);
		if(ret == -1)
			return ret;
		// file operation
		
		file_operation_dispatch(pcb,i,file_dentry.inode_num,0,FILE_DISPATCH);
		return i;
	
}

/* 
 *  close
 *   DESCRIPTION: Close the fd
 *   INPUTS: fd
 *   OUTPUTS: 0
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: NONE
 */
int32_t close(int32_t fd){
	if(fd<=1 || fd>=ENGT)
		return -1;
	//get pcb pointer
	pcb_t *pcb = get_PCB();

	// if this file is already closed
	if(pcb->fds[fd].busy==0)
		return -1;

	// close the file
	pcb->fds[fd].busy=0;
	return 0;
}

/* 
 *   getargs
 *   DESCRIPTION: get the correct argument from the terminal
 *   INPUTS: storage buffer and length
 *   OUTPUTS: 0
 *   RETURN VALUE: 0 for success, -1 for failure
 *   SIDE EFFECTS: NONE
 */
int32_t getargs (uint8_t* buf, int32_t nbytes){
	// sanity check
	if(buf == NULL)
		return -1;
	int i = 0;
	int flag = 0;
	// flag check
	/*
	int flag = 0;
	int i = 0;
	// check the problem of spaces
	
	cli();
	for(i = 0; i < nbytes; i++)
		nputc(buf[i]);
	for(i = 0;i < nbytes; i++)
		if(buf[i] != ' ' && buf[i] != 0 )
			flag = 1;
	
	if(flag == 0){
		sti();
		return -1;
	
	}

	sti();
	*/
	pcb_t *pcb = get_PCB();
	if(strlen(pcb->args) > nbytes-1) 		// check the strlen
	{
		memset(buf, 0, nbytes);
		printf("length of command overpass length of the buf--- program name: %s, args:%s\n",pcb->program_name, pcb->args);
		return -1;
	}
	char k_buf[MAX_CHARACTERS];	//create temporay buf 
	memset(k_buf, 0, MAX_CHARACTERS);

	if(!copy_to_user((void*)k_buf, (void*)pcb->args, strlen(pcb->args)+1))	// copy progress
	{
		memset(buf, 0, nbytes);	// memset progress
		printf("copy to user failed--- program name: %s, args:%s\n",pcb->program_name, pcb->args);
		return -1;
	}

	copy_to_user(buf, k_buf, nbytes);
	// sanity check and trailing spaces
	for(i = 0;i < nbytes; i++)
		if(buf[i] != ' ' && buf[i] != 0 )
			flag = 1;
	
	if(flag == 0){
		return -1;
	}

	//printf("getarg: %s|\n", buf);

	return 0;
}

/* 
 *   vidmap
 *   DESCRIPTION: maps the text-mode viedo memory into user space at a pre-set
 *   INPUTS: (uint8_t** )screen_start
 *   OUTPUTS: (SIZE128MB+SIZE4MB)	//video memory is at position 132MB
 *   RETURN VALUE: video memory position
 *   SIDE EFFECTS: modify the page directory
 */
int32_t vidmap (uint8_t** screen_start){
	if (screen_start==NULL)
	//check for NULL pointer to pointer
		return -1;
	
	pcb_t *pcb = get_PCB();
	if (check_address_available(screen_start, VIDMAP_FOUR)) {
		//printf("video memory failed--- program name: %s, args:%s\n",pcb->program_name, pcb->args);
		return -1;	//invalid user program address
	}
	*screen_start = (uint8_t*) (SIZE128MB+SIZE4MB);
	int pd = NUM_MAX_PROCESS + pcb->pid;

	//mapping to physical memory 132MB

	page_directory_array[pcb->pid].page_directory[PARFN].user_or_supervisor = 1;
	page_directory_array[pcb->pid].page_directory[PARFN].read_or_write = 1;
	page_directory_array[pcb->pid].page_directory[PARFN].present = 1;
	page_directory_array[pcb->pid].page_directory[PARFN].page_size = 0;
	page_directory_array[pcb->pid].page_directory[PARFN].page_base_address = ((uint32_t) (&page_table_array[pd].page_table)) >> OFFSET_TWELVE;	

	page_table_array[pd].page_table[0].present = 1;
	page_table_array[pd].page_table[0].user_or_supervisor = 1;
	page_table_array[pd].page_table[0].read_or_write = 1;
	page_table_array[pd].page_table[0].page_base_address = VIDEO >> OFFSET_TWELVE;
	
	return (SIZE128MB+SIZE4MB);	//video memory is at position 132MB
}

/* 
 *  set_handler
 *   DESCRIPTION: extra credit
 *   INPUTS:
 *   OUTPUTS: 
 *   RETURN VALUE:
 *   SIDE EFFECTS: 
 */
int32_t set_handler(int32_t signum, void* handler_address){
	// To be continued
	return -1;
}

/* 
 *  sigreturn
 *   DESCRIPTION: extra credit
 *   INPUTS:
 *   OUTPUTS: 
 *   RETURN VALUE:
 *   SIDE EFFECTS: 
 */
int32_t sigreturn(void){
	// To be continued
	return -1;
}


/* 
 *   DESCRIPTION: this fuction returns a free pid. if no such pid exist, return -1 .
 *                  It tries to get the smallest available pid.
 *   INPUTS: NONE
 *   OUTPUTS: NONE
 *   RETURN VALUE: -1 or pid
 *   SIDE EFFECTS: NONE
 */
int getPidIdle(){         
    int i;
    for (i=0; i<NUM_MAX_PROCESS; i++) {
        cli();
        if (processes_availability[i] == 0){
			// reset the process
            processes_availability[i] = 1;
			processnum ++;
            sti();
            break;
        }
        sti();
    }
    // loop all, no available
    if (NUM_MAX_PROCESS == i){
            return -1;
    } else {
        return i;
    }
}


/* 
 * getVisiblePID
 *   DESCRIPTION: gets the PID of the process running in the visible terminal
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: Pointer to the requested PCB
 *   SIDE EFFECTS: none 
 */
pcb_t* getVisiblePCB(){
  uint32_t pid = terminal[processing_terminal].apid;
	return (pcb_t*)(MB8-(pid+1)*KB8);
}


/*   PidRelease
 *   DESCRIPTION: this fuction get the input pid freed
 *   INPUTS: pid   the pid to be free
 *   OUTPUTS: none
 *   RETURN VALUE: -1 / -2 /0, 0 for success
 *   SIDE EFFECTS: no. Note taht this fuc doesn't handle the reuse of the resources,
 *                 it only cleans the in-use mask!
 */

int PidRelease(int pid) {  
    cli();
    // validation check
    if (pid >= NUM_MAX_PROCESS){
        sti();
        return -1;
    }
    if (pid < 0){
   //     sti();
        sti();
        return -1;
    }
    if (processes_availability[pid] == 0) {
        sti();
        return -2;
    }
    else{
    // start here!
   // if (processes_availability[pid] != 0) {
        processes_availability[pid] = 0;
		processnum --;
        sti();
        return 0;
    }
}


/* 	 get_PCB
 *   DESCRIPTION: get the current program's pid.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: pid
 *   SIDE EFFECTS: no
 */
pcb_t* get_PCB() {   
    int32_t temp;
    asm volatile(
        "movl   %%ebp, %0"
        : "=g" (temp)
        :
        : "cc", "memory"
    );
    temp = ~temp;
    temp = temp | GET_PID_MASK;
    temp = ~temp;

    return  (pcb_t*) temp;
}


/* initial_pcb
 *   DESCRIPTION: initialize the given program's pid.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: reset the given pcb
 */
void initial_pcb (pcb_t* pcb) {
	//init file descriptor array
	int i = 0;
	for(i=0;i<FILEDESCRIPTOR_SIZE;i++)
	{
		pcb->fds[i].location = 0;
		pcb->fds[i].busy = 0;
		pcb->fds[i].ops_dispatch = NULL;
		pcb->fds[i].inode = 0;
	}

	//fill stdin & stdout file descriptors
	pcb->fds[0].ops_dispatch=&term_ops;
	pcb->fds[1].ops_dispatch=&term_ops;

	//setting busy of stdout, stdin top 1
	pcb->fds[0].busy=1;
	pcb->fds[1].busy=1;
}


/* getProcPCB
 *   DESCRIPTION: get a pid's address
 *   INPUTS: pid
 *   OUTPUTS: none
 *   RETURN VALUE: the address of the specificed pid
 *   SIDE EFFECTS: no
 */
pcb_t* getProcPCB(uint8_t pid){     // TODO:gn
    int32_t retval = ADDRESS_BIAS_8MB - pid * ADDRESS_BIAS_8KB - ADDRESS_BIAS_8KB;
	return (pcb_t*) retval;
}


/* get_pcb_idle
 *	 DESCRIPTION: get an idle pcb's index
 *   INPUTS: current pcb
 *   OUTPUTS: index of pcb which is idle
 *   RETURN VALUE: the index of specific pcb
 *   SIDE EFFECTS: no
*/

int32_t get_pcb_idle(pcb_t* para_pcb){
	int32_t i = 0;
	//finding empty file discriptor
	for(i=0;i<ENGT;i++){
		if(para_pcb->fds[i].busy==0)
			break;
	}
	return i;
}

/* file_operation_dispatch
 *	 DESCRIPTION: handle the file opeartion
 *   INPUTS: element of the file_descriptor_t
 *   OUTPUT: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: set the file opeartion's value
 */
void file_operation_dispatch(pcb_t* pcb, int i, uint32_t ino,uint32_t filep, uint32_t op_num){
	pcb->fds[i].inode = ino;	/* set the parameter */
	pcb->fds[i].busy= 1;
	pcb->fds[i].location = filep;

	/* if it belongs to rtc operations */
	if(op_num == RTC_DISPATCH){
		pcb->fds[i].ops_dispatch = &rtc_ops;
	}

	/* if it belongs to directory operations */
	if(op_num == DIR_DISPATCH){
		pcb->fds[i].ops_dispatch = &dir_ops;
	}

	/* if it belongs to file operations */
	if(op_num == FILE_DISPATCH){
		pcb->fds[i].ops_dispatch = &file_ops;
	}

	return ;
}

/* save_user_status
 *	 DESCRIPTION: save the user status (some registers into pcb)
 *   INPUTS: pcb_t* pcb, int32_t* statusbuf
 *   OUTPUT: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: modify the status value in a certain pcb
 */
void save_user_status(pcb_t* pcb, int32_t* statusbuf){
	int i = 0;
	for(i = 0 ; i < USER_STAT_NUM; i++){
		pcb->user_stat[i] = statusbuf[i];
	}
	return ;
}

/* save_user_status
 *	 DESCRIPTION: get the user status (some registers into pcb)
 *   INPUTS: int32_t* statusbuf, tss_t* ctss
 *   OUTPUT: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: save the status value into pcb from a certain current tss
 */
void get_user_status(int32_t* statusbuf, tss_t* ctss){
	int idx = 0;
	statusbuf[idx++] = ctss->eip;
  	statusbuf[idx++] = ctss->eflags;
  	statusbuf[idx++] = ctss->eax;
  	statusbuf[idx++] = ctss->ecx;
  	statusbuf[idx++] = ctss->edx;
  	statusbuf[idx++] = ctss->ebx;
  	statusbuf[idx++] = ctss->esp;
  	statusbuf[idx++] = ctss->ebp;
  	statusbuf[idx++] = ctss->esi;
  	statusbuf[idx++] = ctss->edi;
	return ;
}

/* load_user_status
 *	 DESCRIPTION: get the user status (some registers into pcb)
 *   INPUTS: pcb_t* pcb, tss_t* ctss
 *   OUTPUT: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: modify the status value in a certain current tss
 */
void load_user_status(pcb_t* pcb, tss_t* ctss){
  int idx = 0;
  ctss->eip = pcb->user_stat[idx++];   
  ctss->eflags = pcb->user_stat[idx++];
  ctss->eax = pcb->user_stat[idx++];
  ctss->ecx = pcb->user_stat[idx++];
  ctss->edx = pcb->user_stat[idx++];
  ctss->ebx = pcb->user_stat[idx++];
  ctss->esp = pcb->user_stat[idx++];
  ctss->ebp = pcb->user_stat[idx++];
  ctss->esi = pcb->user_stat[idx++];
  ctss->edi = pcb->user_stat[idx++];
  return ;
}


