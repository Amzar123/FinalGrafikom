/*
 *  Tank.cpp
 *  Tanks
 *
 *  Created by Chris Cooper on 09-11-04.
 *
 */

#include "header/Tank.h"
#include "header/Cutils.h"
#include "header/Globals.h"

Tank::Tank(float positionX, float positionZ, float initialRotation){
	this->speed = 0.0f;
	this->posX = positionX;
	this->posZ = positionZ;
	this->rotation = initialRotation;
	this->rotationSpeed = 0.0f;
	this->width = 0.5f;
	this->height = 0.5f;
	this->depth = 0.7f;
	this->turretRotation = 0.0f;
	this->turretRotationSpeed = 0.0f;
	this->health = 3;
	this->reloadTime = 20;
	this->reloadCounter = this->reloadTime;
	this->canSeePlayer = false;
	this->sightRange = 23.0f;
	this->sightCounter = 0;
	this->isAimed = false;
	this->destinX = positionX;
	this->destinZ = positionZ;
	this->atDestination = true;
	this->recoilStrength = 0.7f;
	this->recoilDistance = 0.0f;
	this->curRecoilForce = 0.0f;
	this->lastSightingX = positionX;
	this->lastSightingZ = positionZ;
	this->boostSpeed = 0;
	this->boostPower = 0.02f;
	this->shieldStrength = 0;
	this->shieldOpacity = 0.0f;
}

void Tank::accelerate(bool directionIsForward){
	if (directionIsForward){
		this->speed += 0.02;
	}
	else {
		this->speed -= 0.02;
	}
}

void Tank::boost(){
	this->boostSpeed += this->boostPower;
}

void Tank::activateShield(){
	if (this->hasShieldLeft()) {
		this->shieldOpacity = 1.0f;
		this->shieldStrength -= 1;
	}
}

bool Tank::hasShieldLeft(){
	return this->shieldStrength > 0;
}

void Tank::rotate(bool directionIsPositive){
	this->rotationSpeed *= 0.01f;
	if (directionIsPositive) {
		this->rotationSpeed += 3.5;
	}
	else {
		this->rotationSpeed -= 3.5;
	}
	if (this->boostSpeed > 0.02f){
		this->rotationSpeed /= (this->boostSpeed)*100.0f;
	}
}

void Tank::rotateTurret(float amount){
	this->turretRotationSpeed += amount;
	if (this->turretRotation > 180) {
		this->turretRotation -= 360;
	}
	else if (this->turretRotation < -180) {
		this->turretRotation += 360;
	}
}

bool Tank::fire(){
	if (this->reloadCounter <= 0){
		float angle = this->rotation + this->turretRotation;
		bullets.push_back(new Bullet(this->posX - 1.0*sin(angle * PI / 180), 
									0.65f,
									 this->posZ -1.0*cos(angle * PI / 180), this->speedX, this->speedZ,
									 angle
									 ));
		this->reloadCounter = this->reloadTime;
		this->curRecoilForce += this->recoilStrength;
		return true;
	}
	return false;
}

bool Tank::isHitBy(Bullet* theBullet){
	float x = theBullet->givePosX() - this->givePosX();
	float z = theBullet->givePosZ() - this->givePosZ();
	if (this->hasShieldLeft()) {
		return x*x+z*z < (this->width)*(this->width);
	}
	else {
		return x*x+z*z < (this->width*2)*(this->width*2);
	}
}

