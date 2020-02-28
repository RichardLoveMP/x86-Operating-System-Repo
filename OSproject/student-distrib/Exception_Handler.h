//  Interrupt_Descriptor_Table.h
//  Created by kaiwei on 2019/10/18.
//  Copyright © 2019 kaiwei. All rights reserved.
//

#ifndef EXECPTION_HANDLER_H
#define EXECPTION_HANDLER_H

#include "lib.h"

void DIVIDE_ERROR_EXCEPTION();//printk("Divided by ZERO.\n");

void DEBUG_EXCEPTION();//printk("Trap or Fault.\n");

void NMI_EXCEPTION();//printk("Non-maskable Interrupt.\n");

void BREAKPOINT_EXCEPTION();//printk("A breakpoint instruction (INT 3) was executed, causing a breakpoint trap to be generated.\n");
void OVERFLOW_EXCEPTION();// printk("An overflow trap occurred.\n");
    
    /* You  can add  your code here to do what you want */
    /* .... */


void BOUNDS_RANGE_EXCEEDED_EXCEPTION();// printk("A BOUND-range-exceeded fault occurred when a BOUND instruction was executed. \n");
    


void INVALID_OPCODE_EXCEPTION();// printk("Invalid Opcode.\n");


void DEVICE_NOT_AVAILABLE_EXCEPTION();// printk("Device-not-available.\n");

//void DOUBLE_FAULT_EXCEPTION(unsigned int err_code);//printk("The processor detected a second exception while calling an exception handler for a prior exception.\n");
void DOUBLE_FAULT_EXCEPTION();

void COPROCESSOR_SEGMENT_OVERRUN_EXCEPTION();// printk("Detected a page or segment violation while transferring the middle portion of an Intel 387 math coprocessor operand. \n");
    

//void INVALID_TSS_EXCEPTION(unsigned int err_code);//printk("There was an error related to a TSS\n");
void INVALID_TSS_EXCEPTION();

//void SEGMENT_NOT_PRESENT_EXCEPTION(unsigned int err_code);//printk("The present flag of a segment or gate descriptor is clear\n");
void SEGMENT_NOT_PRESENT_EXCEPTION();

//void STACK_FAULT_EXCEPTION(unsigned int err_code);//printk("Stack Fault.\n");
void STACK_FAULT_EXCEPTION();

//void GENERAL_PROTECTION_EXCEPTION(unsigned int err_code);// printk("The processor detected one of a class of protection violations called “general- protection violations.”\n");
void GENERAL_PROTECTION_EXCEPTION();

//void PAGE_FAULT_EXCEPTION(unsigned int err_code);//printk("Page-Fault.\n");
void PAGE_FAULT_EXCEPTION();


//Interrupt 15 - RESERVED BY INTEL
void x87FLOAT_POINT_EXCEPTION();//printk("The x87 FPU has detected a floating-point error\n");


//void ALIGNMENT_CHECK_EXCEPTION(unsigned int err_code);//printk("The processor detected an unaligned memory operand when alignment checking was enabled.\n");
void ALIGNMENT_CHECK_EXCEPTION();

void MACHINE_CHECK_EXCEPTION();// printk("The processor detected an internal machine error or a bus error, or that an external agent detected a bus error.\n");


void SIMD_FLOATING_POINT_EXCEPTION();// printk("The processor has detected an SSE/SSE2/SSE3 SIMD floating-point exception.\n");
    



#endif
