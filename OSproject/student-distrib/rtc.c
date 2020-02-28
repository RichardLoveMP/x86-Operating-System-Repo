/* rtc.c - Functions to interact with the 8259 rtc
 */

#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "tests.h"

#define FOUR 4
#define MAX_F 20 /* max frequency */
#define FIFTN 15
#define SIX 6
#define TWOO 2

//static int period[MAX_F];
//static int count[MAX_F];
//static volatile int interruptted[MAX_F];
static int period;
static int count;
static volatile int interruptted;

static int index = 0;
static int rtc_setup = 0;

/* int rtc_init(unsigned char rate)
 * Function : initialize the rtc, set it to period mode, set initial frequency
 * 				based on given rate (frequency=32768>>(rate-1))
 * Inputs:      rate : an unsigned char between 1 and 15
 * Return Value: none */
void rtc_init() {
	char prev_RB;
	cli();
	outb(STATUS_RB | DISABLE_NMI_CMD, RTC_PORT);		// disable NMI and select RB
	prev_RB = inb(CMOS_PORT);	// fetch RA
	outb(STATUS_RB | DISABLE_NMI_CMD, RTC_PORT);		// reset to RB, any read can set RTC to RD
	outb((prev_RB| TURN_ON_PERIODIC_INTERRUPT_CMD), CMOS_PORT); //rate is lower 4 bits in the lower byte.
	
	rtc_set_rate(FIFTN);		//set initial rate to 2Hz frequency=32768>>(15-1)=2
	sti();
}

/* void rtc_set_rate(int rate)
 * Function : set rtc to given rate
 * Inputs: rate, the rate of rtc
 * Return Value: none*/
void rtc_set_rate(int rate) {
	char prev_RA;
	//printf("rtc base frequency set to %d Hz\n",(int)32768>>(rate-1));
	if (rate>MAX_RTC_RATE) return;
	cli();
	outb(STATUS_RA | DISABLE_NMI_CMD,RTC_PORT);               // select register A, and disable NMI
    prev_RA = inb(CMOS_PORT);             // read the current value of register A
    outb(STATUS_RA | DISABLE_NMI_CMD,RTC_PORT);               // set the index again (a read will reset the index to register D)
    outb((prev_RA & 0xF0) | rate, CMOS_PORT);    //write only our rate to A. Note, rate is the bottom 4 bits. So remain top 4 bits the same using the mask 0xF0
	enable_irq(RTC_IRQ);	//unmask rtc
	sti();
}

/* void rtc_irq_handler()
 * Function : handle rtc interrupt
 * Inputs:  none
 * Return Value: none*/
 void rtc_irq_handler() {
	//int i = 0;
	//unsigned long flag;  /* flag */
	
    cli();  /* clear the interrupt */

	outb(STATUS_RC, RTC_PORT);	//select RC
	inb(CMOS_PORT);		//we don't care the interrupt value
	/*
	for (i = 0; i <= MAX_F; i++) {
		if (count[i]<period[i]) {
			count[i]+=1;	//increase count
		}
		else {
			count[i]=0;		//call test_interrupts at given frequency
			interruptted[i] = 1;
			//if (i==index) printf("set rtc frequency to %d Hz, %d \n",BASE_FREQUENCY/period[index],index);
			//test_interrupts();
			//printf("rtc responsed!!\n");
		}
	}
	*/
	if (count<period) {
		count+=1;	//increase count
	}
	else {
		count=0;		//call test_interrupts at given frequency
		interruptted = 1;
		//if (i==index) printf("set rtc frequency to %d Hz, %d \n",BASE_FREQUENCY/period[index],index);
		//test_interrupts();
		//printf("rtc responsed!!\n");
	}
	
    send_eoi(RTC_IRQ);	//send end of interrupt
	send_eoi(ICW3_SLAVE);
	sti();  /* restore the interrupt */
//	 printf("init rtc handler\n");
 }


/* void rtc_init_with_frequency(unsigned int frequency)
 * Function : set rtc to given frequency
 * Inputs: frequency, the frequency of rtc
 * Return Value:  -1 for failure and 0 for succeed*/
