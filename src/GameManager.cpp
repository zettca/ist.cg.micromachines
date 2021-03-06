/*
 * GameManager.cpp
 *
 * */


#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
using namespace std;
#include <string.h>
#include <ctype.h>
#include "GameManager.hpp"
#include "LightSource.hpp"

#include <GL/glut.h>
#include "debug.hpp"
#include "game_config.hpp"
#include "OrthogonalCamera.hpp"
#include "PerspectiveCamera.hpp"
#include "SpotLightSource.hpp"
#include "LightSource.hpp"

#include <typeinfo>

GameManager::GameManager(){
	initGL();

	D_TRACE();
	_enableTeaPot = false;

	/* Use (lightNum++) when creating a new light object */
	GLenum lightNum = GL_LIGHT0;

	/* Textures for Game pause and lose messages*/
	_tpaused = new Texture((char*) "game_paused.png");
	_tover = new Texture((char*) "game_over.png");

	/* Global Directional Light */
	_globalLight = new SpotLightSource(lightNum++);
	_globalLight->setAmbient(0.05, 0.05, 0.05, 1.0);
	_globalLight->setDiffuse(1.0, 1.0, 1.0, 1.0);
	_globalLight->setSpecular(0.1, 0.1, 0.1, 1.0);
	_globalLight->setPosition(0, 0, GAME_TABLE_LIMIT/4);
	_globalLight->setDirection(0, 0, -1);
	_globalLight->setCutOff(180);
	_globalLight->setExponent(1);

	/** Key State Array **/
	_isKeyPressed = (bool*) malloc(4 * sizeof(bool));
	memset(_isKeyPressed, false, 4);

	/** Game Objects **/
	_roadside = new Roadside();
	_game_objects.push_back(_roadside);

	_table = new Table(0, 0, -GAME_TABLE_LIMIT*1.001);
	_game_objects.push_back(_table);

	_butters.push_back(new Butter(-1.1, 0.7, BUTTER_SIZE_Z/2.));
	_butters.push_back(new Butter(-1.3, 0.0, BUTTER_SIZE_Z/2.));
	_butters.push_back(new Butter(-1.1,-0.8, BUTTER_SIZE_Z/2.));
	_butters.push_back(new Butter(-0.4, 0.8, BUTTER_SIZE_Z/2.));
	_butters.push_back(new Butter( 0.7,-1.1, BUTTER_SIZE_Z/2.));

	Candle *_redCandle;
	_candles.push_back(_redCandle = new Candle(lightNum++, 1.0, -0.4, 0));
	_candles.push_back(new Candle(lightNum++, 1.0,  1.0, 0));
	_candles.push_back(new Candle(lightNum++,-0.8, -0.8, 0));
	_candles.push_back(new Candle(lightNum++,-0.8,  0.8, 0));
	_candles.push_back(new Candle(lightNum++, 0.0,  1.4, 0));
	//_candles.push_back(new Candle(lightNum++,-0.2, -1.4, 0));

	_redCandle->setLightColor(1, 0, 0);

	/* Put candles inside _game_objects */
	for( auto c : _candles ){
		_game_objects.push_back( c );
	}

	/* Put butters inside _game_objects */
	for( auto b : _butters ){
		_game_objects.push_back( b );
	}

	_car = new Car(lightNum++, lightNum++);

	/* Oranges */
	for (int i=0; i<3; i++){
		Orange *o = new Orange();
		_oranges.push_back(o);
		_game_objects.push_back(o);
	}

	_car->reset();
	_game_objects.push_back(_car);


	for (int i=0; i<5; i++){
		Car *c = new Car(lightNum, lightNum);
		c->setPosition(-1.5f+(i*0.1f), 1.5f, 0);
		c->rotateZ(90);
		_car_lives.push_back(c);
	}

	/** Cameras **/

	/* Orthogonal Camera */
	_cameras.push_back(
		new OrthogonalCamera(
			-GAME_WORLD_MAX, GAME_WORLD_MAX,
			-GAME_WORLD_MAX, GAME_WORLD_MAX,
			-GAME_WORLD_MAX, GAME_WORLD_MAX
		)
	);

	/* Fixed perspective camera from table top */
	PerspectiveCamera *fixedPerspCam = new PerspectiveCamera(60, 1, 0.1, 10.);
	fixedPerspCam->setPosition(0, 0, 2.65);
	fixedPerspCam->setCenter(Vector3(0, 0.0, -1));
	fixedPerspCam->setUp(Vector3(0, 1.2, 0));
	_cameras.push_back(fixedPerspCam);

	/* Moving camera that follows the car */
	_movingCamera = new PerspectiveCamera(60, 1, 0.1, 10.);
	_cameras.push_back(_movingCamera);

	/* (not required) Fixed perspective from table side */
	PerspectiveCamera *fixedPerspCam2 = new PerspectiveCamera(60, 1, 0.1, 10.);
	fixedPerspCam2->setPosition(0, -3, 0);
	fixedPerspCam2->setCenter(Vector3(0, 1, 0));
	fixedPerspCam2->setUp(Vector3(0, 0, 1));
	_cameras.push_back(fixedPerspCam2);

	_currentCamera = _cameras[0];

}

