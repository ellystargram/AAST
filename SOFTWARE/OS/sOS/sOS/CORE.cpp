/*
* DEFAULT_FUNCTION.cpp
*
* Created: 7/26/2023 1:40:26 AM
*  Author: 6gioc
*/
#include "TOOLBOX.h"

#define JOYSTICK_DEAD_ZONE 32

unsigned char _dotmatrix_animation_counter=0;
unsigned int _dotmatrix_animation_move=0;
unsigned char _adc_target=_x_joystick;
unsigned int _x_read=0;
unsigned int _y_read=0;
unsigned char _adc_counter=0;
unsigned int _tempature_read=0;
unsigned int _light_read=0;

unsigned int _x_motor_ocr=0;
unsigned int _y_motor_ocr=0;
unsigned char _mind[8];
unsigned char _mode=_manual;
unsigned char _mode_list[10][50]={"Stop", "Auto", "Manual", "Use Joystick to aim target", "Press button to fire"};
unsigned char _tutorial_stage = _tutorial_ended;
unsigned char _conami_code_is_entered=FALSE;
const unsigned char _conami_code[] = {_up,_up,_down,_down,_left,_right,_left,_right};
unsigned char _conami_code_progress=0;
unsigned char _conami_code_entered_value=_nodirection;

unsigned char _fan_pwm_boolean = FALSE;

signed int _x_read_signed=0;
signed int _y_read_signed=0;


unsigned int _auto_manual_switch_timer_miliseconds = 0;

unsigned char _gun_shot_triggered=FALSE;

void startup_(void){
	DDRA=0x00;
	DDRB=0xff;
	DDRC=0xff;
	DDRD=0x00;
	DDRE=0x00;
	DDRF=0x00;
	DDRG=0xff;
	
	ADMUX=0;
	ADCSRA=(TRUE<<ADEN)|(TRUE<<ADPS0)|(TRUE<<ADPS1)|(TRUE<<ADPS2)|(TRUE<<ADIE);
	
	TCCR0=(TRUE<<WGM01)|(TRUE<<CS02)|(TRUE<<CS01)|(TRUE<<CS00);
	OCR0=0;//todo last
	TCCR1A=0;
	TCCR1B=(TRUE<<WGM12)|(TRUE<<CS10)|(TRUE<<CS12);
	TCCR1C=0;
	OCR1AH=0;//todo last
	OCR1AL=0;//todo last
	TCNT2=T2INTITIAL;
	TCCR2=(TRUE<<CS21)|(TRUE<<CS20);
	TCCR3A=0;
	TCCR3B=(TRUE<<WGM32)|(TRUE<<CS30)|(TRUE<<CS32);
	TCCR3C=0;
	OCR3AH=0;//todo last
	OCR3AL=0;//todo last
	TIMSK=(TRUE<<OCIE1A)|(TRUE<<TOIE2)|(TRUE<<OCIE0);
	ETIMSK=(TRUE<<OCIE3A);
	sei();
	display_setup_();
	stepmotor_setup_();
	r_a_comm_setup_();
}
void update_(void){
	display_update_();
	ADCSRA=ADCSRA|(TRUE<<ADSC);
	while((ADCSRA&(TRUE<<ADEN))==TRUE);
	r_a_comm_update_();
	
	if(_conami_code_progress==7){
		_conami_code_progress=0;
		_tutorial_stage = _joystick_tutorial;
	}
}

void portacontroler_(unsigned char _bit, unsigned char _logical_value){
	unsigned char _porta=(TRUE<<_bit);
	_porta=~_porta;
	_porta=_porta&PORTA;
	_porta=_porta|(_logical_value<<_bit);
	PORTA=_porta;
	return;
}
void portbcontroler_(unsigned char _bit, unsigned char _logical_value){
	unsigned char _portb=(TRUE<<_bit);
	_portb=~_portb;
	_portb=_portb&PORTB;
	_portb=_portb|(_logical_value<<_bit);
	PORTB=_portb;
	return;
}
void portccontroler_(unsigned char _bit, unsigned char _logical_value){
	unsigned char _portc=(TRUE<<_bit);
	_portc=~_portc;
	_portc=_portc&PORTC;
	_portc=_portc|(_logical_value<<_bit);
	PORTC=_portc;
	return;
}
void portgcontroler_(unsigned char _bit, unsigned char _logical_value){
	unsigned char _portg=(TRUE<<_bit);
	_portg=~_portg;
	_portg=_portg&PORTG;
	_portg=_portg|(_logical_value<<_bit);
	PORTG=_portg;
	return;
}

