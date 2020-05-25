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

enum ThreeStates{start, zero, one, two} Three_State;
enum OneState{init, off, on} One_State;
enum CombinedStates{begin, transmit} Combined_State;


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
			together = blinkingLED | threeLEDs;
                        break;
        }
	PORTB = together;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF;	PORTB = 0x00;
    /* Insert your solution below */
	unsigned char threeTimer = 0x00;
	unsigned char oneTimer = 0x00;
	TimerSet(100);
	TimerOn();
	Three_State = start;
	One_State = init;
	Combined_State = begin;
    while (1) {
	if(threeTimer >= 3){
		TickThree();
		threeTimer = 0;
	}
	if(oneTimer >= 10){
		TickOne();
		oneTimer = 0;
	}
	TickTogether();
	while(!TimerFlag);
	TimerFlag = 0;
	threeTimer++;
	oneTimer++;
    }
    return 1;
}
