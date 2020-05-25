/*	Author: ryan
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char threeLEDs;
unsigned char blinkingLED;
unsigned char speaker;
unsigned char freq = 5;

enum ThreeStates{start, zero, one, two} Three_State;
enum OneState{init, off, on} One_State;
enum SpeakerState{isa, loud, quiet, wait} Speaker_State;
enum AdjustState{uno, wait1, up, down, wait2} Adjust_State;
enum CombinedStates{begin, transmit} Combined_State;

void TickAdjust(){
	unsigned char btn0;
	unsigned char btn1;
	btn0 = ~PINA & 0x01;
	btn1 = ~PINA & 0x02;
	switch(Adjust_State){
		case uno:
			Adjust_State = wait1;
			break;
		case wait1:
			if(btn0 && btn1){
				Adjust_State = wait1;
			}
			else if(btn0){
				Adjust_State = up;
			}
			else if(btn1){
				Adjust_State = down;
			}
			break;
		case up:
			Adjust_State = wait2;
			break;
		case down:
			Adjust_State = wait2;
			break;
		case wait2:
			Adjust_State = !(btn0&&btn1) ? wait1 : wait2;
			break;
	}
	switch(Adjust_State){
                case uno:
                        break;
                case wait1:
                        break;
                case up:
			freq++;
                        break;
                case down:
			freq--;
                        break;
                case wait2:
                        break;
        }
}
unsigned char i = 0;
void TickSpeaker(){
	unsigned char btn;
	btn = ~PINA & 0x04;

//	unsigned char i = 0;	
	unsigned char high = freq;
	unsigned char low = 10 - freq;
	switch(Speaker_State){
		case isa:
			Speaker_State = wait;
			speaker = 0;
			break;
		case wait:
			Speaker_State = btn ? loud : wait;
			break;
		case quiet:
			if(!btn){
				Speaker_State = wait;
			}
			else{
				if(i < low){
					Speaker_State = quiet;
					i++;
				}
				else if(i >= low){
					Speaker_State = loud;
					i = 0;
				}
			}
			break;
		case loud:
			if(!btn){
                                Speaker_State = wait;
                        }
                        else{
				if(i < high){
					i++;
				}
				else if(i >= high){
					i = 0;
					Speaker_State = quiet;
				}
                        }
                        break;
	}
	switch(Speaker_State){
		case isa:
			break;
		case wait:
			speaker = 0;
		case quiet:
			speaker = 0;
			break;
		case loud:
			speaker = 0x10;
			break;
	}
}

void TickThree(){
	switch(Three_State){
		case start:
			threeLEDs = 0x00;
			Three_State = zero;
			break;
		case zero:
			Three_State = one;
			break;
		case one:
			Three_State = two;
			break;
		case two:
			Three_State = zero;
			break;
	}
	switch(Three_State){
		case start:
			break;
                case zero:
			threeLEDs = 0x01;
                        break;
                case one:
			threeLEDs = 0x02;
                        break;
                case two:
			threeLEDs = 0x04;
                        break;
        }
}

void TickOne(){
	switch(One_State){
		case init:
			blinkingLED = 0x00;
			One_State = off;
			break;
		case off:
			One_State = on;
			break;
		case on:
			One_State = off;
			break;
	}
	switch(One_State){
                case init:
                        break;
                case off:
			blinkingLED = 0x00;
                        break;
                case on:
			blinkingLED = 0x08;
                        break;
        }
}

void TickTogether(){
	unsigned char together;
	together = 0x00;
	switch(Combined_State){
		case begin:
			Combined_State = transmit;
			break;
		case transmit:
			break;
	}
	switch(Combined_State){
                case begin:
                        break;
                case transmit:
			together = speaker | blinkingLED | threeLEDs;
                        break;
        }
	PORTB = together;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00;	PORTA = 0xFF;
	DDRB = 0xFF;	PORTB = 0x00;
    /* Insert your solution below */
	unsigned char adjustTimer = 0x00;
	unsigned short threeTimer = 0x0000;
	unsigned short oneTimer = 0x0000;
	TimerSet(1);
	TimerOn();
	Adjust_State = uno;
	Three_State = start;
	One_State = init;
	Speaker_State = isa;
	Combined_State = begin;
    while (1) {
	if(threeTimer >= 300){
		TickThree();
		threeTimer = 0;
	}
	if(oneTimer >= 1000){
		TickOne();
		oneTimer = 0;
	}
	if(adjustTimer >= 250){	
		TickAdjust();
		adjustTimer = 0;
	}
	TickSpeaker();
	TickTogether();
	while(!TimerFlag);
	TimerFlag = 0;
	threeTimer++;
	oneTimer++;
	adjustTimer++;
    }
    return 1;
}
