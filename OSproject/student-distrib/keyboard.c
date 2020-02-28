#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "x86_desc.h"
#include "Interrupt_Descriptor_Table.h"
#include "terminal.h"
#include "ROfilesystem.h"
#include "rtc.h"
#include "system_call.h"



#define IRQ_KEYB 1  /* handler number of the keyboard */
#define PORT_KEYB 0x60  /* port of the keyboard */
#define KEYB_MAST_CHAR 58   /* maximum of the keyboard character */

/* Keyboard Convention Code Ref: https://wiki.osdev.org/PS/2_Keyboard */

/* Control Status */
#define PRESS_CTRL 0x1D
#define RELASE_CTRL 0x9D    
/* Lshift Status */
#define PRESS_LSHIFT 0x2A   // 42
#define RELEASE_LSHIFT 0xAA // 170

/* Rshift Status */
#define PRESS_RSHIFT 0x36 
#define RELEASE_RSHIFT 0xB6 // 182

/* Enter Status */
#define PRESS_ENTER 0x1C

/* Capslock Status */
#define CAPS_LOCK 0x3A //58

#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4
#define FIVE 5

/* Backspace Status */
#define BACKSPACE 0x0E
#define PRESS_L 0x26
#define PRESS_C 0x2E

/* Alt Status */
#define PRESS_ALT 56
#define RELEASE_ALT 184

/*up & Down*/
#define UP_PRESS 72
#define DOWN_PRESS 80
#define LEFT_PRESS 75
#define RIGHT_PRESS 77


/* Function Key status */
#define PRESS_F1 59
#define PRESS_F2 60
#define PRESS_F3 61

#define RELEASE_F1 187
#define RELEASE_F2 188
#define RELEASE_F3 189


static uint16_t Ctrl;
static uint16_t UP;
static uint16_t DOWN;
static uint16_t LEFT;
static uint16_t RIGHT;
static uint16_t Lshift;
static uint16_t Rshift;
static uint16_t Enter;
static uint16_t Backspace;
static uint16_t Capslock;


static uint16_t Alt;
static uint16_t F1;
static uint16_t F2;
static uint16_t F3;



/* const uint8_t* keyb2ch
 * it contains the lookup table of the keyboard input with input character
 * VERSION 1.0: Only consider [a-z] && [0-9]
 * VERSION 2.0: Extern it to the whole keyboard except the numpad
 * Side Effect: Consideing some other characters
 * Implicit possible glitch: In Mac Style Keyboard
 */
const uint8_t keyb2ch[KEYB_MAST_CHAR] = {0,' ','1','2','3','4','5',
                               '6','7','8','9','0',
                               '-','=','\b','\t','q','w',
                               'e','r','t','y','u',
                               'i','o','p','[',']',
                               '\n',0,'a','s','d',
                               'f','g','h','j','k',
                               'l',';','\'','`',0, 
                               '\\','z','x','c','v',
                               'b','n','m',',','.','/',
                                0,0,0,' '
                               };
/* Shift or CapsLock mode of the lookup table of the keyboard */
const uint8_t shift2ch[KEYB_MAST_CHAR] = {0,' ','!','@','#','$','%',
                               '^','&','*','(',')',
                               '_','+',0,'\t','Q','W',
                               'E','R','T','Y','U',
                               'I','O','P','{','}',
                               '\n',0,'A','S','D',
                               'F','G','H','J','K',
                               'L',':','\"','~',0, 
                               '|','Z','X','C','V',
                               'B','N','M','<','>','\?',
                                0,0,0,' '
                               };


/* init_keyb()
 * In order to initialize the keyboard, we should first enable the IRQ
 * Input: None
 * Output: None
 * Function: enable the interrupt request of the interrupt handler request
 */
void
init_keyb(){
    enable_irq(IRQ_KEYB);   /* enable the interrupt request */
    Ctrl = 0; /* Initialize the ctrl */
    UP = 0;
    DOWN = 0;
    Lshift = 0;
    Rshift = 0; /* Initialize the shift condition */
    Enter = 0;  /* Initialize the enter condition */
    Backspace = 0;  /* Initialize the backspace condition */
    terminal_read_signal = 0; /* Enter is the terminal read signal */
    return ;
}


