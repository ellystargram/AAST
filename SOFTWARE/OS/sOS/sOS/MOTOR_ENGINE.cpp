/*
* MOTOR_ENGINE.cpp
*
* Created: 8/3/2023 4:29:37 PM
*  Author: 6gioc
*/

#include "TOOLBOX.h"

void manual_shot_button_pressed_(void);

enum _stepmotor_phase_choose{
	_1phase,
	_2phase,
	_12phase,
	_phase_count,
};
enum _stepmotor_direction_choose{
	_retrograde=-1,
	_hold=0,
	_prograde=1,
};
enum _stepmotor_xy_define{
	_xmotor=0,
	_ymotor=1,
	_motor_count=2,
};

const unsigned char _stepmotor_sequence[_phase_count][8]={
	{0x11, 0x22, 0x44, 0x88,0x11,0x22,0x44,0x88},//_1phase
	{0x33, 0x66, 0xcc, 0x99,0x33,0x66,0xcc,0x99},//_2phase
	{0x11, 0x33, 0x22, 0x66, 0x44, 0xcc, 0x88, 0x99},//_12phase
};
signed char _stepmotor_direction[_motor_count];
unsigned int _stepmotor_rpm[_motor_count];
const unsigned char _stepmotor_control_invert[_motor_count]={FALSE, FALSE};
signed char _stepmotor_current_step[_motor_count]={0,};
unsigned char _stepmotor_current_phase_choose[_motor_count];
unsigned char _portc_output;

void stepmotor_setup_(void){
	DDRA=0xff;
	_stepmotor_direction[_xmotor]=_hold;
	_stepmotor_direction[_ymotor]=_hold;
	_stepmotor_current_phase_choose[_xmotor]=_2phase;
	_stepmotor_current_phase_choose[_ymotor]=_2phase;
	_stepmotor_rpm[_xmotor]=0;
	_stepmotor_rpm[_ymotor]=0;
}
void stepmotor_update_(signed int _xval, signed int _yval){
	if(_xval==0 ||_gun_shot_triggered==TRUE){
		_stepmotor_direction[_xmotor]=_hold;
		_stepmotor_rpm[_xmotor]=_hold;
	}
	else if(_xval<0){
		if(_stepmotor_control_invert[_xmotor]==FALSE){
			_stepmotor_direction[_xmotor]=_retrograde;
		}
		else{
			_stepmotor_direction[_xmotor]=_prograde;
		}
		_stepmotor_rpm[_xmotor]=abs_(_xval);
	}
	else{
		if(_stepmotor_control_invert[_xmotor]==FALSE){
			_stepmotor_direction[_xmotor]=_prograde;
		}
		else{
			_stepmotor_direction[_xmotor]=_retrograde;
		}
		_stepmotor_rpm[_xmotor]=abs_(_xval);
	}
	
	if(_yval==0 ||_gun_shot_triggered==TRUE){
		_stepmotor_direction[_ymotor]=_hold;
		_stepmotor_rpm[_ymotor]=_hold;
	}
	else if(_yval<0){
		if(_stepmotor_control_invert[_ymotor]==FALSE){
			_stepmotor_direction[_ymotor]=_retrograde;
		}
		else{
			_stepmotor_direction[_ymotor]=_prograde;
		}
		_stepmotor_rpm[_ymotor]=abs_(_yval);
	}
	else{
		if(_stepmotor_control_invert[_ymotor]==FALSE){
			_stepmotor_direction[_ymotor]=_prograde;
		}
		else{
			_stepmotor_direction[_ymotor]=_retrograde;
		}
		_stepmotor_rpm[_ymotor]=abs_(_yval);
	}
	
	if(_stepmotor_direction[_xmotor]==_hold){
		TCCR1B=TCCR1B&(~((TRUE<<CS10)|(TRUE<<CS12)));
		}else{
		TCCR1B=TCCR1B|((TRUE<<CS10)|(TRUE<<CS12));
	}
	if(_stepmotor_direction[_ymotor]==_hold){
		TCCR3B=TCCR3B&(~((TRUE<<CS30)|(TRUE<<CS32)));
		}else{
		TCCR3B=TCCR3B|((TRUE<<CS30)|(TRUE<<CS32));
	}
	
	
	OCR1A=(1100-_stepmotor_rpm[_xmotor]*2);
	OCR3AH=((1100-_stepmotor_rpm[_ymotor]*2)&0xff00)>>8;
	OCR3AL=(1100-_stepmotor_rpm[_ymotor]*2)&0x00ff;
	
	if(TCNT1>OCR1A){
		TCNT1 = 0;
	}
	
	unsigned int _tcnt3 = TCNT3H;
	_tcnt3 = (_tcnt3<<8)|TCNT3L;
	
	if(_tcnt3>(1100-_stepmotor_rpm[_ymotor]*2)){
		TCNT3H=0;
		TCNT3L=0;
	}
	
	if(_xval==_hold){
		portacontroler_(PA0,FALSE);
	}
	else{
		portacontroler_(PA0,TRUE);
	}
	
	if(_yval==_hold){
		portacontroler_(PA1,FALSE);
	}
	else{
		portacontroler_(PA1,TRUE);
	}
	
	if(_mode == _manual){
		manual_shot_button_pressed_();
	}
}

