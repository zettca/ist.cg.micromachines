/*
 * game_config.hpp
 * Game configuration constants
 * 
 * */

#ifndef __GAME_CONFIG_INCLUDED
#define __GAME_CONFIG_INCLUDED

#include "myunits.hpp"



#define GAME_WINDOW_TITLE		"tg010/cg-micromachines"


// Time between updates in ms
#define GAME_TIMER_PERIOD		20



// Key accelaraton
#define GAME_CAR_ANGLE_ACCELARATION(v)	160*v
#define GAME_CAR_SPEED_ACCELARATION		1.5
#define GAME_CAR_SPEED_DRAG(v)		(0.015 + 110*1.225/2. * 0.81 * cm(10)*cm(10) * v * v)




// World map limits

#define	GAME_WORLD_MAX	1.6f


#endif //__GAME_CONFIG_INCLUDED