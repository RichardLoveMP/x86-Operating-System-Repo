#include "cursor.h"

#define TWIDTH 80
#define VGA_Add_Port  0x3D4
#define VGA_Data_Port 0x3D5
#define BITSHIFT 8
#define BITMASKK 0xFF

#define FIRST_OUT 0x0F
#define SECOND_OUT 0x0E
/* set_cursor()
 * INPUT: The place (column and row) configuration to handle the cursor's position
 * Input: intenger COLUMN and ROW
 * Output: NONE
 * Functionality: To Place the cursor
 */
/* REF: OSDEV  https://wiki.osdev.org */
void set_cursor(int c, int r){

    // revised for checkpoint 5
    // sanity check for the sync of currentpcb with activated terminal 
    pcb_t *pcb = get_PCB();
    if(processing_terminal != pcb->terminal_belong_to)
      return;

    unsigned short position=(r*TWIDTH) + c;

    // cursor LOW port to vga INDEX register
    outb(FIRST_OUT, VGA_Add_Port);
    outb((unsigned char)(position&BITMASKK),VGA_Data_Port);
    // cursor HIGH port to vga INDEX register
    outb(SECOND_OUT, VGA_Add_Port);
    outb((unsigned char )((position>>BITSHIFT)&BITMASKK), VGA_Data_Port);
    return ;
}
