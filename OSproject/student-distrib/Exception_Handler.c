//  Interrupt_Descriptor_Table.h
//  Created by kaiwei on 2019/10/18.
//  Copyright © 2019 kaiwei. All rights reserved.
//

#include "Exception_Handler.h"
#include "system_call.h"

/*
 *DIVIDE_ERROR_EXCEPTION()
 * DIVIDE_ERROR_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print DIVIDE_ERROR_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
DIVIDE_ERROR_EXCEPTION(){
    
    printf("DIVIDE_ERROR_EXCEPTION: Divided by ZERO.\n");
    
    //while(1){
        bluescreen();
    //}
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *DEBUG_EXCEPTION()
 * DEBUG_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print DEBUG_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
DEBUG_EXCEPTION(){
    printf("DEBUG_EXCEPTION: Trap or Fault.\n");
    //while(1){
        bluescreen();
    //}
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *NMI_EXCEPTION()
 * NMI_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print NMI_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
NMI_EXCEPTION(){
    printf("NMI_EXCEPTION: Non-maskable Interrupt.\n");
    //while(1){
        bluescreen();
    //}
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *BREAKPOINT_EXCEPTION()
 * BREAKPOINT_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print BREAKPOINT_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
BREAKPOINT_EXCEPTION(){
    printf("BREAKPOINT_EXCEPTION: A breakpoint instruction (INT 3) was executed, causing a breakpoint trap to be generated.\n");
   // while(1){
        bluescreen();
    //}
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *OVERFLOW_EXCEPTION()
 * OVERFLOW_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print OVERFLOW_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
OVERFLOW_EXCEPTION(){
    printf("OVERFLOW_EXCEPTION: An overflow trap occurred.\n");
    //while(1){
        bluescreen();
    //}
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *BOUNDS_RANGE_EXCEEDED_EXCEPTION()
 * BOUNDS_RANGE_EXCEEDED_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print BOUNDS_RANGE_EXCEEDED_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
BOUNDS_RANGE_EXCEEDED_EXCEPTION(){
    printf("BOUNDS_RANGE_EXCEEDED_EXCEPTION: A BOUND-range-exceeded fault occurred when a BOUND instruction was executed. \n");
    //while(1){
        bluescreen();
   // }
    /* You  can add  your code here to do what you want */
    /* .... */
     halt(15); // 15 is a exception signal
}
/*
 *INVALID_OPCODE_EXCEPTION()
 * INVALID_OPCODE_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print INVALID_OPCODE_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
INVALID_OPCODE_EXCEPTION(){
    printf("INVALID_OPCODE_EXCEPTION: Invalid Opcode.\n");
  //  while(1){
        bluescreen();
    //}
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *DEVICE_NOT_AVAILABLE_EXCEPTION()
 * DEVICE_NOT_AVAILABLE_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print DEVICE_NOT_AVAILABLE_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
DEVICE_NOT_AVAILABLE_EXCEPTION(){
    printf("DEVICE_NOT_AVAILABLE_EXCEPTION: Device-not-available.\n");
   // while(1){
        bluescreen();
    //}
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *DOUBLE_FAULT_EXCEPTION()
 * DOUBLE_FAULT_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print DOUBLE_FAULT_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
DOUBLE_FAULT_EXCEPTION(){  // the err_code always is 0
    printf("DOUBLE_FAULT_EXCEPTION: The processor detected a second exception while calling an exception handler for a prior exception.\n");
   // printk("(Always 0)The err_code is %08x\n", err_code);
  //  while(1){
        bluescreen();
   // }
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *COPROCESSOR_SEGMENT_OVERRUN_EXCEPTION()
 * COPROCESSOR_SEGMENT_OVERRUN_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print COPROCESSOR_SEGMENT_OVERRUN_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
COPROCESSOR_SEGMENT_OVERRUN_EXCEPTION(){
    printf("COPROCESSOR_SEGMENT_OVERRUN_EXCEPTION: Detected a page or segment violation while transferring the middle portion of an Intel 387 math coprocessor operand. \n");
   // while(1){
        bluescreen();
   // }
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *INVALID_TSS_EXCEPTION()
 * INVALID_TSS_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print INVALID_TSS_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
INVALID_TSS_EXCEPTION(){
    printf("INVALID_TSS_EXCEPTION: There was an error related to a TSS\n");
     //while(1){
        bluescreen();
    //}
   // printk("The err_code is %08x\n", err_code);
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *SEGMENT_NOT_PRESENT_EXCEPTION()
 * SEGMENT_NOT_PRESENT_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print SEGMENT_NOT_PRESENT_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
SEGMENT_NOT_PRESENT_EXCEPTION(){
    printf("SEGMENT_NOT_PRESENT_EXCEPTION: The present flag of a segment or gate descriptor is clear\n");
   // while(1){
        bluescreen();
   // }
    //printk("The err_code is %08x\n", err_code);
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *STACK_FAULT_EXCEPTION()
 * STACK_FAULT_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print STACK_FAULT_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
STACK_FAULT_EXCEPTION(){
    printf("Stack Fault.\n");
  //  while(1){
        bluescreen();
   // }
  //  printk("The err_code is %08x\n", err_code);
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *GENERAL_PROTECTION_EXCEPTION()
 * GENERAL_PROTECTION_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print GENERAL_PROTECTION_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
GENERAL_PROTECTION_EXCEPTION(){
    printf("GENERAL_PROTECTION_EXCEPTION: The processor detected one of a class of protection violations called “general- protection violations.”\n");
   // while(1){
        bluescreen();
   // }
     //printk("The err_code is %08x\n", err_code);
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *PAGE_FAULT_EXCEPTION()
 * PAGE_FAULT_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print PAGE_FAULT_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
PAGE_FAULT_EXCEPTION(){
    printf("PAGE_FAULT_EXCEPTION: Page-Fault.\n");
//    while(1){
        bluescreen();
//    }
    //printk("The err_code is %08x\n", err_code);
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}


//Interrupt 15 - RESERVED BY INTEL

/*
 *x87FLOAT_POINT_EXCEPTION()
 * x87FLOAT_POINT_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print x87FLOAT_POINT_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
x87FLOAT_POINT_EXCEPTION(){
    printf("x87FLOAT_POINT_EXCEPTION: The x87 FPU has detected a floating-point error\n");
  //  while(1){
        bluescreen();
  //  }
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *ALIGNMENT_CHECK_EXCEPTION()
 * ALIGNMENT_CHECK_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print ALIGNMENT_CHECK_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
ALIGNMENT_CHECK_EXCEPTION(){  // err_code always 0
    printf("ALIGNMENT_CHECK_EXCEPTION: The processor detected an unaligned memory operand when alignment checking was enabled.\n");
  //  printk("The err_code is %08x\n", err_code);
   // while(1){
        bluescreen();
   // }
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *MACHINE_CHECK_EXCEPTION()
 * MACHINE_CHECK_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print MACHINE_CHECK_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
MACHINE_CHECK_EXCEPTION(){
    printf("MACHINE_CHECK_EXCEPTION: The processor detected an internal machine error or a bus error, or that an external agent detected a bus error.\n");
    //while(1){
        bluescreen();
    //}
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}
/*
 *SIMD_FLOATING_POINT_EXCEPTION()
 * SIMD_FLOATING_POINT_EXCEPTION
 * INPUT:none
 * OUTPUT: blue screen and print SIMD_FLOATING_POINT_EXCEPTION error 
 * RETURN: none 
 * SIDEEFFECT: none 
 */
void
SIMD_FLOATING_POINT_EXCEPTION(){
    printf("SIMD_FLOATING_POINT_EXCEPTION: The processor has detected an SSE/SSE2/SSE3 SIMD floating-point exception.\n");
    //while(1){
        bluescreen();
    //}
    /* You  can add  your code here to do what you want */
    /* .... */
        halt(15); // 15 is a exception signal
}

