#include "terminal.h"
#include "cursor.h"
#include "system_call.h"
#include "contextswitchhelper.h"
#include "terminal.h"
#include "types.h"
#include "scheduler.h"
#include "paging.h"

#define HISTORY_LEN 129
#define TERMINAL_NUMBER 3
#define MAX_TRACK 20
#define FIRST_TERMINAL_COLOR 0x01
#define SECOND_TERMINAL_COLOR 0x02
#define THIRD_TERMINAL_COLOR 0x03
// THREE terminal function
terminal_t terminal[TERMINAL_NUMBER];
terminal_t* terminal_now;

// detect whether it is writing to the terminal
int iswriting = 0 ;
// create new terminal
int open_terminal_index = -1;
int processnum=0;

// active terminal number
int processing_terminal = 0;

// which terminal
int whichterminal = -1;

// terminal in our eyes
int visible_terminal = 0;

//command history
int cursor_back_up[TERMINAL_NUMBER][2];
int comand_entered = 1;


char last_input[MAX_TRACK][HISTORY_LEN];
int last_input_cnt = 0;
int last_read_cnt = 0;
int filled_last_input_entry_num = 0;
int last_action = -1;

int32_t CanBeSeen(pcb_t* PCB,uint8_t ch);




/*
 * Terminal_track_up
 *   DESCRIPTION: show history command upwards
 *   INPUTS:
 *   OUTPUTS: NONE 
 *   RETURN VALUE: NONE 
 *   SIDE EFFECTS: Modify the keyboard buffer and the video memory 
 *   in order to print the correct character on the screen 
 */
void terminal_track_up() {
	
	if (last_action==1) {
		//last_read_cnt is one greater by actual 
		if (last_read_cnt==0)
			last_read_cnt = filled_last_input_entry_num - 1;
		else
			last_read_cnt = (last_read_cnt-1)%filled_last_input_entry_num;
	}

	char buf[HISTORY_LEN];
	pcb_t* pcb = get_PCB();

	//backspace to the backup 
	if (pcb->terminal_belong_to==visible_terminal) {
		cli();
		while (terminal[visible_terminal].kbuflen > 0) {
			Process_BackSpace();
		}
		sti();
	} else return;
	
	memset(buf, 0, HISTORY_LEN);
	strcpy(buf, last_input[last_read_cnt]);		//copy recorded command history
	if (last_read_cnt==0)
		last_read_cnt = filled_last_input_entry_num - 1;
	else
		last_read_cnt = (last_read_cnt-1)%filled_last_input_entry_num;
	
	//copy to shell
	int i = 0;
	for (i=0; i<strlen(buf)-1; i++) {
		char ch = buf[i];
		Process_Character((uint8_t)ch);
	}
	//Process_Character("|");
	//printf("cursor backup: %d, %d",cursor_back_up[visible_terminal][0], cursor_back_up[visible_terminal][1]);
	last_action = 0;
}


/*
 * Terminal_track_down
 *   DESCRIPTION: show history command downwards
 *   INPUTS:
 *   OUTPUTS: NONE 
 *   RETURN VALUE: NONE 
 *   SIDE EFFECTS: Modify the keyboard buffer and the video memory 
 *   in order to print the correct character on the screen 
 */
void terminal_track_down() {
	
	if (last_action==0) {
		//last_read_cnt is one less by actual 
		last_read_cnt = (last_read_cnt+1)%filled_last_input_entry_num;
	}

	char buf[HISTORY_LEN];

	pcb_t* pcb = get_PCB();

	//backspace to the backup 
	if (pcb->terminal_belong_to==visible_terminal) {
		cli();
		while (terminal[visible_terminal].kbuflen > 0) {
			Process_BackSpace();
		}
		sti();
	} else return;
	
	memset(buf, 0, HISTORY_LEN);
	strcpy(buf, last_input[last_read_cnt]);
	last_read_cnt = (last_read_cnt+1)%filled_last_input_entry_num;	//copy recorded command history
	
	int i = 0;
	//copy to shell
	for (i=0; i<strlen(buf)-1; i++) {
		char ch = buf[i];
		Process_Character((uint8_t)ch);
	}

	last_action = 1;
}


