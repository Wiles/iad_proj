/**
* Pins 1.3 and 1.6 will be used for switches.
*/

#include "msp430g2231.h"

#define HARDWARE_DEBOUNCE

#define ONE_SECOND_TENTH 3276
#define UPDATE_DELAY 100

#define BUTTON_START BIT3
#define BUTTON_STOP BIT6
#define BUTTON_RESET (BUTTON_START | BUTTON_STOP)
#define BUTTON_MASK (BUTTON_START | BUTTON_STOP)

//Globals
unsigned long timer;

int running = 0;
int buttons = 0;

unsigned long updateTime = 0;
unsigned long startTime = 0;
unsigned long endTime = 0;

void delay_ms(unsigned int ms)
{
  unsigned int i;
  for (i = 0; i<= ms; i++)
  {
    __delay_cycles(500);

  }
}

void main(void)
{
  // Stop watchdog timer
  WDTCTL = WDTPW + WDTHOLD;

  // CCR0 interrupt enabled
  CCTL0 = CCIE;

  // SMCLK/8, upmode
  TACTL = TASSEL_2 + MC_1 + ID_3;

  // 125 Hz
  CCR0 =  10000;

  // All P1 pins output. hmm..
  P1DIR |= BIT0 | BIT5;
  // Turn off all outputs, including LED
  P1OUT &= 0x00;

  // Enter low power mode, but keep interrupts on
  _BIS_SR(GIE);
  
  //TODO start communication with serial device
  //Write 'U' to serical port

  while(1)
  {
    buttons = P1IN & BUTTON_MASK;
    
    if( buttons == BUTTON_START && running == 0 )
    {
      startTime = timer;
      running = 1;
    }
    else if( buttons == BUTTON_STOP && running == 1 )
    {
      endTime = timer - startTime;
      running = 2;
    }
    else if( buttons == BUTTON_RESET && running == 2 ) 
    {
      running = 3;
    }
    else if( buttons == 0 && running == 3 )
    {
      running = 0;
    }
    
    if( (timer - updateTime) > UPDATE_DELAY )
    {
    	updateTime = timer;
      char command[] = {0x53,0x00,0x00,0x00,0x00,0x00,0x02,0xFF,0x00,0x00,0x30,0x30,0x3A,0x30,0x30,0x3A,0x30,0x30,0x2E,0x30,0x30,0x00};
      //TODO make command string
      if( running == 1 )
      {
        //TODO update command to show timer - startTime
      }
      else if( running == 2 )
      {
        //TODO update command to show endTime
      }
      //TODO disable interrupts
      
      //TODO update display
      
      //TODO enable interrupts
    }
  }
}

// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{   
  // increment timer
  ++timer;
}
