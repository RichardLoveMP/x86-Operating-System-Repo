//  Interrupt_Descriptor_Table.h
//  Created by kaiwei on 2019/10/18.
//  Copyright © 2019 kaiwei. All rights reserved.
//
#include "Interrupt_Descriptor_Table.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "Exception_Handler.h"
#include "keyboard.h"
#include "rtc.h"
#include "system_call.h"
#include "syscall_helper.h"
#include "pithelper.h"


#define PIT_IDTIDX 0x20
// The IDT will contain entries for execption handlers and hardware interrupt handlers and the system call handlers

/*
First is how to set the five reserve bits:
        execption:                  R0 = 0, R1 = 1, R2 = 1, R3 = 0, R4 = 0
        hard interrupt:             R0 = 0, R1 = 1, R2 = 1, R3 = 1, R4 = 0
        system call:                R0 = 0, R1 = 1, R2 = 1, R3 = 1, R4 = 0

next is the DPL:
    execption interrupt handler    Descriptor Privilege Level (DPL) is 0//kernel
    hardware interrupt handler     Descriptor Privilege Level (DPL) is 0//kernel
    system call handler            Descriptor Privilege Level (DPL) is 3//user
    
for the idt_desc_t:
    uint16_t offset_15_00;    //Offset (15 to 0)// we don't need to change
    uint16_t seg_selector;    //Segment Selector// we should get it to KERNEL_CS
    uint8_t  reserved4;       //Reserved Bit 4//
    uint32_t reserved3 : 1;   //Reserved Bit 3//
    uint32_t reserved2 : 1;   //Reserved Bit 2//
    uint32_t reserved1 : 1;   //Reserved Bit 1//
    uint32_t size      : 1;   //Size of gate (D)// 1 for 32bits and 0 for 16
    uint32_t reserved0 : 1;   //Reserved Bit 0//
    uint32_t dpl       : 2;   //Descriptor Privilege Level// see comment before
    uint32_t present   : 1;   //Present Bit// 1 to utilize // we also not need to change for already being set to 0
    uint16_t offset_31_16;    //Offset (31 to 16)// not need to change
*/
//now we can initialize the idt
/* idt_init()
 * this function will initialize the IDT by change the Interrupt gate stored in idt[] array  also, set the IDT ENTRY 
 * RETURN: NONE
 * OUTPUT: NONE
 * SIDE EFFECT: NONE
 * 
 */ 
void
idt_init(){
    int i;
    for( i=0; i<NUM_VEC;i++){ // loop for all 256
       
        idt[i].seg_selector=KERNEL_CS;
        idt[i].reserved4=0;
        idt[i].reserved2=1;
        idt[i].reserved1=1;
        idt[i].reserved0=0;
        idt[i].size=1;
        idt[i].present=0;   //enable the SET_IDT_ENTRY(
        
        idt[i].dpl=0;
        idt[i].reserved3=0;
        /*
        to do with R3 and dpl here
        */
        if (i<IDT_M1){
             idt[i].reserved3=0;
             idt[i].present=1;
        }
        if (i>=IDT_M2&&i<=IDT_M3){ // if just a hard drive interrupt just change the R3,not change DPl
           //idt[i].reserved3=1;
           idt[i].present=1;
        }
        if(i==SYSTEM_CALL_VAECTOR){//if i ==0x80; the system call port change the DPL and R3
            idt[i].reserved3=1;
            idt[i].dpl=DPL3;
            idt[i].present=1;
      
        }
        
        
    }

	

// if we need , we should add & before func (SET_IDT_ENTRY(idt[0], &func);)
// also still need to know in the func use printf() or printk()
/* Sets offset(31 to 16) and (15 to 0) for an IDT entry, also it is defined in IA32.pdf in 5.14 */
     /* Define INT 0x00 through INT 0x18. Route them to respective handlers. */
    SET_IDT_ENTRY(idt[0], DIVIDE_ERROR_EXCEPTION);
    SET_IDT_ENTRY(idt[1], DEBUG_EXCEPTION);
    SET_IDT_ENTRY(idt[2], NMI_EXCEPTION);
    SET_IDT_ENTRY(idt[3], BREAKPOINT_EXCEPTION);
    SET_IDT_ENTRY(idt[4], OVERFLOW_EXCEPTION);
    SET_IDT_ENTRY(idt[5], BOUNDS_RANGE_EXCEEDED_EXCEPTION);
    SET_IDT_ENTRY(idt[6], INVALID_OPCODE_EXCEPTION);
    SET_IDT_ENTRY(idt[7], DEVICE_NOT_AVAILABLE_EXCEPTION);
    SET_IDT_ENTRY(idt[8], DOUBLE_FAULT_EXCEPTION);
    SET_IDT_ENTRY(idt[9], COPROCESSOR_SEGMENT_OVERRUN_EXCEPTION);
    SET_IDT_ENTRY(idt[10], INVALID_TSS_EXCEPTION);
    SET_IDT_ENTRY(idt[11], SEGMENT_NOT_PRESENT_EXCEPTION);
    SET_IDT_ENTRY(idt[12], STACK_FAULT_EXCEPTION);
    SET_IDT_ENTRY(idt[13], GENERAL_PROTECTION_EXCEPTION);
    SET_IDT_ENTRY(idt[14], PAGE_FAULT_EXCEPTION);
    //Interrupt 15 - RESERVED BY INTEL
    SET_IDT_ENTRY(idt[16], x87FLOAT_POINT_EXCEPTION);
    SET_IDT_ENTRY(idt[17], ALIGNMENT_CHECK_EXCEPTION);
    SET_IDT_ENTRY(idt[18], MACHINE_CHECK_EXCEPTION);
    SET_IDT_ENTRY(idt[19], SIMD_FLOATING_POINT_EXCEPTION);
    //interrupt 32 to 255 defined by user

//-------------- cp5 ---------- 
    SET_IDT_ENTRY(idt[PIT_IDTIDX], &pithelper);// at 0x20
//-------------- cp5 ----------
    SET_IDT_ENTRY(idt[MASTER_8259_IRQ1_KEYBOARD], keyboard_helper);//at 0x21
    SET_IDT_ENTRY(idt[SLAVE1_8259_IRQ8_REALTIMECLOCK], rtc_helper);//at 0x28
    
    //call system call handler when execute a int 0x80 instruction  not need in check 1
    SET_IDT_ENTRY(idt[SYSTEM_CALL_VAECTOR], &system_call_helpler);
        //# Load the IDT
   // # IA-32 Intel    2.61/4.9 use LIDT (Load IDTR Register )
    
    lidt (idt_desc_ptr);
}