/*
 * Process_Enter
 *   DESCRIPTION: calling function from lib.h to modify the video memory to display the enter on the correct terminal
 *   INPUTS: character to be written
 *   OUTPUTS: NONE 
 *   RETURN VALUE: NONE 
 *   SIDE EFFECTS: Modify the keyboard buffer and the video memory 
 *   in order to print the correct character on the screen 
 */
void Process_Enter()
{
  	pcb_t *pcb = get_PCB();
	uint8_t enter_ch = '\n';

	comand_entered = 1;

	// detect whether on the visible_terminal
  	if(CanBeSeen(pcb,enter_ch) == 0){
		// if we cannot see the current PCB
    	return;	//return 
  	}

	// handle the height overflow
	terminal[visible_terminal].terminal_cursor[0] = 0;	// xpos reset to zero
	terminal[visible_terminal].terminal_cursor[1] ++;	// ypos reset to the origin + 1
	if(terminal[visible_terminal].terminal_cursor[1] >= T_HEI){	// if reaching the bottom
		Process_Scroll();	// process possible scroll
	}

	// give the terminal read signal
	terminal[visible_terminal].tread_signal = 1;

	// add the enter to the keyboard buffer
	int kbuflen = terminal[visible_terminal].kbuflen;
	terminal[visible_terminal].kbuf[kbuflen] = enter_ch;
	terminal[visible_terminal].kbuflen = 0;

	//code for input history
	//set history buffer to empty
	memset(last_input[last_input_cnt], 0, HISTORY_LEN);
	memcpy(last_input[last_input_cnt], terminal[visible_terminal].kbuf, kbuflen+1); //copy command to the buffer
	last_read_cnt = last_input_cnt;
	last_input_cnt = (last_input_cnt+1)%MAX_TRACK;//UPDATE buffer index
	if (filled_last_input_entry_num<MAX_TRACK) {
		filled_last_input_entry_num += 1;
	}

	// reset the cursor
	set_cursor(terminal[visible_terminal].terminal_cursor[0],terminal[visible_terminal].terminal_cursor[1]);

	return ;

}


/*
 * Process_Backspace
 *   DESCRIPTION: calling function from lib.h to modify the video memory to display the backspace on the correct terminal
 *   INPUTS: character to be written
 *   OUTPUTS: NONE 
 *   RETURN VALUE: NONE 
 *   SIDE EFFECTS: Modify the keyboard buffer and the video memory 
 *   in order to print the correct character on the screen 
 */
void Process_BackSpace()
{	
	cli();
	// set iswriting to make sure that we should do it
	iswriting = 1;
  	pcb_t *pcb = get_PCB();	// get pcb
	uint8_t backspace_ch = '\b';
	// detect whether on the visible_terminal
  	if(CanBeSeen(pcb,backspace_ch) == 0){
		// if we cannot see the current PCB
		iswriting = 0;
		sti();
    	return;	//return 
  	}

	// lowerbound of the backspace
	if(0 == terminal[visible_terminal].kbuflen){
		sti();
		iswriting = 0;
		return ;
	}
	// modify the keyboard buffer
	terminal[visible_terminal].kbuflen --;

	//back to previous line
	if(terminal[visible_terminal].terminal_cursor[0]-- == 0){
		terminal[visible_terminal].terminal_cursor[1]--;
		terminal[visible_terminal].terminal_cursor[0] = T_WID - 1;
	}
	// clear the position	
	clear_pos(terminal[visible_terminal].terminal_cursor[0],terminal[visible_terminal].terminal_cursor[1]);
	// reset the cursor
	set_cursor(terminal[visible_terminal].terminal_cursor[0],terminal[visible_terminal].terminal_cursor[1]);
	iswriting = 0;
	sti();
	return ;
}



/*
 * Process_Scroll
 *   DESCRIPTION: Handle a DETERMINED scroll
 *   INPUTS: NONE
 *   OUTPUTS: NONE 
 *   RETURN VALUE: NONE 
 *   SIDE EFFECTS: Modify the video memory in order to scroll the screen correctly
 */
