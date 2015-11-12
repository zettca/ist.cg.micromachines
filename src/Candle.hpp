/*
 * Candle.hpp
 *
 * */

#ifndef __CANDLE_H
#define __CANDLE_H

#include "StaticObject.hpp"
#include "LightSource.hpp"
#include <GL/glut.h>

class Candle : public StaticObject {
	public:
		Candle(GLenum lightNum, double x, double y, double z);
		~Candle();
		void draw();
		void toggleLight();
	private:
		LightSource *_lightsource;

};

#endif //__CANDLE_H
