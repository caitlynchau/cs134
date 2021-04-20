#pragma once

#include "BaseObject.h"
#include "SpriteSystem.h"

//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class Emitter : public BaseObject {
public:
	Emitter(SpriteSystem *);
	void draw();
	void start();
	void stop();
	void setLifespan(float);
	void setVelocity(ofVec3f);
	void setChildImage(ofImage);
	void setChildSize(float w, float h) { childWidth = w; childHeight = h; }
	void setImage(ofImage);
	void setRate(float);
	void update();
	void shoot(float time);
	void move(MoveDir type, int speed);

	SpriteSystem *sys;
	float rate;
	float lifespan;
	float width, height;
	float childWidth, childHeight;
	float lastSpawned;
	ofImage childImage;
	ofImage image;
	ofSoundPlayer shootSound;

	bool drawable;
	bool started;
	bool haveChildImage;
	bool haveImage;
	bool left;

	EnemyType type;

	void integrate();

	ofVec3f velocity = ofVec3f(0, 0, 0);
	float angularVelocity = 0;
	ofVec3f acceleration;
	ofVec3f forces;
	float damping = 0.99;
	float mass = 1.0;

	//ThrustForce *tForce;

};