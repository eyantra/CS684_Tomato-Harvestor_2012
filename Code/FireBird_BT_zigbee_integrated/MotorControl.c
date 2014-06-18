/**@mainpage package Motor Control
 @author Group 7: 	Jagbandhu 09307603 
 					Shah Rohan 09307050
 
 AVR Studio Version 4.17, Build 666

 Date: 13th January 2010
 
*********************************************************************************/

/********************************************************************************

   Copyright (c) 2010, ERTS Lab IIT Bombay erts@cse.iitb.ac.in               -*- c -*-
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

   * Source code can be used for academic purpose. 
	 For commercial use permission form the author needs to be taken.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. 

  Software released under Creative Commence cc by-nc-sa licence.
  For legal information refer to: 
  http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode

********************************************************************************/

#ifndef F_CPU
#define F_CPU 11059200UL
#endif
#include <avr/io.h>
#include <util/delay.h>

#include "MotorControl.h"

/*Function to initialize Peripherals required for Motor Speed Control*/
void Init_Motor_Peri()
{
	motor_init();
}

void motor_init(void)		// Motor Initializations
{
	DDRA=0xFF;
	PORTA=0x00;

	DDRL=0x38;
	PORTL=0x38;

	DDRE=0x08;
	PORTE=0x08;
	Timer5_init();
}

void Timer5_init()			// Timer Initializations
{
	TCCR5B=0x00;
	TCNT5H=0xFF;
	TCNT5H=0x00;
	OCR5AH=0x00;
	OCR5BH=0x00;
	OCR5BL=0xFF;
	OCR5CH=0x00;
	OCR5CL=0xFF;
	TCCR5A=0xA9;
	TCCR5B=0x0B;
}

void velocity(unsigned char left_motor, unsigned char right_motor)
{
	OCR5AL=(unsigned char)left_motor;
	OCR5BL=(unsigned char)right_motor;
	OCR3AL=(unsigned char)left_motor;
}

void Forward(void)
{
	PORTA=(PORTA & 0xF0 )| 0x06;
}

void Reverse(void)
{
	PORTA=(PORTA & 0xF0) | 0x09;
}

void Left_Turn(void)
{
	PORTA=(PORTA & 0xF0) | 0x05;
}

void Right_Turn(void)
{
	PORTA=(PORTA & 0xF0) | 0x0A;
}

void Stop(void)
{
	PORTA=PORTA & 0xF0;
}

//c1 is the port for cutter (pins 4,5)
//c2 is the port for pulley (pins 6,7)
void motor_c1_forward(void)
{
	PORTA=(PORTA & 0xCF) | 0x20;
}

void motor_c1_backward(void)
{
	PORTA=(PORTA & 0xCF) | 0x10;
}

void motor_c2_forward(void)
{
	PORTA=(PORTA & 0x3F) | 0x80;
}

void motor_c2_backward(void)
{
	PORTA=(PORTA & 0x3F) | 0x40;
}

void motor_c1_stop(void)
{
	PORTA=PORTA & 0xCF;
}

void motor_c2_stop(void)
{
	PORTA=PORTA & 0x3F;
}
//Voltage redings 13.5V and Maximum current.