void Process_Scroll()
{
	pcb_t *pcb = get_PCB();
	// roll it
	roll();
	// terminal set
	terminal[pcb->terminal_belong_to].terminal_cursor[1] = T_HEI - 1;
	// set the new cursor
	set_cursor(terminal[pcb->terminal_belong_to].terminal_cursor[0],terminal[pcb->terminal_belong_to].terminal_cursor[1]);
	return ;
}


/*
 * Process_Character
 *   DESCRIPTION: calling function from lib.h to modify the video memory to display the character on the correct terminal
 *   INPUTS: character to be written
 *   OUTPUTS: NONE 
 *   RETURN VALUE: NONE 
 *   SIDE EFFECTS: Modify the keyboard buffer and the video memory 
 *   in order to print the correct character on the screen 
 */
void Process_Character(uint8_t ch){	
	// in order to sync with multi-terminal, we should build a critical section
	cli();
	iswriting = 1;	// while in this section, we won't want to be bothered by PIT
	// get current pcb;
  	pcb_t *pcb = get_PCB();
	iswriting = 1;
	// detect whether on the visible_terminal
  	if(CanBeSeen(pcb,ch) == 0){
		// if we cannot see the current PCB
		iswriting = 0;
		sti();
    	return;	//return 
  	}

	// keep reset because we want limit the racing issue as possible as we can
  	iswriting = 1;
	// handle the overflow situation
	if(terminal[visible_terminal].kbuflen>=KBUFMAXN){
		iswriting = 0;
		sti();
		return;
	}
	iswriting = 1;
	
	// write this character into the terminal
	int currpos = terminal[visible_terminal].kbuflen;
	terminal[visible_terminal].kbuf[currpos] = ch;
	terminal[visible_terminal].kbuflen += 1;
	// write this position with color defined
	write_pos_with_color(terminal[visible_terminal].terminal_cursor[0],terminal[visible_terminal].terminal_cursor[1],ch,terminal[visible_terminal].term_colours);
	
	
	// keep writting to ensure
	iswriting = 1;
	terminal[visible_terminal].terminal_cursor[0]++;	// get the cursor zero increase by 1
	iswriting = 1;

	// reset the terminal read signal
	if(terminal[visible_terminal].tread_signal==1)
		terminal[visible_terminal].tread_signal = 0;

	// process if we want to scroll the screen
	Process_Possible_Scroll();

	iswriting = 1;
	set_cursor(terminal[visible_terminal].terminal_cursor[0],terminal[visible_terminal].terminal_cursor[1]);
	iswriting = 0;
	sti();	// finish the critical section
	return ;
}

//End of keyboard handler helper functions for terminal 

/*
 * terminal_open
 *   DESCRIPTION: Open a new terminal
 *   INPUTS: filename (maybe NULL)
 *   OUTPUTS: NONE 
 *   RETURN VALUE: 0 for success 
 *   SIDE EFFECTS: INIT the terminal structure
 */
int32_t terminal_open(const uint8_t *filename){
	memset(last_input, 0, MAX_TRACK*HISTORY_LEN);
	int i;
	for(i=0; i<MAX_TERMINALS; i++){
		
		// terminal variable (structure) INIT

		terminal[i].isbootup = 0;	// 1 init terminal boot up
		terminal[i].apid = 0; // 2 init activated task of the terminal
		if(i > 0)
			terminal[i].apid = -1;	// first bootup, set to -1 (NULL TASK)
		int j = 0;
		for(j = 0; j < MAXLENN; j++)   // 3 init characters to be printed (reset to zero)
			terminal[i].pbuf[j] = '\0';
		terminal[i].pbuflen = 0; // 4 reset the length of a certain terminal
		terminal[i].terminal_cursor[0] = 0;
		terminal[i].terminal_cursor[1] = 0;	// reset the cursor position

		for(j = 0; j < MAXLENN; j++)
			terminal[i].kbuf[j] = '\0';	// reset the keyboard buffer

		terminal[i].kbuflen = 0;	// reset the buffer index

		terminal[0].term_colours = FIRST_TERMINAL_COLOR;	// set the first terminal's color
		terminal[1].term_colours = SECOND_TERMINAL_COLOR;	// set the second terminal's color
		terminal[2].term_colours = THIRD_TERMINAL_COLOR;	// set the third terminal's color

		terminal[i].tread_signal = 0;	// terminal read signal reset

	}

	terminal_clear();

	return 0;
}

