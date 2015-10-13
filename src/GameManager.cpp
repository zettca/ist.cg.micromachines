/*
 * GameManager.cpp
 *
 * */

#include <iostream>
using namespace std;
#include <string.h>
#include "GameManager.hpp"
#include "Camera.hpp"
#include "GameObject.hpp"
//#include "LightSource.hpp"

#include "Roadside.hpp"
#include "Butter.hpp"
#include "Orange.hpp"
#include "Car.hpp"
#include <GL/glut.h>
#include "debug.hpp"


#define	WORLD_MAX	1.6f

GameManager::GameManager(){
	D_TRACE();
	
	_isKeyPressed = (bool*) malloc(4 * sizeof(bool));
	memset(_isKeyPressed, false, 4);

	_game_objects.push_back(new Roadside());
	
	_car = new Car();
	_game_objects.push_back(_car);
	
	_game_objects.push_back(new Butter(-0.9, 1, 0));
	_game_objects.push_back(new Butter(-0.9, 0, 0));
	_game_objects.push_back(new Butter(-0.9, -1, 0));
	_game_objects.push_back(new Butter(0.2, 0.4, 0));
	_game_objects.push_back(new Butter(0.7, -0.7, 0));
	_game_objects.push_back(new Orange(-1.3, 0.7, 0));
	_game_objects.push_back(new Orange(0.85, 0.9, 0));
	_game_objects.push_back(new Orange(1.3, -0.9, 0));

}

GameManager::~GameManager(){
	D_TRACE();
	free(_isKeyPressed);
	
	for(vector<GameObject*>::iterator i = _game_objects.begin();
		i != _game_objects.end(); i++){
		delete (*i);
	}
}

void GameManager::init(){
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(-1, -1);
	glutCreateWindow("tg010/cg-micromachines");
}

void GameManager::reshape(GLsizei w, GLsizei h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if(w<h){
		glOrtho(	-WORLD_MAX,	WORLD_MAX,
					-WORLD_MAX*h/w,	WORLD_MAX*h/w,
					-WORLD_MAX,	WORLD_MAX);
	}else{
		glOrtho(	-WORLD_MAX*w/h,	WORLD_MAX*w/h,
					-WORLD_MAX,	WORLD_MAX,
					-WORLD_MAX,	WORLD_MAX);
	}

	/* pos, look at, up_v*/
	gluLookAt(	0, 0, 0,  // position of the eye point
				0, 0, -1,  // center - position of the reference point
				0, 1, 0); // up - specifies the direction of the up point

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void drawCube(GLfloat x, GLfloat y, GLfloat z){
	glPushMatrix();

	glTranslatef(x, y, z);
	glColor3f(0.1f, 0.3f, 0.1f);
	glutSolidCube(3.0f);

	glPopMatrix();
}

void GameManager::display(){
	D_TRACE();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	drawCube(0, 0, 0);

	for(vector<GameObject*>::iterator i = _game_objects.begin();
		i != _game_objects.end(); i++){
		(*i)->draw();
	}

	glFlush();
}

void GameManager::keyPressed(unsigned char key, int x, int y){
	
}

void GameManager::onTimer(int val){
	update(((double) val)/1000.);
	glutPostRedisplay();
}

void GameManager::idle(){

}

#define ANGLE_DELTA	5.
#define SPEED_DELTA	0.05

void GameManager::update(double delta_t){
	D_TRACE(<< "speed before");
	_car->getSpeed().println();

	if(_isKeyPressed[LEFT] ^ _isKeyPressed[RIGHT]){
		_car->setSpeed(_car->getSpeed().rotateZ(
			_isKeyPressed[LEFT] ? ANGLE_DELTA : -ANGLE_DELTA
		));
	}
	if(_isKeyPressed[UP] ^ _isKeyPressed[DOWN]){
		_car->setSpeed(_car->getSpeed().increaseMod(
			_isKeyPressed[UP] ? SPEED_DELTA : -SPEED_DELTA
		));
	}
	_car->update(delta_t);
}

void GameManager::setKeyPressed(int glut_key, bool status){
	switch (glut_key){
		case GLUT_KEY_UP:		_isKeyPressed[UP] = status; break;
		case GLUT_KEY_LEFT:		_isKeyPressed[LEFT] = status; break;
		case GLUT_KEY_DOWN:		_isKeyPressed[DOWN] = status; break;
		case GLUT_KEY_RIGHT:	_isKeyPressed[RIGHT] = status; break;
		default: break;
	}
}
