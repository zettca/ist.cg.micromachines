/*
 * Butter.cpp
 *
 * */

#include "Butter.hpp"
#include <iostream>
#include "Vector3.hpp"
#include <math.h>

#include "myunits.hpp"
using namespace std;
#include <GL/glut.h>

#include "debug.hpp"
#include "game_config.hpp"


Butter::Butter() : Obstacle(){

}

Butter::Butter(double x, double y, double z) : Obstacle(x, y, z){

}

Butter::~Butter(){

}

void Butter::updateBox(){
	_box.changeTo(BUTTER_SIZE_X, BUTTER_SIZE_Y, *getPosition());
}

/* Draws a cube of butter on x, y, z position */
void Butter::draw(){
	//cout << "Butter::draw()" << endl;
	Vector3 *pos = this->getPosition();

	glPushMatrix();
		glTranslatef(pos->getX(), pos->getY(), pos->getZ());
		glScalef(BUTTER_SIZE_X, BUTTER_SIZE_Y, BUTTER_SIZE_Z);
		glColor3f(0.8f, 0.8f, 0.5f);
		glutSolidCube(1);
	glPopMatrix();

	Obstacle::draw();
}

bool Butter::processCollisionWith(GameObject &obj){
	D_TRACE();
	if(GameObject::processCollisionWith(obj)){
		DynamicObject &dobj = ((DynamicObject&)obj);

		dobj.setSpeed(dobj.getSpeed()*2);
		return true;
	}
	return false;
}
