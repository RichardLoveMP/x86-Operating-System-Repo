/* lib.c - Some basic library functions (printf, strlen, etc.)
 * vim:ts=4 noexpandtab */

#include "lib.h"
#include "terminal.h"

#define NAME_LENGTH 32
#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25
#define ATTRIB      0x7

#define ADDR_BIAS_PROG   0x08000000
#define ADDR_BIAS_4MB    0x400000

static int screen_x;
static int screen_y;
static char* video_mem = (char *)VIDEO;

/* roll()
 * INPUT: None
 * Output: NONE
 * Functionality: Roll the screen up by one line
 * and clear the last line
 * SIDE EFFECT: Modify the video memory to achieve this function's goal 
 */
void roll(){
    uint32_t i;
    /* roll the screen up */
	for(i=0; i<(NUM_COLS * (NUM_ROWS -1)* 2); i++)  // clear it 
	{
		video_mem[i] = video_mem[i+NUM_COLS*2];// modify hte video memory
	}
    /* empty the last line */
	for(i=0;i<NUM_COLS;i++){
		video_mem[(NUM_ROWS-1)*NUM_COLS*2+i*2]=0;   // set the last line to empty
		video_mem[(NUM_ROWS-1)*NUM_COLS*2+i*2+1]= (ATTRIB | 0xF0);    // set the attribute of the color
	}
    return ;
}

/* write_pos()
 * INPUT: x positon, y position and the write char 
 * Output: NONE
 * Functionality: Write the character to the given position we have 
 * and clear the last line
 * SIDE EFFECT: Modify the video memory to achieve the goal 
 */
void write_pos(int x,int y, uint8_t ch){
    int32_t i;
    i = x + (y * NUM_COLS);
    *(uint8_t *)(video_mem + (i << 1)) = ch;    /* Very like clear a postion and clear */
    *(uint8_t *)(video_mem + (i << 1) + 1) = (ATTRIB | 0xF0);
}


/* write_pos()
 * INPUT: x positon, y position and the write char and the color
 * Output: NONE
 * Functionality: Write the character to the given position we have 
 * and clear the last line
 * SIDE EFFECT: Modify the video memory to achieve the goal 
 */
void write_pos_with_color(int x,int y, uint8_t ch, uint8_t color){
    int32_t i;
    i = x + (y * NUM_COLS);
    *(uint8_t *)(video_mem + (i << 1)) = ch;    /* Very like clear a postion and clear */
    *(uint8_t *)(video_mem + (i << 1) + 1) = (color | 0xF0);
}


/* clear_pos()
 * INPUT: x positon, y position
 * Output: NONE
 * Functionality: Write the EMPTY character to the given position we have 
 * and clear the last line
 * SIDE EFFECT: Modify the video memory to achieve the goal 
 */
void clear_pos(int x,int y){
    int32_t i;
    i = x + (y * NUM_COLS);
    *(uint8_t *)(video_mem + (i << 1)) = ' '; /* Very like clear a postion and clear */
    *(uint8_t *)(video_mem + (i << 1) + 1) = (ATTRIB | 0xF0);
}


/* void clear(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears video memory */
void clear(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = (ATTRIB | 0xF0);
    }
}

