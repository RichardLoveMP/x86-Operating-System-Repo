#ifndef PIT_H
#define PIT_h

#include "lib.h"
#include "types.h"
#include "i8259.h"

#define PIT_1 0x40 
#define PIT_2 0x43
#define PIT_IRQ   0
#define PIT_BASE_FREQ 1193180 

#define PIT_INIT_CMD 0x36

#define PIT8 8

#define PITBITMASK 0xFF

extern int32_t pit_init(int hz);

extern void pit_handler();

#endif