void Tank::move(){
	this->speedX = -(this->speed+this->boostSpeed) * sin(this->rotation * PI / 180);
	this->speedZ = -(this->speed+this->boostSpeed) * cos(this->rotation * PI / 180);

	if (this->canMoveTo(this->posX + this->speedX, this->posZ)){
		this->posX += this->speedX;
	}
	else{
		this->speedX *= 0.5f;
	}
	if (this->canMoveTo(this->posX, this->posZ + this->speedZ)){
		this->posZ += this->speedZ; 
	}
	else {
		this->speedZ *= 0.5f;
	}

	this->rotation += this->rotationSpeed;
	this->turretRotation -= this->rotationSpeed;
	this->turretRotation += this->turretRotationSpeed;
	this->turretRotationSpeed *= 0.5f;
	this->rotationSpeed *= 0.5f;
	this->speed *= 0.8f;
	this->boostSpeed *= 0.8f;
	if (this->rotation > 360.0f){
		this->rotation -= 360.0f;
	}
	else if (this->rotation < -360.0f){
		this->rotation += 360.0f;
	}
	if (this->turretRotation > 360.0f){
		this->turretRotation -= 360.0f;
	}
	else if (this->turretRotation < -360.0f){
		this->turretRotation += 360.0f;
	}
	this->reloadCounter -= 1;
	
	this->recoilDistance += this->curRecoilForce;
	this->curRecoilForce *= 0.5f;
	this->recoilDistance *= 0.8f;
	this->shieldOpacity *= 0.95f;
}

void Tank::runAI(){
	float dist = distanceBetween(this->givePosX(), this->givePosZ(), playerTank->givePosX(), playerTank->givePosZ());
    this->lastSightingX = playerTank->givePosX();//code untuk membuat musuh mencari player
    this->lastSightingZ = playerTank->givePosZ();//code untuk membuat musuh mencari player
	this->sightCounter -= 1;
	if (dist < this->sightRange){
		this->sightCounter = 10;
		this->canSeePlayer = true;
	}
	else {
		if (this->sightCounter < 0) {
			this->canSeePlayer = false;
			this->destinX = this->lastSightingX;
			this->destinZ = this->lastSightingZ;

		}
	}

	if(this->canSeePlayer){
		float bulletCyclesToTarget = dist/bulletSpeed;
		float pX = playerTank->givePosX() - bulletCyclesToTarget*playerTank->speed*sin(playerTank->rotation*PI/180) - bulletCyclesToTarget*this->speedX;
		float pZ = playerTank->givePosZ() - bulletCyclesToTarget*playerTank->speed*cos(playerTank->rotation*PI/180) - bulletCyclesToTarget*this->speedZ;
		this->turnTurretToward(this->angleTo(pX, pZ));
		if (this->isAimed){
			this->fire();
		}

	}
	else {
	}

	if (distanceBetween(this->posX, this->posZ, this->destinX, this->destinZ) > 5.0f) {
		this->atDestination = false;
	}
	else {
		this->atDestination = true;
	}


	if (!(rand() % 30) && this->canSeePlayer){
		this->destinX += rand() % 20 - 10;
		this->destinZ += rand() % 20 - 10;
	}

	if (this->atDestination){
		this->rotate(true);
		this->accelerate(true);
	}
	else {
		this->turnToward(this->angleTo(this->destinX, this->destinZ));
		this->accelerate(true);
	}
}


float Tank::angleTo(float x, float z){
	float deltaX = x - this->posX;
	
	float deltaZ = z - this->posZ;
	if (deltaX >= 0 && deltaZ >= 0) {
		return -180 + (( atan(deltaX/deltaZ))*180/PI);
	}
	else if (deltaX >= 0 && deltaZ < 0) {
		return (atan(deltaX/deltaZ)*180/PI);
	}
	else if (deltaX < 0 && deltaZ >= 0) {
		return 180 + ((atan(deltaX/deltaZ))*180/PI);
	}
	else /* (deltaX < 0 && deltaZ < 0) */{
		return ((atan(deltaX/deltaZ))*180/PI);
	}
	return 0;
}

void Tank::turnTurretToward(float newAngle){
	this->isAimed = false;
	float curAngle = this->rotation+this->turretRotation;
	if (curAngle > 180){
		curAngle -= 360;
	}
	else if (curAngle < -180){
		curAngle += 360;
	}
	if (curAngle > 90 && newAngle < -90){
		newAngle += 360;
	}
	else if (curAngle < -90 && newAngle > 90){
		newAngle -= 360;
	}
	if (curAngle < newAngle){
		if (newAngle - curAngle >= 1.0f){
			this->rotateTurret(1.0f);
		}
		else {
			this->rotateTurret(newAngle - curAngle);
			this->isAimed = true;
		}
	}
	else if (curAngle > newAngle){
		if (newAngle - curAngle <= -1.0f){
			this->rotateTurret(-1.0f);
		}
		else {
			this->rotateTurret(newAngle - curAngle);
			this->isAimed = true;
		}
	}
}

