#pragma once
#include "BaseObject.h"

class Sprite : public BaseObject {
public:
	Sprite();
	
	// functions
	void draw();
	float age();
	void setImage(ofImage);

	// variables
	float speed;    //   in pixels/sec
	ofVec3f velocity; // in pixels/sec
	ofImage image;
	float birthtime; // elapsed time in ms
	float lifespan;  //  time in ms
	string name;
	bool haveImage;
	float width, height;
};

