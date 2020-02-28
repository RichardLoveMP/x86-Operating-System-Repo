#ifndef _TERMINAL_H
#define _TERMINAL_H

//#include "lib.h"
#include "cursor.h"
#include "lib.h"
#include "keyboard.h"
#include "x86_desc.h"
#include "system_call.h"
#include "types.h"
// In lib.h
/* NUM_COLS 80  NUM_ROWS 25 */

#define MB8 0x800000
#define KB8 0x2000

#define MAX_PROCESSES 6
#define VIDEO_BASE 0xB8

#define VIDEO_BASE_CP 0xB9

#define KBUFMAXN 128
#define T_WID 	80
#define T_HEI 	25
#define NUM_COLS 80
#define NUM_ROWS 25
#define TERMINAL_TWL 12
#define THIRTY_THREE 33
#define MAX_TERMINALS 3
#define STATUS_BUF_LEN 10
#define MAXLENN 129

#define VIDMEM_COPY_ADDRESS 0xB9000
// Version Checkpoint 3.5: Give a structure for the terminal
typedef struct terminal_t{
	uint8_t pbuf[MAXLENN]; // 3 characters to be printed 
	uint8_t kbuf[MAXLENN]; // 6 terminal keyboard buffer
	int kbuflen;	// 7 terminal keyboard buffer index
	int cpos; 		//cursor position in kbuf
	int pbuflen; // 4 the length of the buffer above
	uint8_t term_colours;	// 8 terminal's designated colour
	uint8_t tread_signal; // 9 terminal read signal
	uint8_t isbootup;	// 1 whther it is booted up
	int apid; // 2  terminal's active task

	uint32_t terminal_cursor[2]; // 5 terminal cursor position

}terminal_t;

extern terminal_t terminal[MAX_TERMINALS];
extern terminal_t* terminal_now;

extern int open_terminal_index;

extern int processnum; 
extern int processing_terminal; 
extern int whichterminal;


extern int iswriting ;

extern int visible_terminal;




/* Process the enter code */
extern void Process_Enter();

/* Process the backspace code */
extern void Process_BackSpace();

/* Process the ctrl + L code */
extern void Process_Clean();

/* Process the normal character and print it to the screen */
extern void Process_Character(uint8_t ch);

/* read from the terminal keyboard buffer */
extern int terminal_read(int fd,void* buf, int nbytes);

/* put something to the terminal */
extern int terminal_write(int fd,void* buf,int nbytes);

/* Close the terminal safely */
extern int terminal_close();

/* The revised version of the putchar */
extern void nputc(uint8_t ch);

extern int32_t terminal_open(const uint8_t* filename);

extern void terminal_track_down();

extern void terminal_track_up();

extern void terminal_clear();

void terminal_switch(int isnewterminal, int term_no);
void context_switch_terminal();
/* keyboard clear signal */
extern void keyboard_clear();

void cleanit();

void Process_Possible_Scroll();

void resetONEterminal(int terminal_idx);
void Process_Scroll();
void Process_Possible_Scroll_By_Index(int terminal_index);

void clearbuffer();


#endif
