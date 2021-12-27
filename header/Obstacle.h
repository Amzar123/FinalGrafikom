/*
 *  Obstacle.h
 *  Tanks
 *
 *  Created by Chris Cooper on 09-11-10.
 *
 */

#include "Definitions.h"

#ifndef _OBSTACLE_H
#define _OBSTACLE_H

typedef struct{
	float x;
	float y;
	float z;
}koordinat;

void BalokPelindung(koordinat SisiSatu,koordinat SisiDua,koordinat SisiTiga,koordinat SisiEmpat);

class Obstacle {
public:
	Obstacle(float positionX, float positionZ, float radius);
	void drawSelf();
	float givePosX();
	float givePosZ();
	float giveRadius();
	
private:
	float radius;
	float posX, posZ;
};

#endif