/* keyb_handler()
 * In order to handle the interrupt of the keyboard, we should deal with
 * the first cli() -- clean the interrupt and "DO_IRQ" then sti()
 * Function: Handle the interrput of the keyboard
 * SIDE EFFECT: NONE
 * VERSION 1.0: Only Basic Requirement of the keyboard
 * VERSION 2.0: Handle all of the characters
 */ 
void 
keyboard_handler(){
    uint16_t i;
    unsigned long flg;  /* flag */
    cli_and_save(flg);  /* clear the interrupt */
    disable_irq(IRQ_KEYB);

    uint8_t keyb_input;
    keyb_input = inb(PORT_KEYB); 
    for(;;){
        if(keyb_input){
            break;
        }
        keyb_input = inb(PORT_KEYB);
       // printf("aaaaa: %i\n",keyb_input);
    }
    /* Security Check 
     * If the value of the keyboard_input is outside the array,
     * do nothing.
     * V2.0: Add a helper function to help the sanity check
     */ 
    if((init_scode(keyb_input) == 0)){
        send_eoi(IRQ_KEYB); /* Interrupt handler finish, quit */
        enable_irq(IRQ_KEYB);
        sti();  /* restore the interrupt */
        return ;
    }

    /* special judge for tab, temporarily we do not handle it */
    if(keyb_input < KEYB_MAST_CHAR){
        if(keyb2ch[keyb_input] == '\t'){    // input character equals to tab
            send_eoi(IRQ_KEYB); /* Interrupt handler finish, quit */
            enable_irq(IRQ_KEYB);
            sti();  /* restore the interrupt */
            return ;
        }
    }

    /* version 1.0 
    putc(keyb2ch[keyb_input]);
    */
    /* VERSION 2.0 */
    // there are three cases 1. enter is pressed 2. clean up the terminal 3. backspace is pressed 4. print a char
    for(i = 0; i < FIVE; i ++){
        if(i == 0){
            if(Enter == 1){
                terminal_read_signal = 1;
                Process_Enter();
                Enter = 0;  // reset the Enter
                break;
            }
        }
        if(i == ONE){
            if(Ctrl == 1 && keyb_input == PRESS_L){ // Ctrl + L
                keyboard_clear();    // process clean
                Ctrl = 0;
                break;
            }
            /*
            if(Ctrl == 1 && keyb_input == PRESS_C){ // handle the control
                whichterminal = 1;
                Ctrl = 0;
                break;
            }
            */
        }
        if(i == TWO){
            if(Backspace == 1){ // handle the backspace
                Process_BackSpace();    // process backspace 
                Backspace = 0;
                break;
            }
        }
        
        if(i == THREE){
            /*
            if(UP == 1){ // handle the backspace
                terminal_track_up();    // process up track command history
                UP = 0;
                break;
            }
            if(DOWN == 1){ // handle the backspace
                terminal_track_down();    // process up track command history
                DOWN = 0;
                break;
            }
            */
            if(LEFT == 1){ // handle the backspace
                //terminal_left();    // process up track command history
                LEFT = 0;
                break;
            }
            if(RIGHT == 1){ // handle the backspace
                //terminal_right();    // process up track command history
                RIGHT = 0;
                break;
            }
        }
        
        if(i == FOUR){
            // judge whether it is a real character instead of a 
            if((keyb_input < KEYB_MAST_CHAR) && (keyb2ch[keyb_input] != 0)){
                
                /* Process the character */
                uint8_t ch;
                if((Lshift == 0 && Rshift == 0) && (Capslock == 0)){    // keyboard to char
                    ch = keyb2ch[keyb_input];
                }
                if((Lshift == 0 && Rshift == 0) && (Capslock == 1)){
                    ch = keyb2ch[keyb_input];
                    if(ch >= 'a' && ch <= 'z'){ // ctrl we distinguish, but caps not
                        ch = shift2ch[keyb_input];
                    }
                }
                if((Lshift == 1 || Rshift == 1) && (Capslock == 0)){
                    ch = shift2ch[keyb_input];  // shift to char
                }
                if((Lshift == 1 || Rshift == 1) && (Capslock == 1)){
                    ch = shift2ch[keyb_input];
                    if(ch >= 'A' && ch <= 'Z'){ // if it is in the right path, do it
                        ch = keyb2ch[keyb_input];
                    }
                }

                Process_Character(ch);
            }
        }
    }

	// detect alt
    if(Alt){
        if(F1){
            whichterminal = 0;
			//send_eoi(1);
			//enable_irq(1);
        }else if(F2){
            whichterminal = 1;
			//send_eoi(1);
			//enable_irq(1);
        }else if (F3){
            //switch_term(2);
			whichterminal = 2;
			//send_eoi(1);
			//enable_irq(1);
		}
    }

    send_eoi(IRQ_KEYB); /* Interrupt handler finish, quit */

    restore_flags(flg); /* restore the flag */

    enable_irq(IRQ_KEYB);

    sti();  /* restore the interrupt */

    return ;
}


