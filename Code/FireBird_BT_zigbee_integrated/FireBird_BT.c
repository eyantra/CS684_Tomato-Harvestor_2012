/**@mainpage package FireBird-5 Control Through Bluetooth
 @author Group 7: 	Amandeep 123050017 
 					Lorin 	 123050029
					Jayasree 123050050
					Ashish	 123050023

 AVR Studio Version 4.17, Build 666

 Date: 18th November 2012
 
 This experiment demonstrates FireBird 5 control through Bluetooth Interface


 Bluetooth Connections:
 		Bluetooth	  POD Extension Connector Pin
 			  Rx  --> 1  (Txd)
			  Tx  --> 2  (Rxd)
			  VCC --> 21 (+5v)
			  GND --> 23 (GND)

 Note: 
 
 1. Make sure that in the Bluetooth Module is configured to work at baud rate of 9600
 	bps, No Parity and No Flow Control.

 2.	Make sure that in the configuration options following settings are 
 	done for proper operation of the code

  	Microcontroller: atmega2560
 	Frequency: 11059200
 	Optimization: -O0 (For more information read section: Selecting proper optimization options 
						below figure 4.22 in the hardware manual)

 3. Jumper for USART2 is connected in POD direction and not in USB.

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

#define F_CPU 11059200ul

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "BlueTooth.h"
#include "MotorControl.h"
#include "GlobalVar.h"
#include "LCD.h"

/*Function to Initialize the Peripheral Devices*/
void Init_Devices(void)
{
	Init_USART2();
	Init_Motor_Peri();
	LCD_Init();
	adc_init();
	adc_pin_config();
	interrupt_right_init();
	right_position_encoder_init();
}


////////////////////////////////////////////////////////////

//Function To Initialize UART0
// desired baud rate:9600
// actual baud rate:9600 (error 0.0%)
// char size: 8 bit
// parity: Disabled
void uart0_init(void)
{
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;
 UBRR0L = 0x5F; //set baud rate lo
 UBRR0H = 0x00; //set baud rate hi
 UCSR0B = 0x98;
}


char fcall[5][5];
int i =0, j = 0;
int botId;

unsigned char data; 
// Pass the message to be passed as a string
void send_status(char msg[])
{
	int i;
	UDR0 = 0x28;
	for(i = 0; msg[i]!=0; i++) {
		while ( !( UCSR0A & (1<<UDRE0)) );
		UDR0 = msg[i];
	}
	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0 = 0x29;
}

void function_caller()
{
	int val, par, trough, speed;
	val = atoi(fcall[1]);
	trough = atoi(fcall[2]);
	speed =  atoi(fcall[3]);
	switch(val) {
		
		case 1 : harvest(trough, speed); break;
				
		default: UDR0 = 0x26;
	}
}

//FORMAT "botId$funCode$par1$par2$par3#"
SIGNAL(SIG_USART0_RECV)
{
	cli();
	data = UDR0;

	if(data == 0x23) // #
	{
		if(atoi(fcall[0]) == botId) {
			if(j != 0) {
				fcall[i][j] = 0;
				sei();
				function_caller();
				cli();
			}
			UDR0 = data;
		}
		i = 0;
		j = 0;
	}
	else if(data == 0x24) // $
	{
		fcall[i][j] = 0;
		i++;
		j = 0;
	}
	else
	{	
		fcall[i][j] = data;
		j++;
	}
	sei();
}

//Function To Initialize all The Devices
void init_devices()
{
 cli(); 			//Clears the global interrupts

 uart0_init();		//Initailize UART1 for serial communiaction
 sei(); 			//Enables the global interrupts
}




////////////////////////////////////////////////////////////


/*Main Function*/
int main(void)
{
	botId = 9;
	init_devices();
	while(1);
}

/* The bot initially moves to a specific trough harvest tomato if any 
 and then returns back to the initial position*/
void harvest( int trough, int speed)
{
	int trno = trough;
	int	tmp=((int)speed -(int)'0')*32;
	mode = 1;     // bot directly onto line follower, reaching the desired trough
	cli();
	Init_Devices();
	sei();
	LCD_WR_Command(0x01);
	LCD_Cursor(1,1);
	LCD_String("ANDROID_FB5");
	velocity(255,255);
	while (1)
	{
		_delay_ms(100);
		if (decode == 1)    
		{
			decode = 0;
			switch (mode)
			{
				case 0:
				switch (main_buf[0])
				{
				/* Android processes the Image and commands the bot to act accordingly */
					case 'F' :
						velocity(tmp,tmp);
						Forward();
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("BOT FORWARD");
						break;
					case 'B' :
						velocity(tmp,tmp);
						Reverse();
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("BOT REVERSE");
						break;
					case 'L' :
						tmp=((int)main_buf[1]-(int)'0')*32;
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("Speed changed");
						break;
					case 'Y' :
						Left_Turn();
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("BOT LEFT TURN");
						break;
					case 'R' :
						velocity(tmp,tmp);
						Right_Turn();
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("BOT RIGHT TURN");
						break;
					case 'S' :
						Stop();
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("BOT STOP");
						break;
					case 'U' :
						motor_c2_forward();
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("PULLEY UP");
						break;
					case 'K' :
						motor_c2_backward();
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("PULLEY DOWN");
						break;
					case 'G' :
						motor_c2_stop();
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("PULLEY STOP");
						break;
					case 'Z' :
						motor_c1_forward();
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("CUTTER ON");
						break;
					case 'E' :
						motor_c1_stop();
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("CUTTER OFF");
						break;
					case 'X' :
						mode=2;
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("Return");
						break;
					case 'T' :
						trno=(int)main_buf[1]-(int)'0';
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("Trough no. received");
						break;
					case '1' :
						mode=1;
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("Mode : 1");
						break;
					default :
						Stop();
						LCD_WR_Command(0x01);
						LCD_Cursor(1,1);
						LCD_String("ANDROID_FB5");
						break;
				}
				break;
				case 1:
				/* Bot stops listening to the commands of android
				until it reaches its specific trough */
					moveToTrough(trno);
					mode=0;
					LCD_WR_Command(0x01);
					LCD_Cursor(1,1);
					LCD_String("Leaving Mode : 1");
					_delay_ms(1000);
				break;
				case 2:
				/* Bot stops listening to the commands of android
				until it reaches its starting position */
					LCD_WR_Command(0x01);
					LCD_Cursor(1,1);
					LCD_String("Return to Start");
					_delay_ms(1000);
					returnToInitial(trno);
					mode=0;
				break;
				default :
					LCD_WR_Command(0x01);
					LCD_Cursor(1,1);
					LCD_String("Default");
					break;
			}
		}
	}

}
