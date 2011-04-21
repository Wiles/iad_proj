/*
 * File: time.h
 * Author: Hekar Khani, Thomas Kempton, Samuel Lewis
 * Description: provides indirect access to setting up the watchdog timer
 * and provides functions and marcros for accessing the watch dog timer trigger
 * count in milliseconds
 * 
 * It is expected the the watch dog timer vector will be defined elsewhere
 * and that it will increment wdtCounter every time it is called
 */

#ifndef TIME_H_
#define TIME_H_

#include "msp430g2231.h"

//Watchdog settings
#define MCLK_FREQUENCY 1000000
#define WDT_DIVIDER 512

#define WDT_TICKS_TO_HUNDS(x) ((x) / ((float)WDT_FREQUENCY / 1000)/10)
extern const unsigned long WDT_FREQUENCY;
extern unsigned long wdtCounter;

//Prototypes
unsigned long millis(void);
void initWatchdogTimer(void);
void milliDelay(unsigned long milli );

#endif /*TIME_H_*/