/*
 * terminal_close
 *   DESCRIPTION: close a terminal 
 *   INPUTS: int32_t fd
 *   OUTPUTS: NONE 
 *   RETURN VALUE: 0 for success 
 *   SIDE EFFECTS: NONE
 */
int32_t terminal_close(int32_t fd){
	return 0;	// always return 0
}


/*
 *   terminal_read
 *   DESCRIPTION: read from the termial keyboard buffer into a given external buffer
 *   INPUTS: intenger fd, pointer of the external buffer, and the copy size nbytes
 *   OUTPUTS: Size of the successful copy
 *   RETURN VALUE: Size of the bytes 
 *   SIDE EFFECTS: We should call it by detecting whether we press the enter
 */


int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
		// sanity check
	if(buf == NULL)
		return -1;
	uint32_t i;

	pcb_t *pcb = get_PCB();
	int tty_index = pcb->terminal_belong_to;

	char * cbuf = (char*)buf;
/*
	if(bad_userspace_addr(buf,nbytes)==1)
		return -1;
	*/
	while(1){
		cli();				

		if(1 == terminal[tty_index].tread_signal){
			for(i=0;terminal[tty_index].kbuf[i]!='\n'&&i<=KBUFMAXN;i++){
				// assignment progress
				cbuf[i]=terminal[tty_index].kbuf[i];
			}
			cbuf[i]='\n';terminal[tty_index].tread_signal=0;
			sti();
			break;
		}
		sti();
	}

	return i+1;

}



/*
 *   terminal_write
 *   DESCRIPTION: write to the termial keyboard buffer from a given external buffer
 *   INPUTS: intenger fd, pointer of the external buffer, and the writing size nbytes
 *   OUTPUTS: Size of the successful copy
 *   RETURN VALUE: Size of the bytes written or -1 (unsuccessful)
 *   SIDE EFFECTS: We should call it by detecting whether we press the enter
 */

int32_t terminal_write(int32_t fd,  void* buf, int32_t nbytes){

	// sanity check
	if(buf == NULL)
		return -1;
	pcb_t *pcb = get_PCB();
	int tty_index = pcb->terminal_belong_to;
	uint32_t i=0;
	char *cbuf = (char*) buf;

	cli();
	for(i=0;i<nbytes;i++){
		if(cbuf[i]=='\n'){
			terminal[tty_index].terminal_cursor[1] ++;
			terminal[tty_index].terminal_cursor[0] = 0;
		}
		else{
			write_pos_with_color(terminal[tty_index].terminal_cursor[0],terminal[tty_index].terminal_cursor[1],cbuf[i],terminal[tty_index].term_colours);
			terminal[tty_index].terminal_cursor[0] ++;

			if(terminal[tty_index].terminal_cursor[0] >= T_WID){
				terminal[tty_index].terminal_cursor[0] = 0;
				terminal[tty_index].terminal_cursor[1] ++;
			}
		}
		if(terminal[tty_index].terminal_cursor[1] >= T_HEI){
			Process_Scroll();
		}
		set_cursor(terminal[tty_index].terminal_cursor[0],terminal[tty_index].terminal_cursor[1]);
		cursor_back_up[tty_index][0] = terminal[tty_index].terminal_cursor[0];
		cursor_back_up[tty_index][1] = terminal[tty_index].terminal_cursor[1];
	}
	sti(); 
	return i;
}


/*
 *   nputc (new version of putc)
 *   DESCRIPTION: write to the termial for one char
 *   INPUTS: the char we want to print
 *   OUTPUTS: None
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Modify the video memory to print a char on it
 */