ISR(TIMER1_COMPA_vect){//x
	_portc_output=(_portc_output&0xf0)|(_stepmotor_sequence[_stepmotor_current_phase_choose[_xmotor]][_stepmotor_current_step[_xmotor]]&0x0f);
	if(_stepmotor_current_phase_choose[_xmotor]==_1phase||_stepmotor_current_phase_choose[_xmotor]==_2phase){
		if(_stepmotor_direction[_xmotor]==_prograde){
			_stepmotor_current_step[_xmotor]=(_stepmotor_current_step[_xmotor]+1)%4;
		}
		else if(_stepmotor_direction[_xmotor]==_retrograde){
			if(_stepmotor_current_step[_xmotor]==0){
				_stepmotor_current_step[_xmotor]=3;
			}
			else{
				_stepmotor_current_step[_xmotor]--;
				
			}
		}
	}
	else if(_stepmotor_current_phase_choose[_xmotor]==_12phase){
		if(_stepmotor_direction[_xmotor]==_prograde){
			_stepmotor_current_step[_xmotor]=(_stepmotor_current_step[_xmotor]+1)%8;
		}
		else if(_stepmotor_direction[_xmotor]==_retrograde){
			if(_stepmotor_current_step[_xmotor]==0){
				_stepmotor_current_step[_xmotor]=7;
			}
			else{
				_stepmotor_current_step[_xmotor]--;
			}
		}
	}
	PORTC=_portc_output;
}
ISR(TIMER3_COMPA_vect){//y
	_portc_output=(_portc_output&0x0f)|(_stepmotor_sequence[_stepmotor_current_phase_choose[_ymotor]][_stepmotor_current_step[_ymotor]]&0xf0);
	if(_stepmotor_current_phase_choose[_ymotor]==_1phase||_stepmotor_current_phase_choose[_ymotor]==_2phase){
		if(_stepmotor_direction[_ymotor]==_prograde){
			_stepmotor_current_step[_ymotor]=(_stepmotor_current_step[_ymotor]+1)%4;
		}
		else if(_stepmotor_direction[_ymotor]==_retrograde){
			if(_stepmotor_current_step[_ymotor]==0){
				_stepmotor_current_step[_ymotor]=3;
			}
			else{
				_stepmotor_current_step[_ymotor]--;
			}
		}
	}
	else if(_stepmotor_current_phase_choose[_ymotor]==_12phase){
		if(_stepmotor_direction[_ymotor]==_prograde){
			_stepmotor_current_step[_ymotor]=(_stepmotor_current_step[_ymotor]+1)%8;
		}
		else if(_stepmotor_direction[_ymotor]==_retrograde){
			if(_stepmotor_current_step[_ymotor]==0){
				_stepmotor_current_step[_ymotor]=7;
			}
			else{
				_stepmotor_current_step[_ymotor]--;
			}
		}
	}
	PORTC=_portc_output;
}

void manual_shot_button_pressed_(void){
	const unsigned char _pine = PINE;
	
	if(_pine&(TRUE<<PE4)){
		_gun_shot_triggered=TRUE;
		portgcontroler_(PG0,TRUE);
	}
	else{
		_gun_shot_triggered=FALSE;
		portgcontroler_(PG0,FALSE);
	}
}