#ifndef _SCHE_H
#define _SCHE_H

#include "system_call.h"   // TODO:
#include "lib.h"
#include "terminal.h"
#include "paging.h"

#define MAXIMUM_PROGREM_NUM 6
#define _4_KB 0x1000
#define THERTY_THR 33
#define TWELVE 12
#define _8_MB 0x800000
#define _8_KB 0x2000

#define VIDEO 0xB8000 //duplication. in system_call.c

#define PCB_STATE_RUNNING 1
#define PCB_STATE_SLEEPING 0


// the date struct of a run queue. Used for cp5
typedef struct runqueue_t
{
    pcb_t* head;
    pcb_t* tail;
    pcb_t* currently_process;
    uint32_t number_of_process; // number of programs currently in the scheduler. 6

} runqueue_t;

// the scheduler (or runqueue) of the program
runqueue_t scheduler;

// initialization of the scheduler
extern void init_scheduler();

// a queue... so yes, we are going to have enqueue and dequeue! TODO: 这里可能有重名
extern pcb_t* enqueue(pcb_t * pcb);
//extern pcb_t* dequeue(pcb_t * pcb);  // TODO: 这个好像没用 

void context_switch(uint32_t eip, uint32_t esp, uint32_t ebp, uint32_t* prev_eip);

// When a process is no longer used... This function will enable you to remove an
// arbitrary element in the scheduler.
extern pcb_t* remove_arbitrary(pcb_t* pcb);

// renew 相当于version5里面的rotate. 返回0代表成功，-1代表失败
extern int32_t renew_runqueue();

extern void update_pcb_state(pcb_t* pcb, int dir);\

extern int runqueue_get_next_process_pid(pcb_t* pcb);

#endif
