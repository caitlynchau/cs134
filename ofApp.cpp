#include "ofApp.h"
//----------------------------------------------------------------------------------
//
// This example code demonstrates the use of an "Emitter" class to emit Sprites
// and set them in motion. The concept of an "Emitter" is taken from particle
// systems (which we will cover next week).
//
// The Sprite class has also been upgraded to include lifespan, velocity and age
// members.   The emitter can control rate of emission and the current velocity
// of the particles. In this example, there is no acceleration or physics, the
// sprites just move simple frame-based animation.
//
// The code shows a way to attach images to the sprites and optional the
// emitter (which is a point source) can also have an image.  If there are
// no images attached, a placeholder rectangle is drawn.
// Emitters  can be placed anywhere in the window. In this example, you can drag
// it around with the mouse.
//
// OF has an add-in called ofxGUI which is a very simple UI that is useful for
// creating sliders, buttons and fields. It is not recommended for commercial 
// game development, but it is useful for testing.  The "h" key will hide the GUI
// 
// If you want to run this example, you need to use the ofxGUI add-in in your
// setup.
//
//
//  Kevin M. Smith - CS 134 SJSU

BaseObject::BaseObject() {
	pos = glm::vec3(0, 0, 0);
	scaleVector = glm::vec3(1, 1, 1);
	rot = 0;
}

void BaseObject::setPosition(glm::vec3 pos) {
	this->pos = pos;
}


glm::mat4 BaseObject::getMatrix() {
	glm::mat4 transMatrix = glm::translate(glm::mat4(1.0), glm::vec3(pos));
	glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0), glm::radians(rot), glm::vec3(0, 0, 1));
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(this->scaleVector));

	return (transMatrix * rotMatrix * scaleMatrix);
}


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
	name = "UnamedSprite";
	width = 60;
	height = 80;
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
	ofPushMatrix();
	ofMultMatrix(getMatrix());
	// draw image centered and add in translation amount
	//
	if (haveImage) {
		image.draw(-width / 2.0, -height / 2.0);
	}
	else {
		// in case no image is supplied, draw something.
		// 
		ofSetColor(255, 0, 0);
		ofDrawRectangle(-width / 2.0, -height / 2.0, width, height);
	}
	ofPopMatrix();
}

//  Add a Sprite to the Sprite System
//
void SpriteSystem::add(Sprite s) {
	sprites.push_back(s);
}

// Remove a sprite from the sprite system. Note that this function is not currently
// used. The typical case is that sprites automatically get removed when the reach
// their lifespan.
//
void SpriteSystem::remove(int i) {
	sprites.erase(sprites.begin() + i);
}


//  Update the SpriteSystem by checking which sprites have exceeded their
//  lifespan (and deleting).  Also the sprite is moved to it's next
//  location based on velocity and direction.
//
void SpriteSystem::update() {

	if (sprites.size() == 0) return;
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sprites.end()) {
		if (s->lifespan != -1 && s->age() > s->lifespan) {
			//			cout << "deleting sprite: " << s->name << endl;
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}

	//  Move sprite
	//
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].pos += sprites[i].velocity / ofGetFrameRate();
	}
}

//  Render all the sprites
//
void SpriteSystem::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw();
	}
	
}

//  Create a new Emitter - needs a SpriteSystem
//
Emitter::Emitter(SpriteSystem *spriteSys) {
	sys = spriteSys;
	lifespan = 3000;    // milliseconds
	started = false;

	lastSpawned = 0;
	rate = 1;    // sprites/sec
	haveChildImage = false;
	haveImage = false;
	velocity = ofVec3f(100, 100, 0);
	drawable = true;
	width = 50;
	height = 50;
}

//  Draw the Emitter if it is drawable. In many cases you would want a hidden emitter
//
//
void Emitter::draw() {

	ofPushMatrix();
	ofMultMatrix(getMatrix());

	if (drawable) {
		if (haveImage) {
			image.draw(-image.getWidth() / 2.0, -image.getHeight() / 2.0);
		}
		else {
			ofSetColor(0, 0, 200);
			ofDrawRectangle(-width / 2.0, -height / 2.0, width, height);
		}
	}
	ofPopMatrix();

	sys->draw();

	
}

//  Update the Emitter. If it has been started, spawn new sprites with
//  initial velocity, lifespan, birthtime.
//
void Emitter::update() {

	float time = ofGetElapsedTimeMillis();
	if (started) {
		if ((time - lastSpawned) > (1000.0 / rate)) {
			shoot(time);
		}
	}
	sys->update();
}

// Shoot one sprite
void Emitter::shoot(float time) {
	// spawn a new sprite
	Sprite sprite;
	if (haveChildImage) sprite.setImage(childImage);
	sprite.velocity = velocity;
	sprite.lifespan = lifespan;
	sprite.setPosition(pos);
	sprite.birthtime = time;
	sys->add(sprite);
	lastSpawned = time;
}

// Start/Stop the emitter.
//
void Emitter::start() {
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();
}

