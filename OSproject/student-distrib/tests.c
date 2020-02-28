#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "Interrupt_Descriptor_Table.h"
#include "keyboard.h"
#include "terminal.h"
#include "ROfilesystem.h"
#include "rtc.h"
#include "system_call.h"
/* various scancode of the test input */
#define PRESS_1 2
#define PRESS_6 7
#define PRESS_Q 16
#define PRESS_T 20
#define PRESS_A 30
#define PRESS_D 32
#define PRESS_Z 44

#define OHTE 128
#define TWL 12

#define PASS 1
#define FAIL 0

#define VIDEO_ADDRESS       0xB8000
#define KERNEL_ADDRESS      0x00400000
static volatile int count = 1;

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here

/* idt_test_divide0
 * 
 * divided by 0
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage:divide 0 exception
 * Files: Exception_handler.c/h
 */
int idt_test_divide0(){
	TEST_HEADER;
	int i=42;
	int zero=0;
	i=i/zero;
    return PASS;
}
/* idt_content_test
 * 
 *test the content of some bits in the interrupt gate
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage:divide 0 exception
 * Files: Exception_handler.c/h
 */

int idt_content_test(){
	TEST_HEADER;
	int i;
    for( i=0; i<NUM_VEC;i++){ // loop for all 256
        if (i<20){
             if(idt[i].reserved3!=1||idt[i].present!=1){
				 return FAIL;
			 }
        }
        if (i>=0x20&&i<=0x2F){ // if just a hard drive interrupt just change the R3,not change DPl
           //idt[i].reserved3=1;
           if(idt[i].present!=1){
			   return FAIL;
		   }
        }
        if(i==SYSTEM_CALL_VAECTOR){//if i ==0x80; the system call port change the DPL and R3
          if(idt[i].reserved3!=1||idt[i].dpl!=3||idt[i].present!=1){
			  return FAIL;
		  }
      
        }
        
        
    }
	return PASS;
}

/* keyboard scancode test
 * Input: None
 * Output: PASS
 * Side Effects: None
 * Coverage: scan code of keyboard
 * Files: keyboard.c/h
 */

//int keyboard_scancode_test(){
	/* test case 1 */
//	TEST_HEADER;
//	if(get_input(PRESS_1) != '1')
//		return FAIL;
	/* test case 2 */
//	if(get_input(PRESS_6) != '6')
//		return FAIL;
	/* test case 3 */
//	if(get_input(PRESS_A) != 'a')
//		return FAIL;
	/* test case 4 */
//	if(get_input(PRESS_D) != 'd')
//		return FAIL;
	/* test case 5 */
//	if(get_input(PRESS_Q) != 'q')
//		return FAIL;
//	/* test case 6 */
//	if(get_input(PRESS_Z) != 'z')
//		return FAIL;
	/* test case 7 */
//	if(get_input(PRESS_T) != 't')
//		return FAIL;
//	return PASS;
//}

/* rtc_test
 * Inputs: None
 * Side Effects: None
 * Coverage: test if rtc count time
 * Files: rtc.c/h
 */
int rtc_test(){
	TEST_HEADER;
	while (count);
    return PASS;
}

/* rtc_test_helper
 * Inputs: None
 * Side Effects: change count
 * Coverage: test if rtc count time
 * Files: rtc.c/h
 */
void rtc_test_helper(){
	count = 0;
}


/* dereferencing_incorrect_address()
 * Try to dereference an illegal address. a page-fault should happen.
 * Inputs: None
 * Outputs: bluescreen, and page-fault.
 * Side Effects: None
 * Files: paging.c, paging.h
 */
int dereferencing_incorrect_address()
{
	// note: this test may not appliable to Checkpoint 2,3,4,5
	TEST_HEADER;
	int result = PASS;
	int* addr = (int*)0x42;  // this is NOT a magic number --- Lumetta's luck number, which is also a good test example
	int val = *(addr);
	if (val == 0){
		return result;
	} else {
		return result;
	}
}


/* dereferencing_video_address()
 * Try to dereference an address in the video memory. This should be
 * a legal operation and no error should occur. 
 * Inputs: None
 * Outputs: PASS. Crash if something wrong
 * Side Effects: None
 * Files: paging.c, paging.h
 */
int dereferencing_video_address()
{
	// note: this test may not appliable to Checkpoint 2,3,4,5
	TEST_HEADER;
	int result = PASS;
	int* addr = (int*)(VIDEO_ADDRESS + 16);	
	int val = *(addr);

	if (val == 0){
		return result;
	} else {
		return result;
	}
}

