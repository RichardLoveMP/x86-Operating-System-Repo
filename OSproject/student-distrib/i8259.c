/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

#define MAX_IRQ_NUMBER 8
#define MASK_IRQ_NUMBER 0x07
/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */
static uint8_t irq_mask_master;
static uint8_t irq_mask_slave;
static unsigned int master_slave_IR;
static unsigned int master_eoi;
static unsigned int slave_eoi;


/* void i8259_init(void)
 * Function : Initialize the 8259 PIC 
 * Inputs: none
 * Outputs: none
 * Side Effects: none
 */
void i8259_init(void) {
	//unsigned long flags;
		/* mask interrupt for both master and slave */
        outb(MASK_ALL_IRQ, MASTER_8259_PORT + 1);     
		outb(MASK_ALL_IRQ, SLAVE_8259_PORT + 1);
        
		/* set ICW for master PIC */
		outb(ICW1, MASTER_8259_PORT);     /* set PIC to initialization mode goes to port MASTER_8259_PORT */        
		outb(ICW2_MASTER, MASTER_8259_PORT+1); /* send ICW2, goes to port MASTER_8259_PORT+1 */        
		outb(ICW3_MASTER, MASTER_8259_PORT+1); /* send ICW3, goes to port MASTER_8259_PORT+1 */ 
		outb(ICW4, MASTER_8259_PORT+1);     /* send ICW4, goes to port MASTER_8259_PORT+1 */
		
		/* set ICW for slave PIC */
        outb(ICW1, SLAVE_8259_PORT);     /* set PIC to initialization mode goes to port SLAVE_8259_PORT */        
		outb(ICW2_SLAVE, SLAVE_8259_PORT+1); /* send ICW2, goes to port SLAVE_8259_PORT+1 */        
		outb(ICW3_SLAVE, SLAVE_8259_PORT+1);     /* send ICW3, goes to port SLAVE_8259_PORT+1 */       
		outb(ICW4, SLAVE_8259_PORT+1);     /* send ICW4, goes to port SLAVE_8259_PORT+1 */    
		
       /* unmask both master and slave PIC */
        outb(MASK_ALL_IRQ, MASTER_8259_PORT+1);          
		outb(MASK_ALL_IRQ, SLAVE_8259_PORT+1);  
		
		irq_mask_master = MASK_ALL_IRQ;
		irq_mask_slave = MASK_ALL_IRQ;
		master_slave_IR = ICW3_SLAVE;
		//printf("pic initialized, %x, %x\n",irq_mask_master,irq_mask_slave);
}

/* enable_irq(uint32_t irq_num) 
 * Function : enable irq with given irq number
 * Inputs: irq_num: irq number
 * Outputs: none
 * Side Effects: enable irq with given irq number
 */
void enable_irq(uint32_t irq_num) {
	
	if (irq_num >= MAX_IRQ_NUMBER) {
		irq_mask_slave = irq_mask_slave & ~(0x01<<(irq_num - MAX_IRQ_NUMBER));
		outb(irq_mask_slave, SLAVE_8259_PORT+1); 
	}
	else {
		irq_mask_master = irq_mask_master & ~(0x01<<irq_num);
		outb(irq_mask_master, MASTER_8259_PORT+1); 
	}
	//printf("irq %d enabled, %x, %x\n",irq_num,irq_mask_master,irq_mask_slave);
}


/* void disable_irq(uint32_t irq_num)
 * Function : Disable (mask) the specified IRQ 
 * Inputs: irq_num: irq number
 * Outputs: none
 * Side Effects: Disable (mask) the specified IRQ 
 */
void disable_irq(uint32_t irq_num) {
	
	if (irq_num >= MAX_IRQ_NUMBER) {
		irq_mask_slave = irq_mask_slave | (0x01<<(irq_num - I8259_8));
		outb(irq_mask_slave, SLAVE_8259_PORT+1); 
	}
	else {
		irq_mask_master = irq_mask_master | (0x01<<irq_num);
		outb(irq_mask_master, MASTER_8259_PORT+1); 
	}
	//printf("irq %d disabled, %x, %x\n",irq_num,irq_mask_master,irq_mask_slave);
}

/* Send end-of-interrupt signal for the specified IRQ */

/* void send_eoi(uint32_t irq_num)
 * Function : send an end of interrupt to PIC
 * Inputs: irq_num: irq number
 * Outputs: none
 * Side Effects: send an end of interrupt to PIC
 */
void send_eoi(uint32_t irq_num) {
	if (irq_num >= MAX_IRQ_NUMBER) {
		slave_eoi = EOI + (irq_num & I8259_BITMASK); //take the lower 3 bits of irq number irq from 0 to 7
		master_eoi = EOI + master_slave_IR;	//master connected to slave through master_slave_IR
		/*send eoi*/
		outb(slave_eoi, SLAVE_8259_PORT); 
		outb(master_eoi, MASTER_8259_PORT);
	}
	else {
		//if irq is on the master PIC
		master_eoi = EOI + irq_num;
		outb(master_eoi, MASTER_8259_PORT); 	//send a EOI to master PIC
	}
}

