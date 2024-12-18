/*
* R_A_COMM_ENGINE.cpp
*
* Created: 12/5/2023 9:07:12 AM
*  Author: 6gioc
*/

#include "TOOLBOX.h"

void r_a_comm_setup_(void){
	DDRD=0x00;
}

void r_a_comm_update_(void){
	
	
	if(_mode != _auto){
		return;
	}
	unsigned char _pine = PINE;
	unsigned char _automatic_gun_shot_trigger_value = (_pine & (TRUE<<PE6)!=FALSE);
	if(_automatic_gun_shot_trigger_value != FALSE){
		_gun_shot_triggered=TRUE;
	}
	else{
		_gun_shot_triggered=FALSE;
	}
	portgcontroler_(PG0, _automatic_gun_shot_trigger_value);
	
	unsigned char _pind = PIND;
	unsigned int _abs_ymotor_speed = 0;
	unsigned int _abs_xmotor_speed = 0;
	
	if(_pind & (TRUE<<PD3)){//y motor speed control
		_abs_ymotor_speed = 100;
	}
	else{
		_abs_ymotor_speed = 500;
	}
	
	if(_pind & (TRUE<<PD0)){//x motor speed control
		_abs_xmotor_speed = 100;
	}
	else{
		_abs_xmotor_speed = 500;
	}
	
	signed int _xmotor_auto_speed=0;
	signed int _ymotor_auto_speed=0;
	
	const signed char _xmotor_auto_speed_invert=1;
	const signed char _ymotor_auto_speed_invert=1;
	
	if(_pind&(TRUE<<PD4)){//x+r_a
		_xmotor_auto_speed = _abs_xmotor_speed * _xmotor_auto_speed_invert;
	}
	else if(_pind&(TRUE<<PD5)){//x-r_a
		_xmotor_auto_speed = -1 * _abs_xmotor_speed * _xmotor_auto_speed_invert;
	}
	
	if(_pind&(TRUE<<PD6)){//y+r_a
		_ymotor_auto_speed = _abs_ymotor_speed * _ymotor_auto_speed_invert;
	}
	else if(_pind&(TRUE<<PD7)){//y-r_a
		_ymotor_auto_speed = -1 * _abs_ymotor_speed * _ymotor_auto_speed_invert;
	}
	
	stepmotor_update_( _xmotor_auto_speed,_ymotor_auto_speed);
	
}