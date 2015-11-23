/*
 * Material.hpp
 *
 * */

#ifndef __MATERIAL_H
#define __MATERIAL_H

#include <GL/glut.h>
#include "Vector3.hpp"

class Material {
	public:
		Material();
		Material(GLfloat r, GLfloat g, GLfloat b);
		~Material();
		void setMaterial(GLfloat r, GLfloat g, GLfloat b);
		void setMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
		void setAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
		void setDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
		void setSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
		void setShininess(GLfloat shine);
		void setAlpha(GLfloat a);
		void draw();
	private:
		GLfloat _ambient[4];
		GLfloat _diffuse[4];
		GLfloat _specular[4];
		GLfloat _shine;
};

#endif //__MATERIAL_H
