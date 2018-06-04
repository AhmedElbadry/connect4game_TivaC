// TExaS.h
// Runs on LM4F120/TM4C123
// Periodic timer Timer5A which will interact with debugger and grade the lab 
// It initializes on reset and runs whenever interrupts are enabled
// Jonathan Valvano. Daniel Valvano
// April 9, 2014

/* This example accompanies the book
   "Embedded Systems: Real Time Operating Systems for ARM Cortex M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2013
   Section 6.4.5, Program 6.1

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// You have three options for display drivers/hardware:
// 1) Emulate the Nokia5110 LCD using UART0 and PA1-0 to send data to a PC
//    running TExaSdisplay in Nokia mode. (Note: this mode is not finished yet)
// 2) Interface a real Nokia5110 LCD and use SSI0 and PA7-2 to send data and
//    commands to it.  Allow TExaS to use UART0 and a PC running TExaSdisplay
//    to implement a low-cost oscilloscope.
// 3) Interface a real Nokia5110 LCD and use SSI0 and PA7-2 to send data and
//    commands to it.  Do not enable the oscilloscope.
// 4) There is no LCD. Do not enable the oscilloscope. You may use this mode to
//    have no display or to use the UART ASCII output like Labs 5 and 11
enum DisplayType{
  UART0_Emulate_Nokia5110_NoScope,
  SSI0_Real_Nokia5110_Scope,
  SSI0_Real_Nokia5110_NoScope,
  NoLCD_NoScope
};

// ************TExaS_Init*****************
// Initialize grader, voltmeter on timer 5A, scope on timer4A
// sets PLL to 80 MHz
// This needs to be called once 
// Inputs: display system used to output the results
// Outputs: none
void TExaS_Init(enum DisplayType display);

// ************TExaS_Stop*****************
// Stop the transfer 
// Inputs:  none
// Outputs: none
void TExaS_Stop(void);




//------------UART0_Init------------
// Initialize the UART for 115,200 baud rate (assuming 80 MHz UART clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void UART0_Init(void);

//------------UART0_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
unsigned char UART0_InChar(void);
//------------UART0_InCharNonBlocking------------
// look for new serial port input
// Input: none
// Output: ASCII code for key typed
//         0 if no key ready
unsigned char UART0_InCharNonBlocking(void);
//------------UART0_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART0_OutChar(unsigned char data);
//------------UART0_OutCharNonBlock------------
// Output 8-bit to serial port, do not wait
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART0_OutCharNonBlock(unsigned char data);



//------------UART_InUDec------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 32-bit unsigned number
//     valid range is 0 to 4294967295 (2^32-1)
// Input: none
// Output: 32-bit unsigned number
// If you enter a number above 4294967295, it will return an incorrect value
// Backspace will remove last digit typed
unsigned long UART_InUDec(void);

//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART_OutString(unsigned char buffer[]);

//-----------------------UART_ConvertUDec-----------------------
// Converts a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: store the conversion in global variable String[10]
// Fixed format 4 digits, one space after, null termination
// Examples
//    4 to "   4 "  
//   31 to "  31 " 
//  102 to " 102 " 
// 2210 to "2210 "
//10000 to "**** "  any value larger than 9999 converted to "**** "
void UART_ConvertUDec(unsigned long n);

//-----------------------UART_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Fixed format 4 digits, one space after, null termination
void UART_OutUDec(unsigned long n);