unsigned char getbitofbyte_(unsigned char _byte, unsigned char _bit){
	return (_byte>>_bit)&0x01;
}
void mindwizard_(unsigned int _xval, unsigned int _yval){
	for(unsigned char _erasey=0;_erasey<8;_erasey++){
		_mind[_erasey]=0;
	}
	signed int _xvalsigned=(signed int)_xval-512;
	signed int _yvalsigned=(signed int)_yval-512;
	unsigned char _xline=0;
	
	if(abs_(_xvalsigned)<JOYSTICK_DEAD_ZONE){
		_xval=512;
		_xvalsigned=0;
	}
	if(abs_(_yvalsigned)<JOYSTICK_DEAD_ZONE){
		_yval=512;
		_yvalsigned=0;
	}
	
	if(_xvalsigned == 0 && _yvalsigned == 0 && _conami_code_is_entered){
		if(_conami_code_progress>=8){
			_conami_code_progress=0;
		}
		if(_conami_code[_conami_code_progress]==_conami_code_entered_value){
			_conami_code_progress++;
		}
		else{
			_conami_code_progress=0;
		}
		_conami_code_entered_value = _nodirection;
		_conami_code_is_entered = FALSE;
	}
	
	_xval=_xval/64;
	if(_xval%2==0){
		_xline=3<<(7-(_xval/2));
	}
	else{
		_xline=1<<(7-(_xval/2));
	}
	
	_yval=_yval/64;
	if(_yval%2==0){
		_mind[_yval/2]=_xline;
		if(_yval>=1){
			_mind[_yval/2-1]=_xline;
		}
	}
	else{
		_mind[_yval/2]=_xline;
	}
	
	if(_mode ==_manual){
		stepmotor_update_(_xvalsigned, _yvalsigned);
	}
	
	
	if(_xvalsigned>500 && _conami_code_is_entered == FALSE){
		_conami_code_entered_value = _right;
		_conami_code_is_entered = TRUE;
	}
	else if(_xvalsigned<-500 && _conami_code_is_entered == FALSE){
		_conami_code_entered_value = _left;
		_conami_code_is_entered = TRUE;
	}
	else if(_yvalsigned>500 && _conami_code_is_entered == FALSE){
		_conami_code_entered_value = _down;
		_conami_code_is_entered = TRUE;
	}
	else if(_yvalsigned<-500 && _conami_code_is_entered == FALSE){
		_conami_code_entered_value = _up;
		_conami_code_is_entered = TRUE;
	}
	
	
	
	return;
}
unsigned int abs_(signed int _value){
	if(_value<0){
		return _value * -1;
	}
	else{
		return _value;
	}
}

ISR(TIMER0_COMP_vect){//fan control
	const unsigned char _fan_speed = _tempature_read>>2;
	
	if(_fan_pwm_boolean == TRUE){
		OCR0=_fan_speed;
		_fan_pwm_boolean = FALSE;
		if(_gun_shot_triggered==TRUE){
			portbcontroler_(PB5,FALSE);
			return;
		}
		portbcontroler_(PB5,TRUE);
	}
	else{
		_fan_pwm_boolean = TRUE;
		OCR0=255-_fan_speed;
		if(_fan_speed == 255){
			OCR0=1;
		}
		if(_gun_shot_triggered==TRUE){
			portbcontroler_(PB5,FALSE);
			return;
		}
		portbcontroler_(PB5,FALSE);
	}
}

ISR(TIMER2_OVF_vect){//1ms
	TCNT2=T2INTITIAL;
	
	_dotmatrix_animation_counter++;
	if(_dotmatrix_animation_counter==100){//100ms
		_dotmatrix_animation_counter=0;
		_dotmatrix_animation_move++;
		if(_dotmatrix_animation_move>=7*strlen((char*)_mode_list[_mode])){
			_dotmatrix_animation_move=0;
		}
	}
	_adc_counter++;
	if(_adc_counter==10){
		_adc_counter=0;
		
	}
	
	if(_mode == _manual && _tutorial_stage == _tutorial_ended){
		_auto_manual_switch_timer_miliseconds++;
	}
	
	if(_auto_manual_switch_timer_miliseconds == 5000){//5seconds
		_auto_manual_switch_timer_miliseconds=0;
		_mode=_auto;
	}
	
}

ISR(ADC_vect){
	switch(_adc_target){
		case _x_joystick:
		_adc_target=_x_joystick;
		_x_read=ADCW;
		ADMUX=_y_joystick;
		break;
		case _y_joystick:
		_y_read=1023-ADCW;
		ADMUX=_tempature;
		break;
		case _tempature:
		_tempature_read=ADCW;
		ADMUX=_light;
		break;
		case _light:
		_light_read=ADCW;
		ADMUX=_x_joystick;
		break;
	}
	_adc_target=(_adc_target+1)%4;
	mindwizard_(_x_read, _y_read);

	_x_read_signed = (signed int)_x_read - 512;
	_y_read_signed = (signed int)_y_read - 512;

	if(abs_(_x_read_signed)<256){
		_x_read_signed=0;
	}
	
	if(abs_(_y_read_signed)<256){
		_y_read_signed=0;
	}
	
	if((_x_read_signed != 0 || _y_read_signed != 0)){
		_mode=_manual;
		_auto_manual_switch_timer_miliseconds = 0;
	}
}