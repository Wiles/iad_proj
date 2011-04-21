/*
 * File: time.c
 * Author: Hekar Khani, Thomas Kempton, Samuel Lewis
 * Description: Contains definitions of time and watch dog timer related fuctions
 * 
 * Some code based on the post at 
 * http://naturetm.com/?p=128
 */

#include "time.h"

const unsigned long WDT_FREQUENCY = MCLK_FREQUENCY / WDT_DIVIDER;

/*
 * Sets up the watchdog timer and sets the clock speed
 */
void initWatchdogTimer()
{
	//Setup watchdog timer
    WDTCTL = WDTPW + WDTTMSEL + WDTIS1;

	//Set speed to 1 MHz
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
}


/*
 * Does nothing for the provided number of seconds.
 * 
 * milli - number of milliseconds to do nothing for
 */
void milliDelay(unsigned long milli )
{
	if( milli == 0 )
	{
		return;
	}
	else
	{
		unsigned long start = millis();
		while( start + milli  > millis() );
	}
}

/*
 * Returns the time since startup in milliseconds
 */
unsigned long millis(){
  return wdtCounter / ((float)WDT_FREQUENCY / 1000);
}
