/*
 * GccApplication1.c
 *
 * Created: 5/21/2019 3:08:12 PM
 * Author : Catalin
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "src/usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char rx_buffer[3];
int rx_count = 0;

uint8_t mic_buf[256];
int mic_pos = 0;
uint8_t	buf[2][256];	// wave output buffers (double buffering)
const	 uint16_t	buf_size = 256;	// front and back buffer sizes
volatile uint8_t	buf_front = 0;	// front buffer index (current buffer used)
volatile uint8_t	buf_pos = 0;	// current buffer position
volatile uint8_t	buf_sync = 0;

#define BUF_FRONT	(buf[buf_front])
#define BUF_BACK	(buf[1 - buf_front])

void check_voice_command()
{
	switch (rx_buffer[0]) {
	case 'l':
		// Lumina alba on
		PORTB |= (1 << PB0);
		break;
	case 'o':
		// Lumina alba off
		PORTB &= ~(1 << PB0);
		break;
	case 'p':
		// Lumina portocalie on
		PORTB |= (1 << PB1);
		break;
	case 'k':
		// Lumina portocalie off
		PORTB &= ~(1 << PB1);
		break;
	case 'a':
		// Toate luminile aprinse
		PORTB |= (1 << PB0) | (1 << PB1);
		PORTD |= (1 << PD7);
		break;
	case 'f':
		// Toate luminile stinse
		PORTB &= ~(1 << PB0);
		PORTB &= ~(1 << PB1);
		PORTD &= ~(1 << PD7);
		break;
	}
}

ISR(USART0_RX_vect)
{
	char voice_command;
	voice_command = UDR0;
	rx_buffer[rx_count++] = voice_command;
	
	if (voice_command == '\n') {
		check_voice_command();
		rx_count = 0;			
	}
}


void adc_init(void)
{
	ADMUX = 0;
	ADMUX |= (1 << MUX0);
	ADMUX |= (1 << REFS0);									// AVCC with external capacitor at AREF pin
	ADCSRA = 0;
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);   //set clock prescaler at 128
	ADCSRA |= (1 << ADEN);									// enable ADC
	
}

uint16_t adc_read(uint8_t channel)
{
	ADMUX &= 0xF0;                    	//Clear the older channel that was read
	ADMUX |= channel;                	//Defines the new ADC channel to be read
	ADCSRA |= (1<<ADSC);               //Starts a new conversion
	while(ADCSRA & (1<<ADSC));         //Wait until the conversion is done
	return ADC;                   	//Returns the ADC value of the chosen channel
}

void timer0_start(void)
{
	// interrupt on compare A
	TIMSK0 |= (1 << OCIE0A);
	// CTC, top OCRA
	TCCR0B |= (0 << WGM02);
	TCCR0A |= (1 << WGM01) | (0 << WGM00);
	// prescaler 8
	TCCR0B |= (2 << CS00);
}

void timer0_stop(void)
{
	TCCR0B = 0;
	TCCR0A = 0;
	TIMSK0 = 0;
	OCR0A = 0;
	TCNT0 = 0;
}

void timer1_start(void)
{
	// 8-bit FastPWM
	TCCR1B |= (1 << WGM12);
	TCCR1A |= (1 << WGM10);
	// channel B inverted
	TCCR1A |= (1 << COM1B0) | (1 << COM1B1);
	// prescaler 1
	TCCR1B |= (1 << CS10);
}

void timer1_stop(void)
{
	TCCR1B = 0;
	TCCR1A = 0;
	OCR1B = 0;
	TCNT1 = 0;
}

void calibrate(void)
{
	int i;	
	adc_init();
	
	while (1) {
		uint16_t peakToPeak;
		uint16_t signalMax = 0;
		uint16_t signalMin = 1024;
		uint16_t signal;
		char voltsString[10];

		for (i = 0; i < 50; i++){
			signal = adc_read(0);
			
			if (signal < 1024)  // toss out spurious readings
			{
				if (signal > signalMax)
				{
					signalMax = signal;  // save just the max levels
				}
				else if (signal < signalMin)
				{
					signalMin = signal;  // save just the min levels
				}
			}
			
			// This is equivalent to 10KHz sample rate
			_delay_ms(1);
		}
		
		peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
		double volts = (peakToPeak * 5.0) / 1024;  // convert to volts

		volts /= 2.5;

		uint8_t wave = volts * 255;

		if (wave > 160)
			PORTD ^= (1 << PD7);


		memset(voltsString, 0, 10);
		sprintf(voltsString, "%d\n", wave);

		USART0_print(voltsString);
	}
}


int main(void)
{
	DDRD |= (1 << PD7);
	DDRB |= (1 << PB0);
	DDRB |= (1 << PB1);
	DDRB |= (1 << PB6);
	DDRB |= (1 << PB7);

	ADCSRA |= (1 << ADIE);
	sei();
	
	USART0_init();

	calibrate();

}
