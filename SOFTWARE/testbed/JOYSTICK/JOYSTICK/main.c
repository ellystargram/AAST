/*
 * JOYSTICK.c
 *
 * Created: 6/14/2023 6:28:42 PM
 * Author : 6gioc
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <util/delay.h>

#define X 0
#define Y 1

void setup_(void);
void update_(void);

unsigned char _ADCPIN_selector=0;
unsigned int _x=0;
unsigned int _y=0;
unsigned char _x_default=0-(256/2);
unsigned char _y_default=0-(256/2);
unsigned char _timer0_timer=0;
unsigned char _timer2_timer=0;
unsigned char _porta_output=0;
unsigned char _motor_drive[]={0x01,0x03,0x02,0x06,0x04,0x0c,0x08,0x09};
unsigned char _timer0_motor_count=0;
unsigned char _timer2_motor_count=0;
unsigned char _fire=1;
unsigned char _triggered=1;

int main(void)
{
	setup_();
    /* Replace with your application code */
    while (1) 
    {
		update_();
    }
}

void setup_(void){
	DDRA=0xff;
	DDRB=0xff;
	DDRC=0xff;
	DDRF=0x00;
	DDRG=0b111110;
	PORTG=0xfe;
	
	TCCR0=7;
	TCNT0=_x_default;
	TCCR2=3;
	TCNT2=0;
	TIMSK=(1<<TOIE0)|(1<<TOIE2);
	
	sei();
}
void update_(void){
	
	_ADCPIN_selector=!_ADCPIN_selector;
	
	if(_ADCPIN_selector==X){
		ADMUX=0x00;
	}
	else{
		ADMUX=0x01;
	}
	ADCSRA=0b11001111;
	while(ADCSRA&0b00010000==0);
	_delay_ms(1);
	
	if(PING&1==1){
		_triggered=1;
	}
	else{
		_triggered=0;
	}

	if(_triggered==1){
		TCCR2=3;
	}
	else{
		_fire=1;
		TCCR2=0;
	}
	
	
	
	
	
}

ISR(ADC_vect){
	switch(_ADCPIN_selector){
		case X:
		_x = ADCW;
		if(_x<= 522&& _x>=502){
			PORTB=~((_fire<<4)|(_fire<<3));
		}
		else{
			PORTB=~(_fire<<(_x/128));
		}
		_x_default=abs(_x/4-128);
		break;
		case Y:
		_y=ADCW;
		if(_y<=522&&_y>=502){
			PORTC=((_fire<<4)|(_fire<<3));
		}
		else{
			PORTC=(_fire<<(_y/128));
		}
		_y_default=abs(_y/4-128);
		break;
	}
}

ISR(TIMER0_OVF_vect){
	TCNT0=_x_default;
	_timer0_timer++;
	if(_timer0_timer==20){
		_timer0_timer=0;
		_porta_output=_porta_output&0xf0;
		if(_x<(1023/2)){
			_porta_output=_porta_output|_motor_drive[_timer0_motor_count];
			if(_timer0_motor_count==0){
				_timer0_motor_count=7;
			}
			else{
				_timer0_motor_count--;
			}
		}
		else if(_x>(1023/2)){
			_porta_output=_porta_output|_motor_drive[_timer0_motor_count];
			_timer0_motor_count=(_timer0_motor_count+1)%8;
		}
		else{
			_porta_output=_porta_output|_motor_drive[_timer0_motor_count];
		}
		PORTA=_porta_output;
	}
}
ISR(TIMER2_OVF_vect){
	TCNT2=0;
	_timer2_timer++;
	if(_timer2_timer==100){
		_timer2_timer=0;
		_fire=!_fire;
	}
}