GameManager::~GameManager(){
	D_TRACE();
	free(_isKeyPressed);

	for( auto o : _game_objects ) {
		delete ( o );
	}
}

void GameManager::initGL(){
#ifdef SINGLEBUF
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
#endif
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(-1, -1);
	glutCreateWindow(GAME_WINDOW_TITLE);

	/* Enable automatic normal normalization */
	glEnable(GL_NORMALIZE);

	/* Enable textures */
	glEnable(GL_TEXTURE_2D);

	/* Transparency */
	glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* global lighting stuff */
	GLfloat ambient[4] = {0.13, 0.15, 0.15, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
}

void GameManager::reshape(GLsizei w, GLsizei h){
	glViewport(0, 0, w, h);
	/* Cameras take care of aspect ratio too */
}

void drawStartLine(GLfloat x, GLfloat y, GLfloat z){
	static Material *m = new Material(0.0, 0.0, 0.0);
	glPushMatrix();
		m->apply();
		glTranslatef(x, y, z);
		glScalef(0.2, 3.0, 0.02);
		glutSolidCube(0.1);
	glPopMatrix();
}

void GameManager::drawGameState(Texture *t){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	_cameras[0]->update();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	#define POLY_WIDTH 	1.6
	#define POLY_HEIGHT 0.6
	glTranslatef(0.0-(POLY_WIDTH/2), 0.6, 1.0);

	t->apply();

	glBegin(GL_POLYGON);
		glTexCoord2d(0, 0);
		glVertex3f(0.0, 0.0, 0.0);

		glTexCoord2d(1, 0);
		glVertex3f(POLY_WIDTH, 0.0, 0.0);

		glTexCoord2d(1, 1);
		glVertex3f(POLY_WIDTH, POLY_HEIGHT, 0.0);

		glTexCoord2d(0, 1);
		glVertex3f(0.0, POLY_HEIGHT, 0.0);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	_currentCamera->update();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GameManager::display(){
	D_TRACE();

	glBindTexture(GL_TEXTURE_2D, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	_currentCamera->update();


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.05f, 0.05f, 0.05f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	_globalLight->draw();

	drawStartLine(0.3, -0.2, 0.0);

	for( auto o : _game_objects ) {
		o->draw();
	}

	/* Teapot for light debugging */
	if(_enableTeaPot){
		glPushMatrix();
			glLoadIdentity();
			glTranslatef(	GAME_TABLE_LIMIT/7,
							GAME_TABLE_LIMIT/5,
							GAME_TABLE_LIMIT/15);
			glRotatef(90.0, 1, 0, 0);
			glutSolidTeapot(GAME_TABLE_LIMIT/10.);
		glPopMatrix();
	}

	/* Draw OSD */
	drawOSD();


#ifdef SINGLEBUF
	glFlush();
#else
	glutSwapBuffers();
#endif
}

/* Does not restore matrices */
void GameManager::drawOSD(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	_cameras[0]->update();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	bool lighting_on = glIsEnabled(GL_LIGHTING);
	int lives = _lives;
	glDisable(GL_LIGHTING);
	for( auto l : _car_lives ){
		l->draw();
		if (--lives == 0) break;
	}

	if(_isPaused){
		drawGameState(_tpaused);
	}
	else if(_isLoseState){
		drawGameState(_tover);
	}

	if (lighting_on)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

}

void GameManager::keyPressed(unsigned char key, int x, int y){
	cout << "[" << key << "] ";
	switch(toupper(key)){
		case '1':
		case '2':
		case '3':
		case '4':
			_currentCamera = _cameras[key-'1'];
			glutPostRedisplay();	// force draw
			cout << "Changed to camera " << (key) << endl;
			break;
		case 'R':
			resetGame();
			cout << "Game resetted" << endl;
			break;
		case 'S':
			_isPaused = !_isPaused;
			cout << "Game pause toggled" << endl;
			break;
		case 'L':
			cout << "Global lighting toggled" << endl;
			toggleLighting();
			break;
		case 'C':
			cout << "Candles lighting toggled" << endl;
			toggleCandles();
			break;
		case 'B':
			cout << "Box drawing toggled" << endl;
			for( auto o : _game_objects )
				o->toggleBox();
			break;
		case 'G':
			cout << "Gouraud shading toggled" << endl;
			toggleShading();
			break;
		case 'N':
			cout << "Main lighting toggled" << endl;
			_globalLight->toggleState();
			_car->setHeadLightStateL(!_globalLight->getState());
			_car->setHeadLightStateR(!_globalLight->getState());
			break;
		case 'H':
			cout << "Car light toggled" << endl;
			if (!_globalLight->getState()){
				_car->toggleLight();
			}
			break;
		case 'P':
			cout << "No-clip toggled" << endl;
			_isNoClip = !_isNoClip;
			_car->setGhost(_isNoClip);
			break;
		case 'T':	// Only Uppercase T
			// Teapot for debugging.
			_enableTeaPot = !_enableTeaPot;
			break;
		default:
			cout << "Key has no binding" << endl;
			break;
	}
}

void GameManager::onTimer(int val){
	if (!_isPaused) update(((double) val)/1000.);
	glutPostRedisplay();
}

void GameManager::carIsKilled(){
	if (--_lives <= 0){
		endGame();
	} else{
		_car->reset();
	}
}

void GameManager::endGame(){
	/*
		Stop car
		Display losing message
		Put everything grey ?
	*/
	_car->setSpeed(0, 0, 0);
	_isLoseState = true;
}

void GameManager::resetGame(){
	_lives = 5;
	_car->reset();
	_isLoseState = false;
}

void GameManager::update(double delta_t){
	Vector3 speed_v = _car->getSpeed();

	D_TRACE(<< "speed before: " << VECTOR3_STR(speed_v));

	double speed = _car->getSpeed().getXYModulus();

	/* update moving camera so it follows the car */
	if( _currentCamera == _movingCamera ){
		Vector3 pos = *(_car->getPosition());

		Vector3 new_pos =
				pos												// pos do carro
			+	Vector3(0, 0, 0.25 + speed*0.125)				// aumeta Z com a velocidade
			+	speed_v * (_car->isGoingForward()?-0.3:0.3)		// afasta-se com a velocidade
			-	Vector3(0.5, _car->getXYAngle()					// mantem distancia minima ao carro
			);

		/* smooth camera movement */
		new_pos =	new_pos * (1.-GAME_CAMERA_MOVEMENT_SMOTHENESS)
					+ _movingCamera->getPosition()
						->operator*(GAME_CAMERA_MOVEMENT_SMOTHENESS);

		_movingCamera->setPosition(new_pos);
		_movingCamera->setCenter(pos);		// olha para o carro
	}

	/* change car speed according to keys */

	if(_isKeyPressed[LEFT] ^ _isKeyPressed[RIGHT]){
		double da = delta_t*GAME_CAR_ANGLE_ACCELARATION(speed)*(_isKeyPressed[LEFT] ? 1 : -1);
		if(!_car->isGoingForward()) da *= -1.0;
		_car->setSpeed(_car->getSpeed().rotateZ(da));
		_car->rotateZ(da);
	}

	if((_isKeyPressed[UP] ^ _isKeyPressed[DOWN]) && !_isLoseState){
		double da = delta_t*GAME_CAR_SPEED_ACCELARATION*(_isKeyPressed[UP] ? 1 : -0.5);
		if(_car->getSpeed().getXYModulus() == 0.){
			_car->setSpeed(Vector3(da, _car->getXYAngle()));
			_car->setGoingForward(da >= 0);
		}else{
			if(!_car->isGoingForward()) da *= -1.0;
			_car->setSpeed(_car->getSpeed().increaseMod(da));
		}
	}

	/* update all objects */
	for( auto o: _game_objects ) {
		o->update( delta_t );
		o->updateBox();
	}

	checkCollisions();

}

void GameManager::checkCollisions(){

	if (_isNoClip || _isLoseState) return;

	/* check for collisions */
	for(vector<Orange*>::iterator i = _oranges.begin(); i != _oranges.end(); i++){
		if ((*i)->processCollisionWith(*_car)){
			D_TRACE( << "COLISION!! " << typeid(*i).name() << " " << glutGet(GLUT_ELAPSED_TIME));
			carIsKilled();
		};
	}
	for(vector<Butter*>::iterator i = _butters.begin(); i != _butters.end(); i++){
		if( (*i)->processCollisionWith(*_car)){
			D_TRACE( << "COLISION!! " << typeid(*i).name() << " " << glutGet(GLUT_ELAPSED_TIME));
		};
	}
	if (_roadside->processCollisionWith(*_car)){
		D_TRACE( << "COLISION!! " << typeid(_roadside).name() << " " << glutGet(GLUT_ELAPSED_TIME));
	}
	if (_car->isOutOfBounds()){	// Car fell off the table
		carIsKilled();
	}
}

bool GameManager::toggleLighting(){
	/* returns new state */
	if(glIsEnabled(GL_LIGHTING)){
		glDisable(GL_LIGHTING);
		return false;
	}else{
		glEnable(GL_LIGHTING);
	}
	return true;
}

void GameManager::toggleCandles(){
	for( auto c : _candles ){
		c->toggleLight();
	}
}

void GameManager::toggleShading() {
	GLint param;
	glGetIntegerv(GL_SHADE_MODEL, &param);
	if (param == GL_SMOOTH){
		glShadeModel(GL_FLAT);
	}
	else {
		glShadeModel(GL_SMOOTH);
	}
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

void GameManager::orangeSpeedInc(float inc){
	for( auto o : _oranges ){
		o->setSpeed(o->getSpeed() * inc);
	}
}

void GameManager::orangeRespawn(){
	for( auto o : _oranges ){
		if (!o->isActive()) o->resetPosition();
	}
}

bool GameManager::isPaused(){
	return _isPaused;
}
