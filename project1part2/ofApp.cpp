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
//  Caitlyn Chau 

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

	if (isEnemy) {
		pos -= 5 * heading;	
	}
	else {
		pos += 7 * heading;
	}


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

SpriteSystem::SpriteSystem() {
	sfx.load("sounds/hit.wav");
	sfx.setVolume(0.5f);
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



int SpriteSystem::removeNear(ofVec3f point, float dist) {
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;
	int count = 0;

	while (s != sprites.end()) {
		ofVec3f v = s->pos - point;
		if (v.length() < dist) {
			tmp = sprites.erase(s);
			count++;
			s = tmp;
		}
		else s++;
	}

	if (count > 0) {
		sfx.play();
	}
	return count;

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
	childWidth = 100;
	childHeight = 100;
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
	if (!started) return;

	float time = ofGetElapsedTimeMillis();
	if (started) {
		if ((time - lastSpawned) > (1000.0 / rate)) {
			shoot(time);
		}
	}

	if (isEnemy && type == LinearEnemy) {
		if (left && pos.x >= 0) {
			move(MoveLeft, 3);
		}
		else if (left && pos.x < 0) {
			left = false;
		} 
		else if (!left && pos.x > ofGetWindowWidth()) {
			left = true;
		}
		else if (!left && pos.x <= ofGetWindowWidth()) {
			move(MoveRight, 3);
		}
	}

	if (isEnemy && type == CircularEnemy) {
		move(MoveCircle, 4);
	}

	if (isEnemy && type == SineEnemy) {
		if (left && pos.x >= 0) {
			move(MoveSine, 3);
			pos.x--;
		}
		else if (left && pos.x < 0) {
			left = false;
		}
		else if (!left && pos.x > ofGetWindowWidth()) {
			left = true;
		}
		else if (!left && pos.x <= ofGetWindowWidth()) {
			move(MoveSine, 3);
			pos.x++;
		}
	}



	sys->update();
}

// Shoot one sprite
void Emitter::shoot(float time) {
	shootSound.load("sounds/shoot.wav");
	shootSound.setVolume(0.06f);
	if (!isEnemy) {
		shootSound.play();
	}

	// spawn a new sprite
	Sprite sprite;
	if (haveChildImage) {
		sprite.setImage(childImage);
	}
	sprite.velocity = velocity;
	sprite.lifespan = lifespan;
	sprite.heading = heading;
	sprite.setPosition(pos);
	sprite.birthtime = time;
	sprite.isEnemy = isEnemy;
	sys->add(sprite);
	lastSpawned = time;
	//cout << "velocity = " << sprite.velocity << endl;
}


// Move the Emitter based off a direction: Stop, Left, Right, Up, Down
void Emitter::move(MoveDir direction, int speed) {
	switch (direction) {
	case MoveStop:
		break;
	case MoveLeft:
		pos.x -= speed;
		break;
	case MoveRight:
		pos.x += speed;
		break;
	case MoveUp:
		pos.y -= speed;
		break;
	case MoveDown:
		pos.y += speed;
		break;
	case MoveCircle:
		pos.x += glm::cos(glm::radians(rot)) * speed;
		pos.y += glm::sin(glm::radians(rot)) * speed;
		rot++;
		break;
	case MoveSine:
		pos.y += glm::sin(glm::radians(rot)) * speed;
		rot++;
	default:
		break;
	}
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
	childWidth = img.getWidth();
	childHeight = img.getHeight();
}

void Emitter::setImage(ofImage img) {
	image = img;
	haveImage = true;
	width = img.getWidth();
	height = img.getHeight();
}

void Emitter::setRate(float r) {
	rate = r;
}



//--------------------------------------------------------------
void ofApp::setup() {

	ofSetVerticalSync(true);

	background.load("images/sky.png");

	// create an image for sprites being spawned by emitter
	//
	if (gunSpriteImage.load("images/blue_heart.png")) {
		spriteImageLoaded = true;
	}
	else {
		ofLogFatalError("can't load image: images/blue_heart.png");
		ofExit();
	}

	// load image for gun
	if (gunImage.load("images/penguin.png")) {
		turretImageLoaded = true;
	}
	else {
		ofLogFatalError("can't load image: images/penguin.png");
		ofExit();
	}

	// load image for enemy
	if (enemyImage.load("images/cat.png")) {
		enemyImageLoaded = true;
	}
	else {
		ofLogFatalError("can't load image: images/cat.png");
		ofExit();
	}

	// load image for enemy sprite
	if (enemySpriteImage.load("images/poop.png")) {
		enemySpriteImageLoaded = true;
	}
	else {
		ofLogFatalError("can't load image: images/poop.png");
		ofExit();
	}

	gui.setup();
	gui.add(enemyRateSlider.setup("enemy rate", 0.5, 0.1, 2));
	gui.add(lifeSlider.setup("life", 5, .1, 10));
	gui.add(gunRateSlider.setup("gun rate", 10, 1, 50));
	gui.add(velocity.setup("enemy velocity", ofVec3f(0, 0, 0), ofVec3f(-1000, 0, -1000), ofVec3f(1000, 1000, 1000)));


	// create enemy emitters
	numLinearEnemies = 1;
	float margin = 50;
	float emitterWidth = 50;
	ofPoint winsize = ofGetWindowSize();
	float spacing = (winsize.x - emitterWidth - margin * 2) / (numLinearEnemies - 1);
	float x = emitterWidth / 2 + margin;
	 

	// create an array of emitters and set their position;
	//	
	for (int i = 0; i < numLinearEnemies; i++) {
		Emitter *emit = new Emitter(new SpriteSystem());
		emit->setPosition(ofVec3f(x, 40, 0));
		emit->drawable = true;
		emit->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
		emit->setImage(enemyImage);
		emit->setChildImage(enemySpriteImage);
		emit->isEnemy = true;
		emit->type = LinearEnemy;
		emit->left = ofRandom(0, 2); // random boolean 0 or 1
		x += spacing;
		enemies.push_back(emit);
	}


	// create an enemy that rotates in a circle
	Emitter *rotatingEnemy = new Emitter(new SpriteSystem());
	rotatingEnemy->setPosition(ofVec3f(ofGetWindowWidth() / 2, ofGetWindowHeight() / 4, 0));
	rotatingEnemy->drawable = true;
	rotatingEnemy->setImage(enemyImage);
	rotatingEnemy->setChildImage(enemySpriteImage);
	rotatingEnemy->isEnemy = true;
	rotatingEnemy->type = CircularEnemy;
	enemies.push_back(rotatingEnemy);

	// create an enemy that modulates in sine function
	Emitter *sineEnemy = new Emitter(new SpriteSystem());
	sineEnemy->setPosition(ofVec3f(0, 50, 0));
	sineEnemy->drawable = true;
	sineEnemy->setImage(enemyImage);
	sineEnemy->setChildImage(enemySpriteImage);
	sineEnemy->isEnemy = true;
	sineEnemy->type = SineEnemy;
	enemies.push_back(sineEnemy);


	// create new emitter for penguin
	gun = new Emitter(new SpriteSystem());
	gun->setPosition(ofVec3f(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0));
	gun->drawable = true;
	gun->setImage(gunImage);
	gun->setChildImage(gunSpriteImage);


	bHide = false;
	gameStarted = false;
	
	gameStartTime = 0;
	score = 0;

	text.load("8bitfont.ttf", 20);

	musicPlayer.load("sounds/background-music.wav");
	musicPlayer.setVolume(0.1f);
	
	musicPlayer.play();
	musicPlayer.setLoop(true);

}

//--------------------------------------------------------------
void ofApp::update() {
	//gun->heading = heading();
	gun->setRate(gunRateSlider);
	gun->setLifespan(lifeSlider * 1000);    // convert to milliseconds 
	gun->setVelocity(ofVec3f(0, -100, 0));
	gun->update();

	// update enemies
	for (int i = 0; i < enemies.size(); i++) {
		Emitter *enemy = enemies[i];
		enemy->setRate(enemyRateSlider);
		enemy->setLifespan(lifeSlider * 1000);  // convert to milliseconds 
		enemy->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
		enemy->update();
	}

	checkCollisions();

	// game runs for 50 seconds; check to see if over and stop
	// emitters.  We are computing in milleseconds so we need
	// to multiply seconds by 1000 to convert to ms.
	//
	float t = ofGetElapsedTimeMillis();
	if ((t - gameStartTime > (50 * 1000))) {
		gameOver = true;
		gameStarted = false;
		gun->stop();

		// need to fix startGame()
		// deallocate emitters
		//for (Emitter * e : enemies) {
		//	delete e;
		//}
		//// clear the vector
		//enemies.clear();

	}

}


//--------------------------------------------------------------
void ofApp::draw() {
	background.resize(ofGetWindowWidth(), ofGetWindowHeight());
	background.draw(0, 0);

	if (gameStarted) {
		gun->heading = heading();
		gun->draw();
		//ofDrawLine(gun->pos, gun->pos + 150 * heading());

		for (int i = 0; i < enemies.size(); i++) {
			enemies[i]->draw();
		}

		// draw current score
		//
		string scoreText;
		scoreText += "Score: " + std::to_string(score);
		ofDrawBitmapString(scoreText, ofPoint(10, 20));
	}
	else if (gameOver) {
		
		text.drawString("GAME OVER. PRESS SPACE TO RESTART", 200, ofGetWindowHeight() / 2);
	}
	else {
		text.drawString("PRESS SPACE TO START", 325, ofGetWindowHeight() / 2);
	}

	if (!bHide) {
		gui.draw();
	}
}

void ofApp::checkCollisions() {
	// find the distance at which the two sprites (missles and invaders) will collide
	// detect a collision when we are within that distance.
	//
	float collisionDist = gun->childHeight / 2 + enemies[0]->childHeight / 2;

	// Loop through all the missiles, then remove any invaders that are within
	// "collisionDist" of the missiles.  the removeNear() function returns the
	// number of missiles removed.
	//
	for (int i = 0; i < gun->sys->sprites.size(); i++) {
	

		glm::vec3 shot = gun->sys->sprites[i].pos;

		//cout << "shot pos = " << shot << endl;
		for (int j = 0; j < enemies.size(); j++) {

			int spritesHit = enemies[j]->sys->removeNear(gun->sys->sprites[i].pos, collisionDist);
			score += spritesHit;
			
			if (spritesHit > 0) {
				gun->sys->remove(i); // remove the gun's sprite
				score += spritesHit;
				break;
			}
			
		}

	}
}


//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {


}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	if (gun->bSelected) {
		gun->pos.x = x;
		gun->pos.y = y;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	glm::vec3 mouse = glm::vec3(x, y, 1);

	glm::vec4 point = glm::inverse(gun->getMatrix()) * glm::vec4(mouse, 1);

	int halfOfWidth = gun->width;
	int halfOfHeight = gun->height;

	if (point.x < halfOfWidth && point.x > -halfOfWidth && point.y < halfOfHeight && point.y > -halfOfHeight)
	{
		gun->bSelected = true;
	}
	else {
		//cout << "not inside" << endl;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	gun->bSelected = false;
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
			gameStartTime = ofGetElapsedTimeMillis();
			score = 0;
			for (int i = 0; i < enemies.size(); i++) {
				enemies[i]->start();
			}
			
			//startGame();
		}
		gun->start();
		break;
	case OF_KEY_LEFT:
		if (gun->pos.x > 0) {
			gun->pos.x -= 15.0;
		}
		break;
	case OF_KEY_RIGHT:
		if (gun->pos.x < ofGetWindowWidth()) {
			gun->pos.x += 15.0;
		}
		break;
	case OF_KEY_UP:
		if (gun->pos.y > 0) {
			gun->pos.y -= 15.0;
		}
		break;
	case OF_KEY_DOWN:
		if (gun->pos.y < ofGetWindowHeight()) {
			gun->pos.y += 15.0;
		}
		break;
	case ',': // equivalent to <
		gun->rot -= 5;
		break;
	case '.': // equivalent to >
		gun->rot += 5;
		break;
	}

}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case ' ':
		if (gameStarted) gun->stop();
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