/*
 * ATMEGA8 AS DISPLAY ENGINE.c
 *
 * Created: 7/6/2023 9:36:52 AM
 * Author : 6gioc
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

unsigned char _display_repeat=0;
unsigned char _text[4][64]={"ERROR", "AUTO", "MANUAL", "STOP"};
unsigned char _value_in=0;
unsigned char _digit=0;
unsigned char _moveanimation=0;
unsigned int _animation_repeat=0;
unsigned char _text_4digit[5];
unsigned int _14segment[]={0x0c3f, 0x0406, 0x00db, 0x008f, 0x00e6, 0x00ed, 0x00fd, 0x1401, 0x00ff, 0x00e7, 0x00f7, 0x128f, 0x0039, 0x120f, 0x00f9, 0x00f1, 0x00bd, 0x00f6, 0x1209, 0x001e, 0x2470, 0x0038, 0x0536, 0x2136, 0x003f, 0x00f3, 0x203f, 0x20f3, 0x018d, 0x1201, 0x003e, 0x0c30, 0x2836, 0x2d00, 0x1500, 0x0c09};

void setup_(void);
void update_(void);

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
	
	DDRB=0xf0;
	DDRC=0x7f;
	DDRD=0xff;
	
	TCCR1A=0;
	TCCR1B=(1<<WGM12)|(1<<CS12);
	OCR1A=128;//adjust here
	TIMSK=1<<OCIE1A;
	
	
	sei();
}
void update_(void){
	unsigned char _value_pre=PINB&0x0f;
	if(_value_pre!=_value_in){
		_value_in=_value_pre;
		_moveanimation=0;
		for(unsigned char _mask=0; _mask<4;_mask++){
			_text_4digit[_mask]=_text[_value_in][_mask];
		}
	}
}

ISR(TIMER1_COMPA_vect){
	if(++_display_repeat==5){
		_display_repeat=0;
		PORTB=0xff;
		PORTC=0;
		PORTD=0;
		PORTB=~(1<<(4+_digit));
		if(_text_4digit[_digit]>='0'&&_text_4digit[_digit]<='9'){//if number
			PORTC=_14segment[_text_4digit[_digit]-0x30];
			PORTD=_14segment[_text_4digit[_digit]-0x30]>>7;
		}
		else if(_text_4digit[_digit]>='A'&&_text_4digit[_digit]<='Z'){//if ENGLISH
			PORTC=_14segment[_text_4digit[_digit]-0x41+10];
			PORTD=_14segment[_text_4digit[_digit]-0x41+10]>>7;
		}
		
		if(_digit<3){
			_digit++;
		}
		else{
			_digit=0;
		}
		if(++_animation_repeat==50){
			_animation_repeat=0;
			if(_moveanimation+3<=strlen((char *)_text[_value_in])){
				_moveanimation++;
			}
			else{	
				_moveanimation=0;
			}
			for(unsigned char _mask=0; _mask<4;_mask++){
				_text_4digit[_mask]=_text[_value_in][(_mask+_moveanimation)%4];
			}
		}
	}
	
}