void Emitter::stop() {
	started = false;
}


void Emitter::setLifespan(float life) {
	lifespan = life;
}

void Emitter::setVelocity(ofVec3f v) {
	velocity = v;
}

void Emitter::setChildImage(ofImage img) {
	childImage = img;
	haveChildImage = true;
}

void Emitter::setImage(ofImage img) {
	image = img;
	haveImage = true;
}

void Emitter::setRate(float r) {
	rate = r;
}

bool Emitter::inside(glm::vec3 mouseTransformed, glm::vec3 pos) {
	glm::vec3 p1 = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0);
	glm::vec3 p2 = glm::vec3(ofGetWindowWidth() / 2.0 + image.getWidth(), ofGetWindowHeight() / 2.0, 0);
	glm::vec3 p3 = glm::vec3(ofGetWindowWidth() / 2.0 + image.getWidth(), ofGetWindowHeight() / 2.0 + image.getHeight(), 0);
	glm::vec3 p4 = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0 + image.getHeight(), 0);

	glm::vec3 v1 = glm::normalize(p1 - mouseTransformed);
	glm::vec3 v2 = glm::normalize(p2 - mouseTransformed);
	glm::vec3 v3 = glm::normalize(p3 - mouseTransformed);
	glm::vec3 v4 = glm::normalize(p4 - mouseTransformed);

	float a1 = glm::orientedAngle(v1, v2, glm::vec3(0, 0, 1));
	float a2 = glm::orientedAngle(v2, v3, glm::vec3(0, 0, 1));
	float a3 = glm::orientedAngle(v3, v4, glm::vec3(0, 0, 1));
	float a4 = glm::orientedAngle(v4, v1, glm::vec3(0, 0, 1));

	if (a1 < 0 && a2 < 0 && a3 < 0 && a4 < 0) return true;
	else return false;

}


//--------------------------------------------------------------
void ofApp::setup() {

	ofSetVerticalSync(true);

	// create an image for sprites being spawned by emitter
	//
	if (defaultImage.load("images/bear.png")) {
		imageLoaded = true;
	}
	else {
		ofLogFatalError("can't load image: images/ship.png");
		ofExit();
	}



	// create an array of emitters and set their position;
	//


	turret = new Emitter(new SpriteSystem());
//	turret = new Emitter();

	turret->setPosition(ofVec3f(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0));
	turret->drawable = true;
	turret->setChildImage(defaultImage);


	gui.setup();
	gui.add(rate.setup("rate", 10, 1, 100));
	gui.add(life.setup("life", 5, .1, 10));
	gui.add(velocity.setup("velocity", ofVec3f(0, -200, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	bHide = false;

}

//--------------------------------------------------------------
void ofApp::update() {
	turret->setRate(rate);
	turret->setLifespan(life * 1000);    // convert to milliseconds 
	turret->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
	turret->update();

}


//--------------------------------------------------------------
void ofApp::draw() {

	turret->draw();
	ofDrawLine(turret->pos, turret->pos + 150 * heading());


	if (!bHide) {
		gui.draw();
	}
}


//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	//	cout << "mouse( " << x << "," << y << ")" << endl;
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	glm::vec3 mouse = glm::vec3(x, y, 1);

	if (!turret->bSelected) return;
	
	//glm::vec3 delta = mouse - mouseLast;
	
	turret->setPosition(glm::vec3(x, y, 0));

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	/*
	check bounds
	if ((x > 0 && x < ofGetWindowWidth() - turret->width) && (y > 0 && y < ofGetWindowHeight() - turret->height)) {
		trans.x = x;
		trans.y = y;

	}
	*/

	glm::vec3 mouse = glm::vec3(x, y, 1);

	if (turret->inside(glm::inverse(turret->getMatrix()) * glm::vec4(mouse, 1), turret->pos)) {
		turret->bSelected = true;
	}
	else {
		cout << "not inside" << endl;
	}

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	turret->bSelected = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

void ofApp::keyPressed(int key) {
	switch (key) {
	case 'H':
	case 'h':
		bHide = !bHide;
		break;
	case ' ':
		turret->start();
		break;
	case OF_KEY_LEFT:
		turret->pos.x -= 10.0;
		cout << turret->pos << endl;
		break;
	case OF_KEY_RIGHT:
		turret->pos.x += 10.0;
		cout << turret->pos << endl;
		break;
	case OF_KEY_UP:
		turret->pos += 10.0 * heading();
		cout << turret->pos << endl;
		break;
	case OF_KEY_DOWN:
		turret->pos -= 10.0 * heading();
		cout << turret->pos << endl;
		break;
	case ',': // equivalent to <
		turret->rot -= 3;
		break;
	case '.': // equivalent to >
		turret->rot += 3;
		break;
	}

}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case ' ':
		turret->stop();
		break;
	case OF_KEY_LEFT:
	case OF_KEY_RIGHT:
	case OF_KEY_UP:
	case OF_KEY_DOWN:
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