void nputc(uint8_t c){
	pcb_t *pcb = get_PCB();
	// get pcb
	if(c == '\n') {
        if(++terminal[pcb->terminal_belong_to].terminal_cursor[1]>=T_HEI){
			Process_Scroll();
        }
        terminal[pcb->terminal_belong_to].terminal_cursor[0]=0;
    } else {
		// write the position with color
	    write_pos_with_color(terminal[pcb->terminal_belong_to].terminal_cursor[0],terminal[pcb->terminal_belong_to].terminal_cursor[1],c,terminal[pcb->terminal_belong_to].term_colours);
	  	terminal[pcb->terminal_belong_to].terminal_cursor[0]++;
		// process possible scroll by index
		Process_Possible_Scroll_By_Index(pcb->terminal_belong_to);
    }
	// set the cursor position
	set_cursor(terminal[pcb->terminal_belong_to].terminal_cursor[0], terminal[pcb->terminal_belong_to].terminal_cursor[1]);
	return ;
}



/*
 *   terminal_switch
 *   DESCRIPTION: switch the terminal with back up the video memory
 *   INPUTS: the terminal number that we'd like to switch to 
 *   OUTPUTS: None
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Modify the page structure to make it
 */
void terminal_switch(int isnewterminal, int term_no){
	if(isnewterminal){
		pcb_t *pcb = get_PCB();
		cli();
		// update the global info
		visible_terminal = term_no;
		open_terminal_index=term_no;

		memcpy((void*)VIDMEM_COPY_ADDRESS,(void*)VIDEO,NUM_COLS*NUM_ROWS*2);

		/* switching paging structure */
		paging_video_memory_switch(pcb->pid);

		/* switching terminal */
		terminal[pcb->terminal_belong_to].apid = pcb->pid;

		processing_terminal = term_no;
		//change cursor location
		set_cursor(terminal[term_no].terminal_cursor[0],terminal[term_no].terminal_cursor[1]);
		sti();

		// execute the new shell of the terminal 
		execute((uint8_t*)"shell");

		return;
	}
	cli();
	visible_terminal = term_no;
	pcb_t *pcb = get_PCB();

	
	int processingpid = terminal[processing_terminal].apid;

	pcb_t *processingpcb = getProcPCB(processingpid);
	
	if(processingpcb->terminal_belong_to==term_no)
		return;
	//backup video memory
	memcpy((void*)VIDMEM_COPY_ADDRESS,(void*)VIDEO,NUM_COLS*NUM_ROWS*2);
	
	// video memory switch
	paging_video_memory_switch(processingpid);
	
	set_cursor(terminal[term_no].terminal_cursor[0],terminal[term_no].terminal_cursor[1]);
	//printf("cursor position: %d, %d\n",terminal[term_no].terminal_cursor[0],terminal[term_no].terminal_cursor[1]);

	// next field
	int next_active_pid = terminal[term_no].apid;
	pcb_t *next_term_pcb = getProcPCB(next_active_pid);


	page_table_array[next_active_pid].page_table[VIDEO_BASE].page_base_address = VIDEO >> TERMINAL_TWL;
	if(page_directory_array[next_active_pid].page_directory[THIRTY_THREE].present == 1){
		page_table_array[MAX_PROCESSES + next_active_pid].page_table[0].page_base_address = VIDEO >> TERMINAL_TWL;
	}

	// setting the CR3 register
	CR3_setting((uint32_t*)page_directory_array[next_term_pcb->pid].page_directory);

	memcpy((void*)VIDEO,(void*)VIDMEM_COPY_ADDRESS,NUM_COLS*NUM_ROWS*2);

	// setting the CR3 register
	CR3_setting((uint32_t*)page_directory_array[pcb->pid].page_directory);

	// finally give back the active terminal number
  	processing_terminal = term_no;
  	sti();

	return;
}


/*
 * terminal_clear
 *   DESCRIPTION: calling function from lib.h to modify the video memory to clean the whole terminal
 *   INPUTS: None
 *   OUTPUTS: Nothing
 *   RETURN VALUE: NONE 
 *   SIDE EFFECTS: Clean the whole terminal (Reset the video memory) 
 */
