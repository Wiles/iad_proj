/*
 * File: main.c
 * Author: Hekar Khani, Thomas Kempton, Samuel Lewis
 * Description: Race timer.
 * Can be started, stopped, and reset using the input buttons
 * continually reads out the current race time to serial
 * for display on an external device.
 */

#include "msp430g2231.h"
#include "buttons.h"
#include "serial.h"
#include "time.h"

//Uneve number used to provide nice blur on the lower digits while displaying.
#define UPDATE_DELAY 73

//Clock globals
#define CMD_START_LENGTH 0
#define CMD_END_LENGTH 1
#define TIME_LENGTH 11
const char timeMin[] = "00:00:00.00";
char time[TIME_LENGTH + 1] = "00:00:00.00";
const char timeMax[] = "99:59:59.99";
char commandStart[CMD_END_LENGTH + 1] = "\r";
char commandEnd[CMD_START_LENGTH + 1] = "";
unsigned long hundCounted = 0;
unsigned long runWDTTicks = 0;
//------------------------------------------------------------------------------
// Function prototypes
//------------------------------------------------------------------------------
void updateTime(void);
char * strncpy ( char * destination, const char * source, int num );
void initSerial(void);
//------------------------------------------------------------------------------
// main()
//------------------------------------------------------------------------------
void main(void)
{	
	initWatchdogTimer();
	//Button interrupts
	//All pins output except rx and buttons
	
	initSerial();
	//Enable interrupt on button pins
	initButtons();
	
    __enable_interrupt();
    
    //Enable reading of buttons
	enableButtons();
	
    //Sync timers
    TimerA_UART_init();
    
    //calibrate baud rate
    TimerA_UART_print("U");
    
    //Wait for uVGA to startup
    milliDelay(1000);
    
    unsigned long lastUpdate = 0;
    
    for (;;)
    {               
    	//Update clock output when needed
        if( ( millis() - lastUpdate) > UPDATE_DELAY)
        {
        	lastUpdate = millis();
        	updateTime();
        	
        	//Disable buttons during serial communication. They seem to cause timing issues
			disableButtons();
			//Reinitilize timer to ensure syncing
			TimerA_UART_init();
			//Write Full command string
        	TimerA_UART_print_array(commandStart, CMD_START_LENGTH);
        	TimerA_UART_print_array(time, TIME_LENGTH);
        	TimerA_UART_print_array(commandEnd, CMD_END_LENGTH);
        	
        	//Turn buttons back on.
        	enableButtons();        	
        }
    }
}

/*
 * Increments the ASCII timer to reflect the length of the run
 * Resets the timer to 0 when both buttons are pushed
 */
void updateTime()
{
  if( getButtons() == BUTTON_RESET )
  {
    strncpy( time, timeMin, 11 );
    setButtons(BUTTON_STOP);
    
    //Clean up extra milliseconds
    runWDTTicks = 0;
    hundCounted = 0;
    
    return;
  }
  //Convert run time to hundredths of a second
  unsigned long hunds = WDT_TICKS_TO_HUNDS(runWDTTicks);
  
  //This loop updates the ASCII clock the correct amount
  while( hunds != hundCounted )
  {
  	++hundCounted;
    if( ++time[10] > timeMax[10] )
    {
      time[10] = '0';
      if( ++time[9] > timeMax[9] )
      {
        time[9]='0';
        if( ++time[7] > timeMax[7] )
        {
          time[7] = '0';
          if( ++time[6] > timeMax[6] )
          {
            time[6] = '0';
            if( ++time[4] > timeMax[4] )
            {
              time[4] = '0';
              if( ++time[3] > timeMax[3] )
              {
                time[3] = '0';
                if( ++time[1] > timeMax[1] )
                {
                  time[1] = '0';
                  if( ++time[0] > timeMax[0] )
                  {
                    time[0] = '0';
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}



/*
 * Increments the global counter each time the watchdog is called.
 * Also increments current run tickcount when in the correct state
 */
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void){
  wdtCounter++;
  if( getButtons() == BUTTON_START )
  {
  	++runWDTTicks;
  }
}