int rtc_init_with_frequency(int frequency) {
	//printf("rtc initialized with frequency %d\n",frequency);
	rtc_init();
	rtc_set_rate(SIX);		//set frequency to 1024Hz frequency=32768>>(6-1)=2
	int frequency_temp = BASE_FREQUENCY;
	period = BASE_FREQUENCY/frequency_temp;		//without virtualization, how many interrupt per given frequency
	/*
	index = 0;
	while (frequency_temp >= 1) {
		period[index] = BASE_FREQUENCY/frequency_temp;	//how many 
		index++;
		frequency_temp >>= 1;
	}
	*/
	if (rtc_set_frequency(frequency)) return -1;	//failure
	//memset(count, 0, MAX_F);
	count = 0;
	/*
	int i = 0;
	for (i=0; i<MAX_F; i++) {
		interruptted[i] = 0;
	}
	*/
	interruptted = 0;
	rtc_setup = 1;
	return 0;	//succeed*/
	
 }

/* void rtc_set_frequency(int index)
 * Function : set rtc to given frequency
 * Inputs: frequency, the frequency of rtc
 * Return Value: -1 for failure and 0 for succeed*/
 int rtc_set_frequency(int frequency) {
	//printf("set frequency to %d\n",frequency);
	if (frequency > BASE_FREQUENCY || frequency<=0 || not_power_2(frequency)) {
		printf("invalid frequency %d Hz\n",frequency);
		index = -1;
		return -1;	//failure
	}
	/*
	index = 0;
	int frequency_temp = BASE_FREQUENCY;
	while (frequency_temp != frequency) {
		index++;
		frequency_temp >>= 1;
	}
	*/
	period = BASE_FREQUENCY/frequency/2;	
	count = 0;
	interruptted = 0;
	//printf("set rtc frequency to %d, %d Hz\n",BASE_FREQUENCY/period[index],index);
	return 0;	//succeed*/
 }

/* void not_power_2(int frequency)
 * Function : check if the frequency is a power of two
 * Inputs: frequency, the frequency of rtc
 * Return Value: true if not a power of two, false if it is*/
int not_power_2(int frequency) {
	int count = 0;
	while (frequency>=1) {
		count += frequency & 0x01;
		frequency >>= 1;
	}
	if (count == 1) return 0;
	else return 1;
}


/* int read_rtc()
 * Function : read if an interrupt happend
 * Inputs: 
 *		fd : ignored
 *		buf: ignored
 *		nbytes: ignored
 * Return Value: 0 if interrupt happend*/
 int32_t read_rtc(int fd, void* buf, int nbytes) {
	 //interruptted[index] = 0;
	 //while (!interruptted[index]);
	 //int get_null_pointer = *((int*)buf);
	 //get_null_pointer = 0;
	 interruptted = 0;
	 while (!interruptted);
//	 printf("t");
	 return 0;
 }



/* int write_rtc()
 * Function : read if an interrupt happend
 * Inputs: none
 *		fd : ignored
 *		buf: the 4 byte int, the frequency rtc should be setted
 *		nbytes: always 4
 * Return Value:  -1 for failure and number of byte (4) for succeed*/
 int write_rtc(int fd, void *buf, int nbytes) {
	if(buf == NULL)
	 	return -1;\
	
	//if (rtc_set_frequency(*((int*)buf))) return -1;
	//printf("rtc write to %d\n",(*(int*) buf));
	if (rtc_set_frequency((*(int*) buf))) return -1;
	return FOUR;	//return number of byte (4)
 }
 
 
 /* int open_rtc()
 * Function : open a rtc
 * Inputs: none
 *		filename : ignored
 * Return Value:  -1 for failure and 0 for succeed*/
 int open_rtc(const uint8_t* filename){
	 //printf("rtc opened\n");
	 if (rtc_setup==0) 
		rtc_init_with_frequency(TWOO);
	cli();
	//calling_from_open_rtc=1;
  	sti();
	 return 0;
}



/* int close()
 * Function : close rtc file
 * Inputs: none
 *		fd : ignored
 * Return Value:  0 for succeed*/
int close_rtc(int fd) {
	return 0;
}
 
