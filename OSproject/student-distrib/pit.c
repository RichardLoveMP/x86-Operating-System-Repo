#include "pit.h"
#include "i8259.h"
#include "terminal.h"


// Credit http://www.osdever.net/bkerndev/Docs/pit.htm
/* init_pit
 *   DESCRIPTION: Initialize the pit
 *   INPUTS: The input frequency
 *   OUTPUTS: NONE
 *   RETURN VALUE: always 0 for success
 *   SIDE EFFECTS: Integrate the freq into the pit
 */ 
int32_t pit_init(int freq){
   
  outb(PIT_INIT_CMD, PIT_2);             
  outb((PIT_BASE_FREQ / freq) & PITBITMASK, PIT_1);   
  outb((PIT_BASE_FREQ / freq) >> PIT8, PIT_1);      
  enable_irq(PIT_IRQ);
  return 0;
}

/* pit_handler
 *   DESCRIPTION: handle the pit 
 *   INPUTS:  NONE
 *   OUTPUTS:  NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: send end of interrput signal to PIT
 */ 
void pit_handler(){
  send_eoi(PIT_IRQ);
  while(iswriting == 1);  // lock it
  context_switch_terminal();
  return ;
}