void Tank::turnToward(float newAngle){
	float curAngle = this->rotation;
	if (curAngle > 180){
		curAngle -= 360;
	}
	else if (curAngle < -180){
		curAngle += 360;
	}
	if (curAngle > 90 && newAngle < -90){
		newAngle += 360;
	}
	else if (curAngle < -90 && newAngle > 90){
		newAngle -= 360;
	}
	if (curAngle < newAngle){
		if (newAngle - curAngle >= 3.5f){
			this->rotate(true);
		}
	}
	else if (curAngle > newAngle){
		if (newAngle - curAngle <= -3.5f){
			this->rotate(false);
		}
	}
}

void Tank::damage(int amount){
	this->health -= amount;
}

int Tank::centerTurret(){
	if (this->turretRotation > 1.0f){
		this->rotateTurret(-1.0f);
		return -1;
	}
	else if (this->turretRotation < 1.0f){
		this->rotateTurret(1.0f);
		return 1;
	}
	return 0;
}

bool Tank::isDead(){
	return this->health <= 0;
}

float Tank::givePosX(){
	return this->posX;
}

float Tank::givePosZ(){
	return this->posZ;
}

float Tank::giveSpeedX(){
	return this->speedX;
}

float Tank::giveSpeedZ(){
	return this->speedZ;
}

float Tank::giveRotation(){
	return this->rotation;
}

float Tank::giveRotationSpeed(){
	return this->rotationSpeed;
}

float Tank::giveTurretRotation(){
	return this->turretRotation;
}

float Tank::giveTurretRotationSpeed(){
	return this->turretRotationSpeed;
}

float Tank::giveHealth(){
	return this->health;
}

void Tank::setHealth(int newHealth){
	this->health = newHealth;
}

void Tank::setShieldStrength(int newStrength){
	this->shieldStrength = newStrength;
}

int Tank::giveShieldStrength(){
	return this->shieldStrength;
}

bool Tank::canMoveTo(float newX, float newZ){
	if (newX > mapSize - 1.0f || newX < -mapSize + 1.0f || newZ > mapSize - 1.0f || newZ < -mapSize + 1.0f){
		return false;
	}
	for (int i = 0; i < tanks.size(); i++) {
		if (this != tanks[i] && distanceBetween(newX, newZ, tanks[i]->posX, tanks[i]->posZ) < 1.5f){
			return false;
		}
	}
	for (int i = 0; i < obstacles.size(); i++) {
		if (distanceBetween(newX, newZ, obstacles[i]->givePosX(), obstacles[i]->givePosZ()) < obstacles[i]->giveRadius() + 0.5f){
			return false;
		}
	}
	if (this != playerTank && distanceBetween(newX, newZ, playerTank->posX, playerTank->posZ) < 1.5f) {
		return false;
	}
	return true;
}

