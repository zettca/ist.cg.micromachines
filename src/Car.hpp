/*
 * Car.hpp
 *
 * */

#ifndef __CAR_H
#define __CAR_H

#include "DynamicObject.hpp"
#include "SpotLightSource.hpp"
#include "LightSource.hpp"

class Car : public DynamicObject {
	public:
		Car(GLenum lightNumL, GLenum LightNumR);
		~Car();
		void draw();
		void update(double delta_t);
		double rotateZ(double deg);
		double getXYAngle();
		void toggleLight();
		bool setGoingForward(bool forward);
		bool isGoingForward();
		void setGhost(bool status);
		void setXYAngle(double deg);
		void updateBox();
		void reset();
		bool setHeadLightStateL(bool state);
		bool setHeadLightStateR(bool state);
	private:
		SpotLightSource *createSpotLight(GLenum lightNum);
		double _angle_deg;
		bool _isGoingForward;
		bool _isGhost;
		Material *_tireMaterial;
		SpotLightSource *_headLightL;
		SpotLightSource *_headLightR;
};

#endif //_CAR_H
