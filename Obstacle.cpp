/*
 *  Obstacle.cpp
 *  Tanks
 *
 *  Created by Chris Cooper on 09-11-10.
 *
 */

#include "header/Obstacle.h"
#include "header/Cutils.h"
#include "header/Globals.h"
#include "header/BmpLoader.h"



Obstacle::Obstacle(float positionX, float positionZ, float radius){
	this->posX = positionX;
	this->posZ = positionZ;
	this->radius = radius;
}

void BalokPelindung(koordinat SisiSatu,koordinat SisiDua,koordinat SisiTiga,koordinat SisiEmpat){
	GLuint _textureBalok = loadTexture("img/bricks.bmp");
	glPushMatrix();
    	glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, _textureBalok );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTranslatef(0,0,-6);
        
        glBegin(GL_QUADS); //Belakan
            glTexCoord3f(1.0,1.0,0);  glVertex3f(SisiSatu.x,SisiSatu.y,SisiSatu.z);
            glTexCoord3f(0.0,0.0,0);  glVertex3f(SisiDua.x,SisiDua.y,SisiDua.z);
            glTexCoord3f(1.0,0.0,0);  glVertex3f(SisiTiga.x,SisiTiga.y,SisiTiga.z);
            glTexCoord3f(0.0,1.0,0);  glVertex3f(SisiEmpat.x,SisiEmpat.y,SisiEmpat.z);
        glEnd();
        
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
void Obstacle::drawSelf(){
	koordinat data[100];
	glColor4f(1.0f, 9.0f, 9.0f, 9.0f - (radarVisionMagnitude*0.9f));
	 // Grass
	glPushMatrix();
		glTranslatef(this->posX, 0.0f, this->posZ);
		glutSolidCube(this->radius+3);
	glPopMatrix();
}


float Obstacle::givePosX(){
	return this->posX;
}

float Obstacle::givePosZ(){
	return this->posZ;
}

float Obstacle::giveRadius(){
	return this->radius;
}