/* dereferencing_kernel_address()
 * Try to dereference an address in the kernel, i.e., 0~4MB.
 * Inputs: None
 * Outputs: TEST_HEADER
 * Side Effects: PASS. Crash if something wrong
 * Files: paging.c, paging.h
 */
int dereferencing_kernel_address()
{
	// note: this test may not appliable to Checkpoint 2,3,4,5
	TEST_HEADER;
	int result = PASS;
	int* addr = (int*)KERNEL_ADDRESS;	
	int val = *(addr);

	if (val == 0){
		return result;
	} else {
		return result;
	}

}



/* dereferencing_null()
 * Try to dereference NULL. On no condition should it success.
 * Inputs: None
 * Outputs: if the test pass, you get bluescreen.
 * Side Effects: Crash, blue-screen
 * Coverage: Exception handlers
 * Files: paging.c, paging.h
 */
int dereferencing_null()
{
	// note: this test may not appliable to Checkpoint 2,3,4,5
	TEST_HEADER;
	int result = PASS;
	int* addr = NULL;
	int val = *(addr);
	if (val == 0){
		return result;
	} else {
		return result;
	}

}


/* paging_struct_test()
 * Test something in the structure of page directory and page table.
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: initialzation of paging
 * Files: paging.c, paging.h
 */
/*
int paging_struct_test()
{
	// note: this test may not appliable to Checkpoint 2,3,4,5
	TEST_HEADER;
	int result = PASS;
	if (page_directory[0].read_or_write != 1){
		result = FAIL;
//		assertion_failure();
	}
	if (page_directory[0].present != 1){
		result = FAIL;
//		assertion_failure();
	}
	if (page_directory[1].present != 1){
		result = FAIL;
//		assertion_failure();
	}
	if (page_directory[1].page_size == 0){
		result = FAIL;
	//	assertion_failure();
	}
	return result;

}*/


/* terminal_read_test
 * 
 * test   func   terminal R&W
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Files: terminal.c/h
 */
int terminal_read_test(){
	terminal_open(NULL);
	TEST_HEADER;
	
	int result = PASS;
	int rsize;
	int wsize;
	uint8_t arr[OHTE + 1];
	
	nputc('h');nputc('e');nputc('l');nputc('l');nputc('o');nputc(':');nputc('\n');
	// while(1){
		rsize = terminal_read(0,arr,OHTE);
	nputc('m');nputc('y');nputc(' ');nputc('n');nputc('a');nputc('m');nputc('e');nputc(' ');nputc('i');nputc('s');nputc(':');
		wsize = terminal_write(1,arr,OHTE);
	//}
	if(rsize != wsize)
		result = FAIL;
	terminal_close();
	return result;
}

/* Checkpoint 2 tests */

////////////////////////////////////////////////#include "ROfilesystem.h"   // also need to get bootblock addr in kernel.c

/* test_read_dentry_by_name
 * 
 * test   func   read dentry by name     // we will test three exist file name and the other not
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Files: ROfilesystem.c/h
 */
int test_read_dentry_by_name(char* fname) {
	TEST_HEADER;
    dentry_t temdent;
    if(-1 == read_dentry_by_name((uint8_t*)fname, &temdent)){ // if can not read dentry by name return fail
		return FAIL;
	}
	if(strncmp(fname,".",1)==0){// file "shellotimg"  who has 23 inodes in index2
		if(temdent.inode_num!=0){
			return FAIL;
		}
	}
	if(strncmp(fname,"shell",5)==0){// file "shellotimg"  who has 23 inodes in index2  5 is size of the string "shell"
		if(temdent.inode_num!=23){
			return FAIL;
		}
	}
    if(strncmp(fname,"syserr",6)==0){// file "syserr"  who has 27 inodes in index4 , 6 is the size of the string "syserr"
		if(temdent.inode_num!=27){ 
			return FAIL;
		}
	}

	return PASS;
	
}


/* test_read_dentry_by_index
 * 
 * test   func   read dentry by index     will test 3 file with index 0,2,4and one not exist index
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Files: ROfilesystem.c/h
 */
int test_read_dentry_by_index(uint32_t index) {
	TEST_HEADER;
    dentry_t temdent;
    if(-1 == read_dentry_by_index(index, &temdent)){   // if can not read dentry by name return fail
		return FAIL;
	}
	//printf("num is %d\n",temdent.inode_num);
	//printf("name2 is %s\n",temdent.file_name);
	//printf("name is %s\n",temdent.file_name);
	if(index==0){// 0 is the index of file "." who has 0 inodes
		if(strncmp(temdent.file_name,".",1)!=0||temdent.inode_num!=0){
			return FAIL;
		}
	}
	if(index==2){// 2 is the index of file "shellotimg" who has 23 inodes
		if(strncmp(temdent.file_name,"shell",5)!=0||temdent.inode_num!=23){	// 5 is size of the string "shell"
			return FAIL;
		}
	}
	if(index==4){// 4 is the index of file "syserr"  who has 27 inodes
		if(strncmp(temdent.file_name,"syserr",6)!=0||temdent.inode_num!=27){// 6 is the size of the string "syserr"
			return FAIL;
		}
	}
	//for(i = 0; i < 3; i++) putc(buf[i]);
	return PASS;
}



