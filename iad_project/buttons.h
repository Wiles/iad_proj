/*
 * File: serial.c
 * Author: Hekar Khani, Thomas Kempton, Samuel Lewis
 * Description: Provides an abstracted interface for dealing
 * with two hardware debounced buttons on pin 1.3 and 1.6
 * button statuses are read by an interrupt when any buttons is depressed
 */

#include "msp430g2231.h"

#ifndef BUTTONS_H_
#define BUTTONS_H_

#define BUTTON_START BIT3
#define BUTTON_STOP BIT6
#define BUTTON_RESET (BUTTON_START | BUTTON_STOP)
#define BUTTON_MASK (BUTTON_START | BUTTON_STOP)
#define BUTTON_STATUSES 3

//constants
extern const char buttonOrder[];

//Button globals
extern char orderOffset;
extern char buttons;

//prototypes
void initButtons(void);
void disableButtons(void);				
void enableButtons(void);
int getButtons(void);
void setButtons(char state);

#endif /*BUTTONS_H_*/
