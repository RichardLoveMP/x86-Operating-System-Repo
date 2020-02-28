#include "scheduler.h"


/**
 * init_scheduler
 *   DESCRIPTION: initialize the scheduler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: create 1 page directory and 1 page table, map the 1st entry
*/
void init_scheduler(){
    scheduler.head = NULL;
    scheduler.tail = NULL;
    scheduler.currently_process = NULL;
    scheduler.number_of_process = 0;
    
}



/**
 * enqueue
 *   DESCRIPTION: enqueue a process into the running queue
 *   INPUTS: pcb: the process control block
 *   OUTPUTS: the same pcb as input
 *   RETURN VALUE: the same pcb as input
 *   SIDE EFFECTS: enqueue the input pcb at the tail of current runing queue
*/
// a queue... so yes, we are going to have enqueue and dequeue! 
pcb_t * enqueue(pcb_t * pcb){
    //printf("enqueued %d, %s\n",(int)pcb, pcb->program_name);
    // sanity check: if pcb doesn't exist, or we have reached the maximum number of 
    // available process, return NULL for failure.
    if (pcb == NULL) {  
        return NULL;    
    }
    if (scheduler.number_of_process >= MAXIMUM_PROGREM_NUM){
       
       // printf("You have tooooo many processes!!");
        return NULL;
    }


  
    // if the scheduler is empty, add it to the tail of the scheduler...
    if (scheduler.head == NULL){
        scheduler.head = pcb;
        scheduler.tail = pcb;
        scheduler.currently_process = pcb; 
        // update next & prev
        scheduler.head->next = NULL;
        scheduler.tail->prev = NULL;
    } else {
        // general cases (not empty)
        scheduler.currently_process = scheduler.head;
        scheduler.tail->next = pcb;
        pcb->prev = scheduler.tail;
        pcb->next = NULL;
        scheduler.tail = pcb;
    }
    // general process
    scheduler.number_of_process += 1;
    pcb->state = PCB_STATE_RUNNING;
    //scheduler.currently_process = pcb;   
    //("pcb of %d, %s\n",scheduler.number_of_process, scheduler.head->program_name);
    return pcb;
}



/**
 * remove_arbitrary
 *   DESCRIPTION: remove arbitrary process from the running queue
 *   INPUTS: pcb: the process control block
 *   OUTPUTS: the same pcb as input
 *   RETURN VALUE: the same pcb as input if sucess or NULL if not found
 *   SIDE EFFECTS: find and remove the input pcb at the tail of current runing queue
*/
pcb_t * remove_arbitrary(pcb_t * pcb){
    // sanity check (you can't remove a null ptr nor remove elements from an empty list)
    if (pcb == NULL || scheduler.number_of_process == 0){
        return NULL;
    }

    pcb_t * current = pcb;
    // check there exists such a pcb... If doesn't exist, return null...
    
    if (current->prev == NULL && current->next == NULL && scheduler.head==pcb) {
        // only one pcb
        scheduler.head = NULL;
        scheduler.tail = NULL;
    } else if (current->prev == NULL && current->next != NULL && scheduler.head==pcb){
        // on the head
        scheduler.head->next->prev = NULL; 
        scheduler.head = current->next;
        current->next = NULL;
    } else if (current->prev != NULL && current->next == NULL && scheduler.tail==pcb){
        // to be deleted on the tail
        scheduler.tail->prev->next = NULL;
        scheduler.tail = current->prev;
        current->prev = NULL;
    } else if (current->prev != NULL && current->next != NULL){
        // to be deleted in the middle
        current->prev->next = current->next;
        current->next->prev = current->prev;
        current->prev = NULL;
        current->next = NULL;
    }
    scheduler.number_of_process--;

    return pcb;

}




/**
 * renew_runqueue
 *   DESCRIPTION: rotate the run queue
 *   INPUTS: None
 *   OUTPUTS: 0 for success
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: rotate the runqueue
*/
int32_t renew_runqueue(){
    // sanity check 
    if (scheduler.head == NULL || scheduler.tail == NULL){
        // empty
        return -1;
    }
    if (scheduler.head == scheduler.tail){
        // only one
        return 0;
    }
    // general cases
    scheduler.tail->next = scheduler.head;
    scheduler.head->prev = scheduler.tail;
    scheduler.head = scheduler.head->next;
    scheduler.tail = scheduler.tail->next;
    scheduler.tail->next = NULL;
    scheduler.head->prev = NULL;

    return 0;
}


/**
 * update_pcb_state
 *   DESCRIPTION: change the pcb state
 *   INPUTS: pcb 
 *   OUTPUTS: 0 for success
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: rotate the runqueue
*/
/*
void update_pcb_state(pcb_t * pcb, int dir) {
    if (dir>0) {
        if (scheduler.head == scheduler.tail){
            pcb->state = PCB_STATE_RUNNING;
            return;
        }
        pcb->state = PCB_STATE_RUNNING;
        pcb->parent_pcb_ptr->state = PCB_STATE_SLEEPING;
    }
    else {
        if (scheduler.head == scheduler.tail){
            pcb->state = PCB_STATE_SLEEPING;
            return;
        }
        pcb->state = PCB_STATE_SLEEPING;
        pcb->parent_pcb_ptr->state = PCB_STATE_RUNNING;
    }
}*/


/**
 * runqueue_get_next_process_pid
 *   DESCRIPTION: get next pcb in the run queue
 *   INPUTS: pointer to current pcb
 *   OUTPUTS: None
 *   RETURN VALUE: next process's pid for success, NUll for no element in the runqueue
 *   SIDE EFFECTS: rotate the runqueue
*/
int runqueue_get_next_process_pid(pcb_t* pcb) {
    while (scheduler.head->state==PCB_STATE_SLEEPING) {
        renew_runqueue();
    }
    //printf("pcb of %d, %s\n",scheduler.number_of_process, scheduler.head->program_name);
    return scheduler.head->pid;
}