/* test_read_data
 * 
 * test   func  read_data(called by file read) also file open, close,read
 * Inputs: file name 
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Files: ROfilesystem.c/h
 */
/*
int test_read_data(char* fname){
	int i;
	uint32_t bufsize;
	inode_t* index_nodesss;
	
	// printf("\n\n");
	
	if (0!=file_open(fname)){
		return FAIL;//  open the file 
	}
	
	clear();
	index_nodesss = (inode_t*) bootblockptr + (1 + tempdentry.inode_num);// get the addr of inode 
	bufsize=index_nodesss->length;//get the data size of inode  
	uint8_t buf[bufsize]; 
	
	if(-1==file_read(buf,bufsize)){
		return -1;  // read file failed
	}
	
	read_data(tempdentry.inode_num,0,buf,bufsize);
	//printf("datahere:\n");
	for(i = 0; i < bufsize; i++) nputc(buf[i]);   // print all char in the buf
	//for(i=4096;i<bufsize;i++) putc(buf[i]);
	//printf("dataend");
	nputc('\n');
	//for (i=0;i<32;i++) nputc(fname[i]);  // 32 is the max length of file name
	printf("File name: %s",fname);
	
	if (0!=file_close(fname)){
		return FAIL;// close file
	}
	
	return PASS;
}
*/



/* test_read_directory
 * 
 * test   func  read dentry and print the  all file name and file type and size
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Files: ROfilesystem.c/h
 */
/*
int test_read_directory() {
	TEST_HEADER;
	int i,j;
	inode_t* index_nodesss;
	if (0!=directory_open(".")){
		return FAIL;// if open
	}
	// printf("\n\n");
	 // clear();
	for (i=0;i<17;i++){ // 17 is the num of dentry
		uint8_t buf[length_32B_to_int8];//32 is the max length of file name
	    directory_read(buf, length_32B_to_int8,i);
		printf("File name:");
		for(j=0;j<length_32B_to_int8;j++){//32 is the max length of file name 
			nputc(buf[j]);
		}
		index_nodesss = (inode_t*) bootblockptr + (1 + tempdentry.inode_num);
		printf("file type: %d, file size:%d ",tempdentry.file_type,index_nodesss->length);// get the file size and type 
		printf("\n");
	}

	if (0!=directory_close(".")){
		return FAIL;
	}
	return PASS;
}
*/

/* read_and_print_file_by_index
 * 
 * read_and_print_and _close the pre index num files  if index num ==17 means print all file here
 * Inputs: None
 * Outputs: none
 * Side Effects: None
 * Files: ROfilesystem.c/h
 */
/*
void read_and_print_file_by_index(uint32_t index){
	char* fname; 
	fname=bootblockptr->dir_entry[index].file_name;
	test_read_data(fname);

	
}
*/


/* Checkpoint 2 tests */

/* void rtc_test_ckpt2()
 * Test rtc functionality.
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: rtc change frequency
 * Files: rtc.c, rtc.h
 */
/*
int rtc_test_ckpt2(){
	TEST_HEADER;
	int wait = 1;
	int temp = wait;
	int frequency = 1;
	open_rtc(0);
	write_rtc(0, &frequency, 4);	// 4 means the frequency's size is 4 byte
	//test if rtc can set frequenc for valid input
	while (frequency<=BASE_FREQUENCY) {	//1024Hz is the max frequency, user program can reach
		//wait for rtc response
		write_rtc(0, &frequency, 4);// 4 means the frequency's size is 4 byte
		while(temp>0) {
			if (0 == read_rtc(0, 0, 0)) {
				nputc('I');
				temp--;
			}
		}
		wait *= 2;
		temp = wait;
		frequency *= 2;
	}
	//test invalid input
	
	nputc('\n');

	//case 1: not power of 2
	frequency = 111; // invalid frequency 111 
	int ret = 0;
	ret = write_rtc(0, &frequency, 4);
	printf("return value : %d\n", ret);
	//case 2: exceed maximum frequency
	frequency = 2048;	// 2048 is an example which is higher than frequency 
	ret = write_rtc(0, &frequency, 4);	// 4 means the frequency's size is 4 byte
	printf("return value : %d\n", ret);
	
	close_rtc(0);
	return PASS;
}
*/