void terminal_clear()
{	
	int flags = 0;
	// clean the terminal process
	clear();	// in lib.h 

	if(ece391bootup){
		cleanit();	// first goes into the terminal, only reset terminal zero
		flags = 1;
	}

	if(flags){
		return ;
	}
	// beacuse we need to sync between multiple terminals, we need to avoid the racing conditions
	cli();
	pcb_t *pcb = getVisiblePCB();	// reset pcb which is on the top (in our eyes)
	resetONEterminal(pcb->terminal_belong_to);	// reset one terminal
	sti();

	// reset the new cursor position
	set_cursor(terminal[pcb->terminal_belong_to].terminal_cursor[0], terminal[pcb->terminal_belong_to].terminal_cursor[0]);
	return ;
}


/* 
 * context_switch
 *   DESCRIPTION: using context switch to execute something
 *   INPUTS: NONE
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE_EFFECTS: Modify the paging structure to get the context switched
 */

void context_switch_terminal(){
	int new_term;
	if(iswriting == 1)
		return ;
    static int active_process_pid = 0;
	cli();
  	pcb_t *pcb = get_PCB();
  // check if current process allotted cpu time is in the visible terminal
  
	cli();
  	if(pcb->terminal_belong_to == visible_terminal){
		clearbuffer();
	}
  	sti();
	  
	// decide whether to switched
	if (whichterminal != -1) {		//enter a new terminal
		if(pcb->terminal_belong_to == processing_terminal){
			if(terminal[whichterminal].isbootup==0){
				//need to start a new terminal
				if(processnum>=MAX_PROCESSES){
					puts("cannot launch new terminal \n");
					Process_Enter();
					sti();
					whichterminal = -1;
				}
				else{
					new_term=whichterminal;
					whichterminal = -1;
					sti();
					terminal_switch(1,new_term);
				}
			}
			else{
				//normal switch between terminals
				new_term = whichterminal;
				whichterminal = -1;
				sti();
				terminal_switch(0,new_term);
			}
		whichterminal = -1;
		//return;
		} 
	}
  	
	
	active_process_pid = runqueue_get_next_process_pid(pcb);
	//active_process_pid = terminal[active_process_index].apid;

	if(pcb->pid == active_process_pid) {
		renew_runqueue();
		return;
	}
		
	//save  current execution context
	save_ESP(pcb->esp);
	save_EBP(pcb->ebp);
	terminal[pcb->terminal_belong_to].apid=pcb->pid;
	//need to save eip
	pcb_t *next_pcb = getProcPCB(active_process_pid);

	//change pagedirectory
	CR3_setting((uint32_t*)page_directory_array[next_pcb->pid].page_directory);
	//change esp0 in tss
	tss_t *curr_tss = &tss;
	curr_tss->esp0=MB8-(next_pcb->pid)*KB8;
	
	int32_t statusbuf[STATUS_BUF_LEN];

	get_user_status(statusbuf,curr_tss);
	save_user_status(pcb,statusbuf);

	//reserving stack for assembly function call
	sti();
	
	contextswitchhelper(next_pcb->eip, next_pcb->esp, next_pcb->ebp, &(pcb->eip));


	load_user_status(pcb,curr_tss);

}


/* keyboard clear
 *   DESCRIPTION: if we detect the ctrl + L key, we should set the pending character to terinal clear signal
 * 		and when we process it in context switch process, we will process this step
 *   INPUTS: NONE
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE_EFFECTS: clear the keyboard buffer
 */
void keyboard_clear(){
    int currlen = terminal[visible_terminal].pbuflen;
	terminal[visible_terminal].pbuf[currlen] = '\v';
    terminal[visible_terminal].pbuflen++;
    

    currlen = terminal[visible_terminal].pbuflen;

    terminal[visible_terminal].pbuf[currlen] = '\n';
    terminal[visible_terminal].pbuflen++;
	return ;
}




/* cleanit 
 *   DESCRIPTION: clean the terminal
 *   INPUTS: NONE
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE_EFFECTS: clean the terminal
 */
void cleanit(){
	terminal[0].terminal_cursor[0] = 0;	
	terminal[0].terminal_cursor[1] = 0;	
	set_cursor(terminal[0].terminal_cursor[0], terminal[0].terminal_cursor[1]);	//set cursor to the start of the screen
	return ;
}



/* resetONEterminal 
 *   DESCRIPTION: reset the terminal
 *   INPUTS: terminal_indx: the iondex of the terminal to be reset 
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE_EFFECTS: reset the terminal
 */
