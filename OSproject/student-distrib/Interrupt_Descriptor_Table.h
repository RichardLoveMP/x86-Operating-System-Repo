//  Interrupt_Descriptor_Table.h
//  Created by kaiwei on 2019/10/18.
//  Copyright © 2019 kaiwei. All rights reserved.
//

#ifndef INTERRUPT_DESCRIPTOR_TABLE_H
#define INTERRUPT_DESCRIPTOR_TABLE_H
// just get the IDT from the student notes in page 29
//0x00-0x1F is defined by Intel


#include "x86_desc.h"

#define IDT_M1 20
#define IDT_M2 0x20
#define IDT_M3 0x2F

#define DPL3 3

void idt_init();
// master
//#define MASTER_8259_IRQ0_TIMER_CHIP 0x20
#define MASTER_8259_IRQ1_KEYBOARD 0x21
#define MASTER_8259_IRQ2_SLAVE1 0x22
//#define MASTER_8259_IRQ4_KGDB 0x24

//slave
#define SLAVE1_8259_IRQ8_REALTIMECLOCK 0x28
//#define SLAVE1_8259_IRQ11_NETWORK 0x2B   // don't know if needed
//#define SLAVE1_8259_IRQ12_MOUSE 0x2C
//#define SLAVE1_8259_IRQ14_HARDDRIVE 0x2E

// 0x30-0x7F for APIC vectors availiable to device drivers
// as mp3.pdf said
#define SYSTEM_CALL_VAECTOR 0x80

#endif /* Interrupt_Descriptor_Table */