/* void terminal_write_test()
 * Test read & write functionality.
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: test the small and large stirng test
 * Files: terminal.c/h
 */
/*
int terminal_write_test(){
	Open_Terminal();
	uint8_t str_write_short[19]; // 19 is a size which is less than 128 
	uint8_t str_write_long[1050];	//1050 is a pretty large test longer than 128 
	int i;
	int j;
	int cnt = 0;
	
	printf("TEST1: simplified test \n");
	// 0 - 12 means the index of the string "I LOVE ECE391" 
	str_write_short[0] = 'I'; str_write_short[1] = ' '; str_write_short[2] = 'L';str_write_short[3] = 'O';str_write_short[4] = 'V';str_write_short[5] = 'E';
	str_write_short[6] = ' '; str_write_short[7] = 'E'; str_write_short[8] = 'C';str_write_short[9] = 'E';str_write_short[10] = '3';str_write_short[11] = '9';
	str_write_short[12] = '1';

	if(terminal_write(0,str_write_short,13) != 13)	// 13 means the length of the string we've written 
		return FAIL;
	printf("\n");
	printf("write simple is PASSED \n");
	
	for(i = 0; i < 13;i++){	// 13 means the length of the string 
		for(j = 0; j < 80; j++){	// 80 means the width of the screen 
			str_write_long[cnt] = str_write_short[i];
			cnt++;
		}
	}
	terminal_close();
	if(terminal_write(0,str_write_long,13 * 80) != 13 * 80)	// 13 * 80 means the total # characters we will write 
		return FAIL;
	return PASS;
}
*/

/* Checkpoint 3 tests */

/* Checkpoint 4 tests */
/* void rtc_test_ckpt2()
 * Test rtc functionality.
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: rtc change frequency
 * Files: rtc.c, rtc.h
 */

 
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
//	test_read_data("verylargetextwithverylongname.tx");
//	TEST_OUTPUT("idt_test", idt_test());
//	TEST_OUTPUT("idt_content_test", idt_content_test());
//	TEST_OUTPUT("rtc_test", rtc_test());
//  TEST_OUTPUT("idt_test_divide0", idt_test_divide0());
//	TEST_OUTPUT("test keyboard input", keyboard_scancode_test());	
//	TEST_OUTPUT("dereferencing_incorrect_address", dereferencing_incorrect_address());
//	TEST_OUTPUT("dereferencing_video_address", dereferencing_video_address());
//	TEST_OUTPUT("dereferencing_kernel_address", dereferencing_kernel_address());
//	TEST_OUTPUT("dereferencing_null", dereferencing_null());
//	TEST_OUTPUT("paging_struct_test", paging_struct_test());
//------------------------------------------------------------------------------------	

//check point 2 
// TEST_OUTPUT("terminal_read_test", terminal_read_test());


//below 4 for read_dentry_by_index's test
//TEST_OUTPUT("test1_read_dentry_by_index", test_read_dentry_by_index(0));
//TEST_OUTPUT("test2_read_dentry_by_index", test_read_dentry_by_index(2));
//TEST_OUTPUT("test3_read_dentry_by_index", test_read_dentry_by_index(4));
     // we know by test that the num of dentries is 17, just choose a 42 which is bigger than 17 to test for invalid   
//TEST_OUTPUT("test4_read_dentry_by_index", test_read_dentry_by_index(42));



//below 4 for read_dentry_by_name's test
//TEST_OUTPUT("test1_read_dentry_by_name", test_read_dentry_by_name("."));
//TEST_OUTPUT("test2_read_dentry_by_name", test_read_dentry_by_name("syserr"));
//TEST_OUTPUT("test3_read_dentry_by_name", test_read_dentry_by_name("shell"));
//TEST_OUTPUT("test4_read_dentry_by_name", test_read_dentry_by_name("nosuchfile"));

//below for read_data's test
// test_read_data("frame0.txt");
//verylargetextwithverylongname.tx
/*	int i ;
	for(i = 0; i < 17; i++){
		//if(i == 11)
		//	continue;
		read_and_print_file_by_index(i);//the index shoulde smaller than 16,//17 is total num of files  
	}
*/
	// read_and_print_file_by_index(10);
// test_read_directory();
// test for ckpt 2*/
// TEST_OUTPUT("rtc_test_ckpt2", rtc_test_ckpt2());
//	 TEST_OUTPUT("terminal_write_test",terminal_write_test());
// launch your tests here
//	sys_execute((uint8_t*)"shell");
	//read_dentry_by_name((uint8_t*)"shell",&tempdentry);
	//printf("%d\n",tempdentry.inode_num);
}

