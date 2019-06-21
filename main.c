/*
 * autko_new.c
 *
 * Created: 2019-02-06 13:48:03
 * Author : xx
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "USART.h"
#define input_A1 PINC0
#define input_A2 PINC1
#define input_A3 PINC2
#define input_A4 PINC3
#define S1 (1<<PINC4)
#define S2 (1<<PINC5)

volatile uint8_t manual_mode;
volatile uint8_t button;

void drive_straight(void)
{
	PORTC &= ~((1 << input_A2) | (1 << input_A3));
	PORTC |= ((1 << input_A1) | (1 << input_A4));
}

void turn_right(void)
{
	PORTC &= ~((1 << input_A1) | (1 << input_A3));
	PORTC |= ((1 << input_A2) | (1 << input_A4));
}

void turn_left(void)
{
	PORTC &= ~((1 << input_A2) | (1 << input_A4));
	PORTC |= ((1 << input_A1) | (1 << input_A3));
}

void stop(void)
{
	PORTC &= ~((1 << input_A1) | (1 << input_A2) | (1 << input_A3) | (1 << input_A4));
}

void drive_back(void)
{
	PORTC &= ~((1 << input_A1) | (1 << input_A4));
	PORTC |= ((1 << input_A2) | (1 << input_A3));
}

uint16_t distance_f(uint8_t S)
{
	PORTC |= S;
	PORTB |= (1<<PB0); //impuls
	_delay_us(10);
	PORTB &= ~(1<<PB0);
	while( !(PINB & (1<<PB1)));//pinb do czytania stanu // czekamy az ustawi sie na jeden (schodek)
	TCNT1=0x00;
	while( PINB & (1<<PB1));// czekamy a¿ bedzie zero (schodek)
    uint16_t distance = TCNT1/56;
	PORTC &= ~(S);
	
	printString("Odleglosc: ");
	printWord(distance);
	printString("\n\r");
	return distance;
}

ISR(USART_RX_vect)
{
	manual_mode = 1;
	button = receiveByte();
	printString("Bajt: ");
	transmitByte(button);
}

int main(void)
{
	initUSART();
	UCSR0B |= ( 1 << RXCIE0);
	sei();
	
    DDRB = 0x01;
    DDRC = 0x3F;
    TCNT1 = 0x00;          //aktualny stan licznika przechowywany jest w rejestrze TCNT1;
    TCCR1B = (1<<CS10); // clk/1
    drive_straight();
	while (1) 
    {
		if (manual_mode == 0)
		{
			// pierwszy przypadek, z prawej
			printString("1. Sprawdzam z prawej\n\r");
			if ( distance_f(S1) < 20)
			{
				do
				{
					printString("2. Skrecam w lewo\n\r");
					turn_left();
				}
				while (distance_f(S1) < 30);
			}
			drive_straight();
			
			// drugi przypadek, z lewej
			printString("3. Sprawdzam z lewej\n\r");
			if ( distance_f(S2) < 20)
			{
				do
				{
					printString("4. Skrecam w prawo\n\r");
					turn_right();
				}
				while (distance_f(S2) < 30);
			}
			drive_straight();
			
			// trzeci przypadek, prosto
			printString("5. Sprawdzam na wprost\n\r");
			if ( distance_f((S1|S2)) < 20)
			{
				uint16_t distance_S1 = distance_f(S1);
				uint16_t distance_S2 = distance_f(S2);
				if(distance_S1 > distance_S2)
				{
					do
					{
						printString("6. Skrecam w prawo\n\r");
						turn_right();
					}
					while (distance_f((S1|S2)) < 30);
				}
				else
				{
					do
					{
						printString("7. Skrecam w lewo\n\r");
						turn_left();
					}
					while (distance_f((S1|S2)) < 30);
				}
			}
			drive_straight();
		}
		else
		{
			if (button == 'A')
			{
				drive_straight();
			}
			else if (button == 'B')
			{
				turn_left();
			}
			else if (button == 'C')
			{
				turn_right();
			}
			else if (button == 'D')
			{
				drive_back();
			}
			else if (button == 'E')
			{
				stop();
			}
			else if (button == 'F')
			{
				manual_mode = 0;
			}
		}
    }
	return 0;
}