void resetONEterminal(int terminal_idx){
	//reset all value in terminal data structure
	terminal[terminal_idx].terminal_cursor[0] = 0;	
	terminal[terminal_idx].terminal_cursor[1] = 0;
	terminal[terminal_idx].kbuflen = 0;
	terminal[terminal_idx].tread_signal = 0;	
	return ;
}



/* CanBeSeen 
 *   DESCRIPTION: decide if a terminal can be seen (visible_terminal)
 *   INPUTS: PCB: the process pcb to check; ch: the input char on visible terminal
 *   OUTPUTS: none
 *   RETURN VALUE: 1 for is the current visible terminal, 0 for not being visible
 *   SIDE_EFFECTS: add the input charn to the terminal's kbuf if the terminal is visible
 */
int32_t CanBeSeen(pcb_t* PCB,uint8_t ch){
	int retval = 0;
	//check if the current pcb's terminal field is the current visibele terminal
	if(PCB->terminal_belong_to == visible_terminal)
		//add the input char to the visible terminal's kbuf is done outside this function
		retval = 1;

	if(PCB->terminal_belong_to != visible_terminal){
		retval = 0;
		//add the input charn to the terminal's kbuf if the terminal is not visible
		int currlen = terminal[visible_terminal].pbuflen;
		terminal[visible_terminal].pbuf[currlen] = ch;
		terminal[visible_terminal].pbuflen ++;	
	}
	return retval;
}


/* Process_Possible_Scroll 
 *   DESCRIPTION: if need to scroll down one row, do it
 *   INPUTS: None
 *   OUTPUTS: none
 *   RETURN VALUE: None
 *   SIDE_EFFECTS: may scroll down one row
 */
void Process_Possible_Scroll(){
	// Process possible scroll
	if(terminal[visible_terminal].terminal_cursor[0] >= T_WID){
		//check if the terminal need to scroll up
		terminal[visible_terminal].terminal_cursor[0] = 0;
		terminal[visible_terminal].terminal_cursor[1] ++;	//scroll up one row
		if(terminal[visible_terminal].terminal_cursor[1] >= T_HEI){
			Process_Scroll();
		}
	}
}




/* void Process_Possible_Scroll_By_Index
 *   DESCRIPTION: if given terminal needs to be scrolled down one row, do it
 *   INPUTS: teh index for the terminal to scroll down
 *   OUTPUTS: none
 *   RETURN VALUE: None
 *   SIDE_EFFECTS: may scroll down one row in given terminal
 */
void Process_Possible_Scroll_By_Index(int terminal_index){
		// Process possible scroll
	if(terminal[terminal_index].terminal_cursor[0] >= T_WID){
		//check if the terminal need to scroll up
		terminal[terminal_index].terminal_cursor[0] = 0;
		terminal[terminal_index].terminal_cursor[1] ++;	//scroll up one row
		if(terminal[terminal_index].terminal_cursor[1] >= T_HEI){
			Process_Scroll();
		}
	}
}





/* clearbuffer 
 *   DESCRIPTION: do all things record in the buffer and clean buffer
 *   INPUTS: None
 *   OUTPUTS: none
 *   RETURN VALUE: None
 *   SIDE_EFFECTS: clean buffer
 */
void clearbuffer(){
	int i;
	for(i = 0;i<terminal[visible_terminal].pbuflen;i++){
		iswriting = 1;
	//	printf("visible terminal is : %d\n", visible_terminal);
		switch (terminal[visible_terminal].pbuf[i])
		{
		case '\b':
			Process_BackSpace();	//process backspace
			break;

		case '\n':
			Process_Enter();	//process enter
			break;

		case '\v':
			terminal_clear();	//process terminal clear
			break;
			
		default:
			Process_Character(terminal[visible_terminal].pbuf[i]);	//process input character
			break;
		}
		

		terminal[visible_terminal].pbuf[i] = '\0';
		iswriting = 0;		//change iswriting signal to 0
	}
	terminal[visible_terminal].pbuflen = 0;
	//pbuflen[visible_terminal] = 0;
	return ;
}





