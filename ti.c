/**
* Pins 1.3 and 1.6 will be used for switches.
*/

#include "msp430g2231.h"

#define ONE_SECOND_TENTH 3276
#define UPDATE_DELAY 100

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

  // Enable P1.3 and P1.6 interrupt
  // Don’t waste time interrupting on Pin 1.4
  P1IE |= BIT3 | BIT6;
  // Clear P1.3 and P1.6 IFG
  P1IFG &= ~BIT3 | ~BIT6;

  // Enter low power mode, but keep interrupts on
  _BIS_SR(CPUOFF + GIE);

  while(1)
  {

    // Update the VGA display

    delay_ms(UPDATE_DELAY);
  }
}

// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{   
  // increment timer
}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
  if (pin & 1_3 && pin & 1_6)
  {
    // reset timer

  }

  if (pin == 1_3)

  {

    P1DIR |= BIT0;

    P1OUT &= BIT0;

    // start the timer

  }

  else if (pin == 1_6)

  {

    P1DIR |= BIT0;

    P1OUT &= 0x00;

    // stop timer
  }
}