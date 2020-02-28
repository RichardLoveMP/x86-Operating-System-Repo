/* rtc.h - Defines used in rtc.c
 */
#include "lib.h"

#ifndef _RTC_H
#define _RTC_H

/*RTC IRO number*/
#define RTC_IRQ 8

/*base rtc frequency*/
#define BASE_FREQUENCY 1024

/*port definition */
#define RTC_PORT 0x70
#define CMOS_PORT 0x71

/* register used*/
#define STATUS_RA 0x0A
#define STATUS_RB 0x0B
#define STATUS_RC 0x0C
#define MAX_RTC_RATE 15

/* command */
#define DISABLE_NMI_CMD	0x80
#define TURN_ON_PERIODIC_INTERRUPT_CMD 0x40


/* initialize the rtc */
extern void rtc_init();

/* handle the irq */
extern void rtc_irq_handler();

/* initialize with frequency */
extern int rtc_init_with_frequency(int frequency);

/* set rtc frequency */
extern int rtc_set_frequency(int frequency);

/* eliminate the page fault */
extern void rtc_helper();

void rtc_set_rate(int rate);

int not_power_2(int frequency);

/* open close read write function */
extern int read_rtc(int fd, void* buf, int nbytes);

extern int write_rtc(int fd, void *buf, int nbytes);

extern int open_rtc(const uint8_t* filename);

extern int close_rtc(int fd);
#endif /* _RTC_H */
