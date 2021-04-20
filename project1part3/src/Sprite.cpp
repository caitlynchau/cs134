#include "Sprite.h"
#include "ofApp.h"

//
// Basic Sprite Object
//
Sprite::Sprite() {
	speed = 0;
	velocity = ofVec3f(0, 0, 0);
	lifespan = -1;      // lifespan of -1 => immortal 
	birthtime = 0;
	bSelected = false;
	haveImage = false;
	name = "sprite";
	width = 50;
	height = 50;
}

// Return a sprite's age in milliseconds
//
float Sprite::age() {
	return (ofGetElapsedTimeMillis() - birthtime);
}

//  Set an image for the sprite. If you don't set one, a rectangle
//  gets drawn.
//
void Sprite::setImage(ofImage img) {
	image = img;
	haveImage = true;
	width = image.getWidth();
	height = image.getHeight();

}

//  Render the sprite
//
void Sprite::draw() {

	ofSetColor(255, 255, 255, 255);

	if (isEnemy) {
		pos -= 5 * heading;
	} else {
		pos += 7 * heading;
	}


	// draw image centered and add in translation amount
	//
	if (haveImage) {
		image.draw(-width / 2.0 + pos.x, -height / 2.0 + pos.y);
		//cout << "have image" << endl;
	} else {
		// in case no image is supplied, draw something.
		// 
		ofSetColor(255, 0, 0);
		ofDrawRectangle(-width / 2.0 + pos.x, -height / 2.0, width, height + pos.y);
	}
}
