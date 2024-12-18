/*
* sOS.cpp
*
* Created: 7/26/2023 1:37:32 AM
* Author : 6gioc
* Designed to operate on Atmega128
*/


#include "TOOLBOX.h"


int main(void)
{
	startup_();
	while (TRUE){
		update_();
	}
}