/* init_scode
 * Used by external function: to detect the character or special character of the input scancode
 * Mark them in advance, so we can handle it more easily later
 * SIDE EFFECT: NONE
 * RETURN: 0 if the scancode is not valid, 1 if the scan code is valid
 * VERSION 2.0: Handle all of the characters
 */ 

uint8_t init_scode(uint8_t scode){
    /* return value 0 means wrong */
    uint8_t rval = 0;

    if(scode == PRESS_ENTER){   /* if the scan code is press enter */
        rval = 1;
        Enter = 1;
        return rval;
    }

    if(scode == BACKSPACE){ /* if the scan code is backspace */
        rval = 1; 
        Backspace = 1;  /* directly return */
        return rval;
    }

    if(scode == CAPS_LOCK){ /*if it is capslock */
        rval = 1;
        /* Cycle */
        Capslock = ((Capslock + 1) % TWO);  /* Cycle to move, because it is a discrete status */
        return rval;
    }
    /* SPECIAL JUDGE */
    if(scode == PRESS_CTRL){    /* Press control */
        rval = 1;
        Ctrl = 1;
        return rval;
    }
    if(scode == RELASE_CTRL){   /* handle the situation release control */
        rval = 1;
        Ctrl = 0;
        return rval;
    }
    if(scode == DOWN_PRESS){    /* Press control */
        rval = 1;
        DOWN = 1;
        return rval;
    }
    if(scode == UP_PRESS){    /* Press control */
        rval = 1;
        UP = 1;
        return rval;
    }
    if(scode == LEFT_PRESS){    /* Press control */
        rval = 1;
        LEFT = 1;
        return rval;
    }
    if(scode == RIGHT_PRESS){    /* Press control */
        rval = 1;
        RIGHT = 1;
        return rval;
    }
    if(scode == PRESS_LSHIFT){  /* If we press the left shift */
        rval = 1;
        Lshift = 1; /* return rval */
        return rval;
    }
    if(scode == PRESS_RSHIFT){  /* To process right shift */
        rval = 1;
        Rshift = 1;
        return rval;    /* return rval */
    }
    if(scode == RELEASE_LSHIFT){    /* To release left shift */
        rval = 1;
        Lshift = 0;
        return rval;    /* return the val */
    }
    if(scode == RELEASE_RSHIFT){    /* Release the right shift */
        rval = 1;
        Rshift = 0;
        return rval;    /* return the return value */
    }

    if(scode == PRESS_ALT){	/* press the alt key */
        rval = 1;
        Alt = 1;
        return rval;	/* return the rval */
    }

    if(scode == RELEASE_ALT){/* release the alt function */
        rval = 1;
        Alt = 0;
        return rval;	/* return the return value */
    }

    if(scode == PRESS_F1){	/* press F1 */
        rval = 1;
        F1 = 1;
        return rval;	/* return the return value */
    }

    
    if(scode == PRESS_F2){	/* press the F2 function key */
        rval = 1;	
        F2 = 1;
        return rval;	/* return the return value */
    }

    
    if(scode == PRESS_F3){	/* press the F3 function key */
        rval = 1;
        F3 = 1;
        return rval;	/* return the return value */
    }

    if(scode == RELEASE_F1){	/* release F1 */
        rval = 1;
        F1 = 0;
        return rval;	/* return the return value */
    }
    if(scode == RELEASE_F2){	/* release the F2 */
        rval = 1;
        F2 = 0;
        return rval;	/* return the rval */
    }

     if(scode == RELEASE_F3){/* release the F3 key */
        rval = 1;
        F3 = 0;
        return rval;
    }

    /* If it is bounded by the array */
    if(scode < KEYB_MAST_CHAR){ /* If the above case is not satisfied, we judge if it is a normal character */
        rval = 1;
        return rval;
    }
    /* return value */
    return rval;
}



