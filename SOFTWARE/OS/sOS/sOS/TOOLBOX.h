/*
* TOOLBOX.h
*
* Created: 7/26/2023 1:41:46 AM
*  Author: 6gioc
*/
#pragma once

#ifndef TOOLBOX_H_
#define TOOLBOX_H_





#endif /* TOOLBOX_H_ */

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define T2INTITIAL 6

enum _adc_list{
	_x_joystick,
	_y_joystick,
	_tempature,
	_light,
};

enum MODELIST{
	_stop,
	_auto,
	_manual,
	_tutorial_sentence_1,
	_tutorial_sentence_2,
};

enum CONAMICODE_JOYSITICK_ENTER{
	_up,
	_down,
	_left,
	_right,
	_nodirection,
};

enum TUTORIAL_STATUS{
	_joystick_tutorial,
	_button_tutorial,
	_tutorial_ended,
};

void startup_(void);
void update_(void);
void display_setup_(void);
void display_update_(void);
void stepmotor_setup_(void);
void stepmotor_update_(signed int _xval, signed int _yval);
void r_a_comm_setup_(void);
void r_a_comm_update_(void);





void portacontroler_(unsigned char _bit, unsigned char _logical_value);
void portbcontroler_(unsigned char _bit, unsigned char _logical_value);
void portccontroler_(unsigned char _bit, unsigned char _logical_value);
void portgcontroler_(unsigned char _bit, unsigned char _logical_value);

unsigned char getbitofbyte_(unsigned char _byte, unsigned char _bit);
unsigned int abs_(signed int _value);


extern unsigned char _mind[8];
extern unsigned char _mode;
extern unsigned char _mode_list[10][50];
extern unsigned int _dotmatrix_animation_move;

extern unsigned char _dotmatrix_brightness;
extern unsigned int _auto_manual_switch_timer_miliseconds;

extern unsigned char _tutorial_stage;
extern unsigned char _conami_code_progress;
extern unsigned char _gun_shot_triggered;
