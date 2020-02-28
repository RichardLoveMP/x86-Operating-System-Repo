#ifndef _KEYBOARD_H
#define _KEYBOARD_H


#include "i8259.h"
#include "types.h"
#include "x86_desc.h"



/* Prototype of the set of the function */
/* VERSION 1.0: add "init_keyb" and "keyb_handler" to "keyb.h" */
/* enable the IRQ function (temporary) */
extern void init_keyb();

/* handle the interrupt function */
extern void keyboard_handler();

/* support the multi functions test */
extern void keyboard_helper();

/* initialize the scancode */
extern uint8_t init_scode(uint8_t scode);

/* detect signal that wake up the terminal read signal */
int32_t terminal_read_signal;


#endif
