#include "buttons.h"

//constants

//Used to prevent operation in the incorrect order
const char buttonOrder[] = {BUTTON_START,BUTTON_STOP, BUTTON_MASK};

//Button globals
char orderOffset = 0;
char buttons = 0;

/*
 * Prepares pins 1.3 and 1.6 to be used by 
 * interrupts
 * 
 */
void initButtons(void)
{
	P1DIR &= ~BUTTON_MASK;
	
	//Interrupt of falling edge
	P1IES &= ~BUTTON_MASK;
	//Clear button interrupt status
	P1IFG &= ~BUTTON_MASK;
	
  	IE1 |= WDTIE;
}

/*
 * Disables the interrupt that reads the button
 * 
 */
void disableButtons(void)
{	
	P1IE &= ~BUTTON_MASK;
}

/*
 * Enables the interrupt on the buttons
 * 
 */
void enableButtons(void)
{	
	P1IE |= BUTTON_MASK;
}

/*
 * Provides access to the buttons global
 * 
 * Returns the last read button status
 */
int getButtons(void)
{
	return buttons;
}

/*
 * Allows the status of the button to be set
 * will be overriden the next time the button
 * interrupt is fired.
 * 
 * state - the new status for the buttons
 */
void setButtons(char state)
{
	buttons = state;
}

/*
 * Updates the Button state for the programme
 * only allowing the state to change in the correct order
 */
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{	
	P1IFG &= ~BUTTON_MASK;
	int p = P1IN;
	int buttonState = p & BUTTON_MASK;
	if( buttonState == buttonOrder[orderOffset] )
	{
		orderOffset = (orderOffset + 1 ) % BUTTON_STATUSES;
		buttons = buttonState;
	}
}