void Tank::drawSelf(){
	glPushMatrix();
		glTranslatef(this->posX, 0.0f, this->posZ);
		glRotatef(this->rotation, 0.0f, 1.0f, 0.0f);
		
	
		static float w, h, d, d2;
		w = this->width; //0.5
		h = this->height; //0.5
		d = this->depth;	//0.7
		d2 = this->depth / 1.4f; //0.5
		
		glColor3f(1.9f,1.0f, 0.3f);

		glBegin(GL_QUADS);
		//Front
		glNormal3f(0.0f, h, -d);

		glVertex3f(-w, h, -d);
		glVertex3f(w, h, -d);
		glVertex3f(w, 0.0, -d2);
		glVertex3f(-w, 0.0, -d2);

		//Back
		glNormal3f(0.0f, -0.5f, 0.7f);

		glVertex3f(-w, h, d2);
		glVertex3f(w, h, d2);
		glVertex3f(w, 0.0, d);
		glVertex3f(-w, 0.0, d);

		//Left
		glNormal3f(-1.0f, 0.0f, 0.0f);

		glVertex3f(-w, h, -d);
		glVertex3f(-w, 0.0, -d2);
		glVertex3f(-w, 0.0, d);
		glVertex3f(-w, h, d2);

		//Right
		glNormal3f(1.0f, 0.0f, 0.0f);

		glVertex3f(w, h, -d);
		glVertex3f(w, 0.0, -d2);
		glVertex3f(w, 0.0, d);
		glVertex3f(w, h, d2);

		//Bottom
		glNormal3f(0.0f, -1.0f, 0.0f);

		glVertex3f(-w, 0.0, -d2);
		glVertex3f(-w, 0.0, d);
		glVertex3f(w, 0.0, d);
		glVertex3f(w, 0.0, -d2);


		//Top
		glNormal3f(0.0f, 1.0f, 0.0f);

		glVertex3f(-w, h, -d);
		glVertex3f(-w, h, d2);
		glVertex3f(w, h, d2);
		glVertex3f(w, h, -d);

		glEnd();
		
		glPushMatrix();
			glTranslatef(0.0f, h*1.3f, 0.0f);
			glRotatef(this->turretRotation, 0.0f, 1.0f, 0.0f);
			glTranslatef(0.0f, 0.0f, this->recoilDistance*0.5);
			makeRectangularPrism(w*(3.0f/5.0f), 0.0f, -d/2, -w*(3.0f/5.0f), h/2, d/2);
			glTranslatef(0.0f, 0.025f, -0.6f);
			glTranslatef(0.0f, 0.0f, -0.2f + this->recoilDistance*0.8f);
			makeRectangularPrism(0.05f, -0.05f, -1.0f, -0.05f, 0.05f, 0.0f);
		glPopMatrix();
		if(this->hasShieldLeft()){
			glColor4f(0.1f, 0.1f, 1.0f, this->shieldOpacity);
			glutSolidSphere(this->width*3, 20, 20);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
	
			
	glPopMatrix();
	
}

//blok
void blok(float tebal,int ratiol,int ratiop)
{
 float i,j;
 glPushMatrix();
 for(i=0;i<ratiop;i++)
 {
  glTranslatef(-(ratiol+1)*tebal/2,0.0,0.0);
  for(j=0;j<ratiol;j++)
  {
   glTranslatef(tebal,0.0,0.0);
   glutSolidCube(tebal);
  }
  glTranslatef(-(ratiol-1)*tebal/2,0.0,tebal);
 }
 glPopMatrix();
}

//cylinder
void cylinder(float alas,float up,float height)
{
 float i;
 glPushMatrix();
 glTranslatef(1.0,0.0,-alas/8); // perpindahan dari X,Y,Z
 glutSolidCone(alas,0,32,4);
 for(i=0;i<=height;i+=alas/24)
 {
  glTranslatef(0.0,0.0,alas/24);
  glutSolidTorus(alas/4,alas-((i*(alas-up))/height),16,16);
 }
 glTranslatef(0.0,0.0,alas/4);
 glutSolidCone(height,0,20,1);
 glColor3f(0.,0.,0.);
 glPopMatrix();
}

//enemy's jeep
void Tank::jeep_musuh()
{
	static float w, h, d, d2;
		w = this->width; //0.5
		h = this->height; //0.5
		d = this->depth;	//0.7
		d2 = this->depth / 1.4f; //0.5
  //untuk menggerakan si mobil
  glTranslatef(this->posX, 0.0f, this->posZ);
  glRotatef(this->rotation, 0.0f, 1.0f, 0.0f);
  
  glScalef(0.04,0.04,0.04);
  glRotatef(90.0, 0.0, 1.0, 0.0);
  
//  glTranslatef(-w, h, -d);
  glTranslatef(0,14,0);// Peletakan mobil
 //body
 glPushMatrix();
 glColor3f(1.2,1.2,0.2);
 blok(10, 3, 2);
 glTranslatef(0, 9, 0);
 blok(10, 3, 2);
 glTranslatef(10, -10,0);
 blok(10, 5.5, 2);
 glRotatef(-35, 0, 0, 15);
 glTranslatef(0, 7,0);
 blok(10, 2, 2);
 glTranslatef(2, 4.9,-2.5);

 // warna kaca depan
 glColor3f(9,9,9);
 blok(0.5, 20, 31);
 glRotatef(180, 45, -45, 0);
 glPopMatrix();


 //roda kanan depan
 glPushMatrix();
 glColor3f(1.0,0.0,0.0);
 glTranslatef(20, -8,-7);
 cylinder(5, 5,3);

 // roda belakang kanan
 glColor3f(1.0,0.0,0.0); 
 glTranslatef(-20, 8,7);
 glTranslatef(-5, -8,-7);
 cylinder(5, 5,3);

 // roda belakang kiri
 glColor3f(1.0,0.0,0.0);
 glTranslatef(5, 8,7);
 glRotatef(180,0,180,0);
 glTranslatef(3,-8,-17);
 cylinder(5, 5,3);

 // roda depan kiri
 glColor3f(1.0,0.0,0.0); 
 glTranslatef(-3,8,17);
 glTranslatef(-22,-8,-17);
 cylinder(5, 5,3);

 // all kaca samping kanan dan kiri
 glColor3f(9,9,9);
 glRotatef(90,1,0,0);
 glTranslatef(8, 2.5,-15);



 glRotatef(90,0,1,0);
 glTranslatef(0,-0.2,7);
 blok(2, 4, 8);

 glColor3f(9,9,9);
 glRotatef(0,0,0,0);
 glTranslatef(0,19.2,0);
 blok(2, 4, 8);

 glColor3f(9,9,9);
 glRotatef(90,0,1,0);
 glTranslatef(7, 0,-8);


 //lampu
 glColor3f(9.9,9.9,0.0);
 glRotatef(90,0,1,0);
 glTranslatef(0,-3,20);
 cylinder(2, 2,3);

 //lampu
 glColor3f(9.9,9.9,0.0);
 glRotatef(0,0,0,0);
 glTranslatef(0,-12,0);
 cylinder(2, 2,3);

 //knalpot kanan
 glColor3f(1.0,1.0,1.0);
 glRotatef(0,0,0,0);
 glTranslatef(0,-0.5,-52);
 cylinder(2, 1,3);

 //knalpot kiri
 glColor3f(1.0,1.0,1.0);
 glRotatef(0,0,0,0);
 glTranslatef(0,11.5,0);
 cylinder(2, 1,3);

 // kaca belakang
 glColor3f(0.8,0.96,0.98); 
 glRotatef(90,1,0,0);
 
 // geser bawah,kedalam, geser kiri
 glTranslatef(-7,3.5,-0.5); 
 blok(2, 6.0, 7.0);
 
 
// glScalef(5.0,5.0,5.0);
// glRotatef(-90.0, 0.0, 1.0, 0.0);
 penembak();
		
 glPopMatrix();

 glFlush();
 
}

//jeep
void Tank::jeep()
{
	
 if(FPS == 1){//merupakan code untuk mengubah perspektif kamera menjadi first person, namun masih gagal
 	gluLookAt(0.0,0.0,1.0, playerTank->posX,0.0,playerTank->posZ-5, 0,0,0);
 }
  //untuk menggerakan si mobil
  glTranslatef(this->posX, 0.0f, this->posZ);
  glRotatef(this->rotation, 0.0f, 1.0f, 0.0f);
  
  glScalef(0.04,0.04,0.04);
  glRotatef(90.0, 0.0, 1.0, 0.0);
  glTranslatef(0,14,0);// Peletakan mobil
 //body
 glPushMatrix();
 glColor3f(0.2,0.2,0.2);
 blok(10, 3, 2);
 glTranslatef(0, 9, 0);
 blok(10, 3, 2);
 glTranslatef(10, -10,0);
 blok(10, 5.5, 2);
 glRotatef(-35, 0, 0, 15);
 glTranslatef(0, 7,0);
 blok(10, 2, 2);
 glTranslatef(2, 4.9,-2.5);

 // warna kaca depan
 glColor3f(9,9,9);
 blok(0.5, 20, 31);
 glRotatef(180, 45, -45, 0);
 glPopMatrix();


 //roda kanan depan
 glPushMatrix();
 glColor3f(1.0,0.0,0.0);
 glTranslatef(20, -8,-7);
 cylinder(5, 5,3);

 // roda belakang kanan
 glColor3f(1.0,0.0,0.0); 
 glTranslatef(-20, 8,7);
 glTranslatef(-5, -8,-7);
 cylinder(5, 5,3);

 // roda belakang kiri
 glColor3f(1.0,0.0,0.0);
 glTranslatef(5, 8,7);
 glRotatef(180,0,180,0);
 glTranslatef(3,-8,-17);
 cylinder(5, 5,3);

 // roda depan kiri
 glColor3f(1.0,0.0,0.0); 
 glTranslatef(-3,8,17);
 glTranslatef(-22,-8,-17);
 cylinder(5, 5,3);

 // all kaca samping kanan dan kiri
 glColor3f(9,9,9);
 glRotatef(90,1,0,0);
 glTranslatef(8, 2.5,-15);



 glRotatef(90,0,1,0);
 glTranslatef(0,-0.2,7);
 blok(2, 4, 8);

 glColor3f(9,9,9);
 glRotatef(0,0,0,0);
 glTranslatef(0,19.2,0);
 blok(2, 4, 8);

 glColor3f(9,9,9);
 glRotatef(90,0,1,0);
 glTranslatef(7, 0,-8);


 //lampu
 glColor3f(9.9,9.9,0.0);
 glRotatef(90,0,1,0);
 glTranslatef(0,-3,20);
 cylinder(2, 2,3);

 //lampu
 glColor3f(9.9,9.9,0.0);
 glRotatef(0,0,0,0);
 glTranslatef(0,-12,0);
 cylinder(2, 2,3);

 //knalpot kanan
 glColor3f(1.0,1.0,1.0);
 glRotatef(0,0,0,0);
 glTranslatef(0,-0.5,-52);
 cylinder(2, 1,3);

 //knalpot kiri
 glColor3f(1.0,1.0,1.0);
 glRotatef(0,0,0,0);
 glTranslatef(0,11.5,0);
 cylinder(2, 1,3);

 // kaca belakang
 glColor3f(0.8,0.96,0.98); 
 glRotatef(90,1,0,0);
 
 // geser bawah,kedalam, geser kiri
 glTranslatef(-7,3.5,-0.5); 
 blok(2, 6.0, 7.0);
 
 
// glScalef(5.0,5.0,5.0);
// glRotatef(-90.0, 0.0, 1.0, 0.0);
 penembak();
		
 glPopMatrix();

 glFlush();
 
}

void Tank::penembak(){
	
	static float w, h, d, d2;
		w = this->width; //0.5
		h = this->height; //0.5
		d = this->depth;	//0.7
		d2 = this->depth / 1.4f; //0.5
		
	glPushMatrix();
	glScalef(10,10,10);
	 glRotatef(90.0, 0.0, 0.0, 1.0);
	 glRotatef(-90.0, 0.0, 1.0, 0.0);
			glTranslatef(0.6f, h*2.f, -2.0f);
			glRotatef(this->turretRotation, 0.0f, 1.0f, 0.0f);
			glTranslatef(0.0f, 0.0f, this->recoilDistance*0.5);
			makeRectangularPrism(w*(3.0f/5.0f), 0.0f, -d/2, -w*(3.0f/5.0f), h/2, d/2);
			glTranslatef(0.0f, 0.025f, -0.6f);
			glTranslatef(0.0f, 0.0f, -0.2f + this->recoilDistance*0.8f);
			makeRectangularPrism(0.05f, -0.05f, -1.0f, -0.05f, 0.05f, 0.0f);
		glPopMatrix();
		if(this->hasShieldLeft()){
			glColor4f(0.4f, 0.4f, 0.0f, this->shieldOpacity);
			glutSolidSphere(this->width*3, 20, 20);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
}
Tank::~Tank(){
}
