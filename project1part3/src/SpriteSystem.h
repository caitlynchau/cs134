#pragma once
#include "ofMain.h"
#include "Sprite.h"

class SpriteSystem {
public:
	SpriteSystem();

	// functions
	void add(Sprite);
	void remove(int);
	int removeNear(ofVec3f point, float dist);
	void update();
	void draw();

	// variables
	vector<Sprite> sprites;
	//ofSoundPlayer sfx;

};

