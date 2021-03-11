#include "ofApp.h"

BaseObject::BaseObject() {
	pos = glm::vec3(0, 0, 0);
	scaleVector = glm::vec3(1, 1, 1);
	rot = 0;
	heading = glm::vec3(0, -1, 0);
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
	name = "Snowflake";
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

	pos += 5 * heading;
	// draw image centered and add in translation amount
	//
	if (haveImage) {
		image.draw(-width / 2.0 + pos.x, -height / 2.0 + pos.y);
		//cout << "have image" << endl;
	}
	else
	{
		// in case no image is supplied, draw something.
		//
		ofSetColor(255, 0, 0);
		ofDrawRectangle(-width / 2.0 + pos.x, -height / 2.0, width, height + pos.y);
	}

	//ofPopMatrix();
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
	//cout << "sprite system update: size = " << sprites.size() << endl;
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
		//sprites[i].pos += sprites[i].velocity / ofGetFrameRate();
	}
}

//  Render all the sprites
//
void SpriteSystem::draw() {
	//cout << "sprite system draw called" << endl;
	//cout << "system size = " << sprites.size() << endl;
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
	width = 100;
	height = 100;
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
	//cout << "drew emitter" << endl;
	ofPopMatrix();

	sys->draw();
	//cout << "sys->draw() called" << endl;
}

//  Update the Emitter. If it has been started, spawn new sprites with
//  initial velocity, lifespan, birthtime.
//
void Emitter::update() {
	if (!started) return;

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
	shootSound.load("sounds/hit-sound.wav");
	shootSound.setVolume(0.2);
	shootSound.play();
	// spawn a new sprite
	Sprite sprite;
	if (haveChildImage) {
		sprite.setImage(childImage);
		//cout << "have child image" << endl;
	}
	sprite.velocity = velocity;
	sprite.lifespan = lifespan;
	sprite.heading = heading;
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



//--------------------------------------------------------------
void ofApp::setup() {

	ofSetVerticalSync(true);

	background.load("images/cloud.png");

	// create an image for sprites being spawned by emitter
	//
	if (defaultImage.load("images/pink-snowflake.png")) {
		imageLoaded = true;
	}
	else {
		ofLogFatalError("can't load image: images/snowflake.png");
		ofExit();
	}

	// load image for turret
	if (turretImage.load("images/penguin.png")) {
		turretImageLoaded = true;
	}
	else {
		ofLogFatalError("can't load image: images/penguin.png");
		ofExit();
	}


	// create new emitter
	turret = new Emitter(new SpriteSystem());
	turret->setPosition(ofVec3f(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0));
	turret->drawable = true;
	turret->setImage(turretImage);
	turret->setChildImage(defaultImage);

	gui.setup();
	gui.add(rate.setup("rate", 10, 1, 100));
	gui.add(life.setup("life", 5, .1, 10));
	gui.add(velocity.setup("velocity", ofVec3f(0, -200, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));

	bHide = false;
	gameStarted = false;

	text.load("8bitfont.ttf", 20);

	musicPlayer.load("sounds/background-music.wav");
	musicPlayer.setVolume(0.2);
	musicPlayer.play();
	musicPlayer.setLoop(true);

}

//--------------------------------------------------------------
void ofApp::update() {
	//turret->heading = heading();
	turret->setRate(rate);
	turret->setLifespan(life * 1000);    // convert to milliseconds
	turret->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
	turret->update();
}


//--------------------------------------------------------------
void ofApp::draw() {
	background.resize(ofGetWindowWidth(), ofGetWindowHeight());
	background.draw(0, 0);

	if (gameStarted) {
		turret->heading = heading();
		turret->draw();
		//ofDrawLine(turret->pos, turret->pos + 150 * heading());
	}
	else {
		text.drawString("PRESS SPACE TO START", 325, ofGetWindowHeight() / 2);
	}

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

	if (turret->bSelected) {
		turret->pos.x = x;
		turret->pos.y = y;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	glm::vec3 mouse = glm::vec3(x, y, 1);

	glm::vec4 point = glm::inverse(turret->getMatrix()) * glm::vec4(mouse, 1);

	int halfOfWidth = turret->width;
	int halfOfHeight = turret->height;

	if (point.x < halfOfWidth && point.x > -halfOfWidth && point.y < halfOfHeight && point.y > -halfOfHeight)
	{
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
		if (!gameStarted) {
			gameStarted = true;
		}
		turret->start();
		break;
	case OF_KEY_LEFT:
		if (turret->pos.x > 0) {
			turret->pos.x -= 15.0;
		}
		break;
	case OF_KEY_RIGHT:
		if (turret->pos.x < ofGetWindowWidth()) {
			turret->pos.x += 15.0;
		}
		break;
	case OF_KEY_UP:
		if (turret->pos.y > 0) {
			turret->pos.y -= 15.0;
		}
		break;
	case OF_KEY_DOWN:
		if (turret->pos.y < ofGetWindowHeight()) {
			turret->pos.y += 15.0;
		}
		break;
	case ',': // equivalent to <
		turret->rot -= 5;
		break;
	case '.': // equivalent to >
		turret->rot += 5;
		break;
	}

}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case ' ':
		if (gameStarted) turret->stop();
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
