/*
 * File: serial.c
 * Author: Hekar Khani, Thomas Kempton, Samuel Lewis
 * Description: defentitions for function used in serial communication
 */

#include "serial.h"

//------------------------------------------------------------------------------
// Global variables used for full-duplex UART communication
//------------------------------------------------------------------------------
unsigned int txData;                        // UART internal variable for TX
unsigned char rxBuffer;                     // Received UART character

/*
 * Sets up the PIN 1.1 and 1.2 for serial communication and
 * sets up the timer for bit banging serial
 */
void initSerial(void)
{	
	//RX input and TX output
	P1DIR &= ~UART_RXD;
	P1DIR |= UART_TXD;
    // Timer function for TXD/RXD pins   
    P1SEL = UART_TXD + UART_RXD;
}

//------------------------------------------------------------------------------
// Function configures Timer_A for full-duplex UART operation
// Provided by TI
//------------------------------------------------------------------------------
void TimerA_UART_init(void)
{
    TACCTL0 = OUT;                          // Set TXD Idle as Mark = '1'
    TACCTL1 = SCS + CM1 + CAP + CCIE;       // Sync, Neg Edge, Capture, Int
    TACTL = TASSEL_2 + MC_2;                // SMCLK, start in continuous mode
}

//------------------------------------------------------------------------------
// Outputs one byte using the Timer_A UART
//
// byte - character to write to serial
//
// Provided by TI
//------------------------------------------------------------------------------
void TimerA_UART_tx(unsigned char byte)
{
    while (TACCTL0 & CCIE);                 // Ensure last char got TX'd
    TACCR0 = TAR;                           // Current state of TA counter
    TACCR0 += UART_TBIT;                    // One bit time till first bit
    TACCTL0 = OUTMOD0 + CCIE;               // Set TXD on EQU0, Int
    txData = byte;                          // Load global variable
    txData |= 0x100;                        // Add mark stop bit to TXData
    txData <<= 1;                           // Add space start bit
}

//------------------------------------------------------------------------------
// Prints a char array over serial using the Timer_A UART. Used if you
// need to transmit a message that includes 0s.
//
// array - char array to print
// count - size of array in bytes
//------------------------------------------------------------------------------
void TimerA_UART_print_array(char *array, int count)
{
	int i = 0;
	for( i = 0; i < count; ++i )
	{
        TimerA_UART_tx(array[i]);
	}
}

//------------------------------------------------------------------------------
// Prints a string over using the Timer_A UART
//
// string - null terminated string to write to serial
//
// Provided by TI
//------------------------------------------------------------------------------
void TimerA_UART_print(char *string)
{
    while (*string) {
        TimerA_UART_tx(*string++);
    }
}
//------------------------------------------------------------------------------
// Timer_A UART - Transmit Interrupt Handler
// Provided by TI
//------------------------------------------------------------------------------
#pragma vector = TIMERA0_VECTOR
__interrupt void Timer_A0_ISR(void)
{
    static unsigned char txBitCnt = 10;

    TACCR0 += UART_TBIT;                    // Add Offset to CCRx
    if (txBitCnt == 0) {                    // All bits TXed?
        TACCTL0 &= ~CCIE;                   // All bits TXed, disable interrupt
        txBitCnt = 10;                      // Re-load bit counter
    }
    else {
        if (txData & 0x01) {
          TACCTL0 &= ~OUTMOD2;              // TX Mark '1'
        }
        else {
          TACCTL0 |= OUTMOD2;               // TX Space '0'
        }
        txData >>= 1;
        txBitCnt--;
    }
}      
//------------------------------------------------------------------------------
// Timer_A UART - Receive Interrupt Handler
// Provided by TI
//------------------------------------------------------------------------------
#pragma vector = TIMERA1_VECTOR
__interrupt void Timer_A1_ISR(void)
{
    static unsigned char rxBitCnt = 8;
    static unsigned char rxData = 0;

    switch (__even_in_range(TAIV, TAIV_TAIFG)) { // Use calculated branching
        case TAIV_TACCR1:                        // TACCR1 CCIFG - UART RX
            TACCR1 += UART_TBIT;                 // Add Offset to CCRx
            if (TACCTL1 & CAP) {                 // Capture mode = start bit edge
                TACCTL1 &= ~CAP;                 // Switch capture to compare mode
                TACCR1 += UART_TBIT_DIV_2;       // Point CCRx to middle of D0
            }
            else {
                rxData >>= 1;
                if (TACCTL1 & SCCI) {            // Get bit waiting in receive latch
                    rxData |= 0x80;
                }
                rxBitCnt--;
                if (rxBitCnt == 0) {             // All bits RXed?
                    rxBuffer = rxData;           // Store in global variable
                    rxBitCnt = 8;                // Re-load bit counter
                    TACCTL1 |= CAP;              // Switch compare to capture mode
                    __bic_SR_register_on_exit(LPM0_bits);  // Clear LPM0 bits from 0(SR)
                }
            }
            break;
    }
}