/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output. */
int32_t printf(int8_t *format, ...) {

    /* Pointer to the format string */
    int8_t* buf = format;

    /* Stack pointer for the other parameters */
    int32_t* esp = (void *)&format;
    esp++;

    while (*buf != '\0') {
        switch (*buf) {
            case '%':
                {
                    int32_t alternate = 0;
                    buf++;

format_char_switch:
                    /* Conversion specifiers */
                    switch (*buf) {
                        /* Print a literal '%' character */
                        case '%':
                            putc('%');
                            break;

                        /* Use alternate formatting */
                        case '#':
                            alternate = 1;
                            buf++;
                            /* Yes, I know gotos are bad.  This is the
                             * most elegant and general way to do this,
                             * IMHO. */
                            goto format_char_switch;

                        /* Print a number in hexadecimal form */
                        case 'x':
                            {
                                int8_t conv_buf[64];
                                if (alternate == 0) {
                                    itoa(*((uint32_t *)esp), conv_buf, 16);
                                    puts(conv_buf);
                                } else {
                                    int32_t starting_index;
                                    int32_t i;
                                    itoa(*((uint32_t *)esp), &conv_buf[8], 16);
                                    i = starting_index = strlen(&conv_buf[8]);
                                    while(i < 8) {
                                        conv_buf[i] = '0';
                                        i++;
                                    }
                                    puts(&conv_buf[starting_index]);
                                }
                                esp++;
                            }
                            break;

                        /* Print a number in unsigned int form */
                        case 'u':
                            {
                                int8_t conv_buf[36];
                                itoa(*((uint32_t *)esp), conv_buf, 10);
                                puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a number in signed int form */
                        case 'd':
                            {
                                int8_t conv_buf[36];
                                int32_t value = *((int32_t *)esp);
                                if(value < 0) {
                                    conv_buf[0] = '-';
                                    itoa(-value, &conv_buf[1], 10);
                                } else {
                                    itoa(value, conv_buf, 10);
                                }
                                puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a single character */
                        case 'c':
                            nputc((uint8_t) *((int32_t *)esp));
                            esp++;
                            break;

                        /* Print a NULL-terminated string */
                        case 's':
                            puts(*((int8_t **)esp));
                            esp++;
                            break;

                        default:
                            break;
                    }

                }
                break;

            default:
                nputc(*buf);
                break;
        }
        buf++;
    }
    return (buf - format);
}

/* int32_t puts(int8_t* s);
 *   Inputs: int_8* s = pointer to a string of characters
 *   Return Value: Number of bytes written
 *    Function: Output a string to the console */
int32_t puts(int8_t* s) {
    register int32_t index = 0;
    while (s[index] != '\0') {
        nputc(s[index]);
        index++;
    }
    return index;
}

/* void putc(uint8_t c);
 * Inputs: uint_8* c = character to print
 * Return Value: void
 *  Function: Output a character to the console */
void putc(uint8_t c) {
    if(c == '\n' || c == '\r') {
        screen_y++;
        screen_x = 0;
    } else {
        *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = ATTRIB;
        screen_x++;
        screen_x %= NUM_COLS;
        screen_y = (screen_y + (screen_x / NUM_COLS)) % NUM_ROWS;
    }
}

/* int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix);
 * Inputs: uint32_t value = number to convert
 *            int8_t* buf = allocated buffer to place string in
 *          int32_t radix = base system. hex, oct, dec, etc.
 * Return Value: number of bytes written
 * Function: Convert a number to its ASCII representation, with base "radix" */
int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix) {
    static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int8_t *newbuf = buf;
    int32_t i;
    uint32_t newval = value;

    /* Special case for zero */
    if (value == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }

    /* Go through the number one place value at a time, and add the
     * correct digit to "newbuf".  We actually add characters to the
     * ASCII string from lowest place value to highest, which is the
     * opposite of how the number should be printed.  We'll reverse the
     * characters later. */
    while (newval > 0) {
        i = newval % radix;
        *newbuf = lookup[i];
        newbuf++;
        newval /= radix;
    }

    /* Add a terminating NULL */
    *newbuf = '\0';

    /* Reverse the string and return */
    return strrev(buf);
}

/* int8_t* strrev(int8_t* s);
 * Inputs: int8_t* s = string to reverse
 * Return Value: reversed string
 * Function: reverses a string s */
int8_t* strrev(int8_t* s) {
    register int8_t tmp;
    register int32_t beg = 0;
    register int32_t end = strlen(s) - 1;

    while (beg < end) {
        tmp = s[end];
        s[end] = s[beg];
        s[beg] = tmp;
        beg++;
        end--;
    }
    return s;
}

/* uint32_t strlen(const int8_t* s);
 * Inputs: const int8_t* s = string to take length of
 * Return Value: length of string s
 * Function: return length of string s */
uint32_t strlen(const int8_t* s) {
    register uint32_t len = 0;
    while (s[len] != '\0')
        len++;
    return len;
}

/* void* memset(void* s, int32_t c, uint32_t n);
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set n consecutive bytes of pointer s to value c */
void* memset(void* s, int32_t c, uint32_t n) {
    c &= 0xFF;
    asm volatile ("                 \n\
            .memset_top:            \n\
            testl   %%ecx, %%ecx    \n\
            jz      .memset_done    \n\
            testl   $0x3, %%edi     \n\
            jz      .memset_aligned \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            subl    $1, %%ecx       \n\
            jmp     .memset_top     \n\
            .memset_aligned:        \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            movl    %%ecx, %%edx    \n\
            shrl    $2, %%ecx       \n\
            andl    $0x3, %%edx     \n\
            cld                     \n\
            rep     stosl           \n\
            .memset_bottom:         \n\
            testl   %%edx, %%edx    \n\
            jz      .memset_done    \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            subl    $1, %%edx       \n\
            jmp     .memset_bottom  \n\
            .memset_done:           \n\
            "
            :
            : "a"(c << 24 | c << 16 | c << 8 | c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memset_word(void* s, int32_t c, uint32_t n);
 * Description: Optimized memset_word
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set lower 16 bits of n consecutive memory locations of pointer s to value c */
void* memset_word(void* s, int32_t c, uint32_t n) {
    asm volatile ("                 \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            cld                     \n\
            rep     stosw           \n\
            "
            :
            : "a"(c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memset_dword(void* s, int32_t c, uint32_t n);
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set n consecutive memory locations of pointer s to value c */
void* memset_dword(void* s, int32_t c, uint32_t n) {
    asm volatile ("                 \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            cld                     \n\
            rep     stosl           \n\
            "
            :
            : "a"(c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memcpy(void* dest, const void* src, uint32_t n);
 * Inputs:      void* dest = destination of copy
 *         const void* src = source of copy
 *              uint32_t n = number of byets to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of src to dest */
void* memcpy(void* dest, const void* src, uint32_t n) {
    asm volatile ("                 \n\
            .memcpy_top:            \n\
            testl   %%ecx, %%ecx    \n\
            jz      .memcpy_done    \n\
            testl   $0x3, %%edi     \n\
            jz      .memcpy_aligned \n\
            movb    (%%esi), %%al   \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            addl    $1, %%esi       \n\
            subl    $1, %%ecx       \n\
            jmp     .memcpy_top     \n\
            .memcpy_aligned:        \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            movl    %%ecx, %%edx    \n\
            shrl    $2, %%ecx       \n\
            andl    $0x3, %%edx     \n\
            cld                     \n\
            rep     movsl           \n\
            .memcpy_bottom:         \n\
            testl   %%edx, %%edx    \n\
            jz      .memcpy_done    \n\
            movb    (%%esi), %%al   \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            addl    $1, %%esi       \n\
            subl    $1, %%edx       \n\
            jmp     .memcpy_bottom  \n\
            .memcpy_done:           \n\
            "
            :
            : "S"(src), "D"(dest), "c"(n)
            : "eax", "edx", "memory", "cc"
    );
    return dest;
}

/* void* memmove(void* dest, const void* src, uint32_t n);
 * Description: Optimized memmove (used for overlapping memory areas)
 * Inputs:      void* dest = destination of move
 *         const void* src = source of move
 *              uint32_t n = number of byets to move
 * Return Value: pointer to dest
 * Function: move n bytes of src to dest */
void* memmove(void* dest, const void* src, uint32_t n) {
    asm volatile ("                             \n\
            movw    %%ds, %%dx                  \n\
            movw    %%dx, %%es                  \n\
            cld                                 \n\
            cmp     %%edi, %%esi                \n\
            jae     .memmove_go                 \n\
            leal    -1(%%esi, %%ecx), %%esi     \n\
            leal    -1(%%edi, %%ecx), %%edi     \n\
            std                                 \n\
            .memmove_go:                        \n\
            rep     movsb                       \n\
            "
            :
            : "D"(dest), "S"(src), "c"(n)
            : "edx", "memory", "cc"
    );
    return dest;
}

/* int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
 * Inputs: const int8_t* s1 = first string to compare
 *         const int8_t* s2 = second string to compare
 *               uint32_t n = number of bytes to compare
 * Return Value: A zero value indicates that the characters compared
 *               in both strings form the same string.
 *               A value greater than zero indicates that the first
 *               character that does not match has a greater value
 *               in str1 than in str2; And a value less than zero
 *               indicates the opposite.
 * Function: compares string 1 and string 2 for equality */
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n) {
    int32_t i;
    for (i = 0; i < n; i++) {
        if ((s1[i] != s2[i]) || (s1[i] == '\0') /* || s2[i] == '\0' */) {

            /* The s2[i] == '\0' is unnecessary because of the short-circuit
             * semantics of 'if' expressions in C.  If the first expression
             * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
             * s2[i], then we only need to test either s1[i] or s2[i] for
             * '\0', since we know they are equal. */
            return s1[i] - s2[i];
        }
    }
    return 0;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src)
 * Inputs:      int8_t* dest = destination string of copy
 *         const int8_t* src = source string of copy
 * Return Value: pointer to dest
 * Function: copy the source string into the destination string */
int8_t* strcpy(int8_t* dest, const int8_t* src) {
    int32_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
 * Inputs:      int8_t* dest = destination string of copy
 *         const int8_t* src = source string of copy
 *                uint32_t n = number of bytes to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of the source string into the destination string */
int8_t* strncpy(int8_t* dest, const int8_t* src, uint32_t n) {
    int32_t i = 0;
    while (src[i] != '\0' && i < n) {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return dest;
}

/* void test_interrupts(void)
 * Inputs: void
 * Return Value: void
 * Function: increments video memory. To be used to test rtc */
void test_interrupts(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        video_mem[i << 1]++;
    }
}
/* void bluescreen()
 * Inputs: void
 * Return Value: void
 * Function:turn the all screen to blue(0x16)
 */
void bluescreen(){
    int i;
    for(i=0; i<NUM_COLS*NUM_ROWS;i++){
        *(uint8_t *)(VIDEO+(i<<1)+1)=BLUECOLOR;
    }
}

/* int8_t* strcpynon_blank(int8_t* dest, const int8_t* src)
 * Inputs:      int8_t* dest = destination string of copy
 *         const int8_t* src = source string of copy
 *                uint32_t n = number of bytes to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of the source string into the destination string */
int8_t strcpy_non_blank(uint8_t* dest, const uint8_t* src) {
	int start = 0;
	if (src[start]=='\0') return 0;
	while (src[start] == ' ') {
		start++;			//strip leading spaces
	}
	int end_name = 0;
	int end = start;
	while (src[end] != '\0') {
		if (src[end]==' ') {
			end_name = end-1;
			break;
			}
		end++;				//find the end of commands
	}
    if (src[end]=='\0') end_name = end-1;
	memset(dest,0,NAME_LENGTH+1);
	memcpy(dest, src+start, end_name-start+1);		
	return end;	//return a offset in the string
}


/* int8_t* strcpy_strip_blank(int8_t* dest, const int8_t* src)
 * Inputs:
 * Return Value: pointer to dest
 * Function: copy n bytes of the source string into the destination string 
 * */
int8_t strcpy_strip_blank(uint8_t* dest, const uint8_t* src) {
	int start = 0;
	if (src[start]=='\0') return 0;
	while (src[start] == ' ') {
		start++;			//strip leading spaces
	}
	int end_name = 0;
	int end = start;
	while (src[end] != '\0') {
		if (src[end]=='\0' || src[end]=='\n') {
			end_name = end-1;
			break;
			}
		end++;				//find the end of commands
	}
	memset(dest,0,NAME_LENGTH+1);
	memcpy(dest, src+start, end_name-start+1);		
	return end;	//return a offset in the string
}



/* get user program name
 * INPUTS: user program name and the command
 * OUTPUT: vector 
 * RETURN VALUE: the vector to the target
 * SIDE EFFECTS: make the copy of the user program and command 
 */
 int get_user_program_name(char* user_program_name, const char* command){
     int i = 0 ;
     int parse_pos = 0;
     int count = 0;
     //printf("%s|\n", command);
     int lencommand = strlen(command);
     while(command[i] == ' '){
         command++;
         count++;
         if (*command == 0) break;
     }
     lencommand = strlen(command);
     for(i=0; i<lencommand; i++){
        if(command[i] == ' ' || command[i] == '\0'){
            user_program_name[i] = '\0';
            break;
        } user_program_name[i] = command[i];
    }

    //printf("user program name: %s|\n",user_program_name);
    //("user args: %s|\n",command+i);
        
    parse_pos = i;
    for(; parse_pos<lencommand; parse_pos++) {
        if (command[parse_pos] != '\0' && command[parse_pos] != '\n' && command[parse_pos] != ' ') break;
    }
    //printf("user args: %d|\n",i);
    //printf("user arg first character: %c|",command[i]);
    return parse_pos+count;
 }
    




/* int8_t* check_address_available(const void* addr, int32_t len)
 * Inputs:      const void* addr  required address
 *               int32_t len      how long the content is
 * Return Value: ok or not ok. 0:ok, 1:not ok
 * Function: test if an address is ok to use
 */

int32_t check_address_available(const void* addr, int32_t len){
    uint32_t address = (uint32_t) addr;
    if (address >= ADDR_BIAS_PROG){
     //   if (address + (uint32_t)len >= ADDR_BIAS_PROG){
            if (address <= ADDR_BIAS_PROG + ADDR_BIAS_4MB){
                if (address + (uint32_t)len <= ADDR_BIAS_PROG + ADDR_BIAS_4MB){
                    // check the start address and end address (i.e., len+addr)
                    return 0;
                } else { return 1; }
            } else { return 1; }
     //   } else { return 1; }
    } else { return 1; }

}

	
/* 
 * copy_from_user
 *   DESCRIPTION: copy a period of memory from user space 
 *   INPUTS: to: destinaton of memory
             from: source of memory
             len: the length to copy
 *   OUTPUTS: None
 *   RETURN VALUE: 0 for failure, a positvie number for success (this number is same as len)
 *   SIDE EFFECTS: copy the memory
 */


int32_t copy_from_user(void *to, void *from, int32_t length){

	if(check_address_available(from, length)){
        return 0;
    }
    if (length > ADDR_BIAS_4MB){
        // too big a block to copy...
        return 0;
    }
	memcpy(to, from, length);
	return length;

}
	



/* 
 * copy_to_user
 *   DESCRIPTION: copy a period of memory to user space 
 *   INPUTS: to: destinaton of memory
             from: source of memory
             len: the length to copy
 *   OUTPUTS: None
 *   RETURN VALUE: 0 for failure, a positvie number for success (this number is same as len)
 *   SIDE EFFECTS: copy the memory
 */


int32_t copy_to_user(void *to, void *from, int32_t length){

    //if(check_address_available(to, length)){
   //     return 0;   
   // }
        if (length > ADDR_BIAS_4MB){
            // too big a block to copy...
            return 0;
        }
    memcpy(to, from, length);
    return length;
}


/* 
 * SET_TSS
 *   DESCRIPTION: set the tss 
 *   INPUTS: tss
 *   OUTPUTS: None
 *   RETURN VALUE: NONE
 *   SIDE_EFFECT: modify the tss
 */
void SET_TSS(tss_t* my_tss,uint32_t dat){
	my_tss->esp0 = dat;
	my_tss->ss0 = KERNEL_DS;
    return ;
}

/* 
 * SET_TS
 *   DESCRIPTION: set the tss for halt
 *   INPUTS: tss
 *   OUTPUTS: None
 *   RETURN VALUE: NONE
 *   SIDE_EFFECT: modify the tss
 */
void SET_TS(tss_t* my_tss,uint32_t dat){
	my_tss->esp0 = dat;
    return ;
}


