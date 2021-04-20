#include "ofApp.h"

//--------------------------------------------------------------
ThrustForce::ThrustForce(float magnitude) {
	this->magnitude = magnitude;
}

//--------------------------------------------------------------
void ThrustForce::updateForce(Emitter *e) {
	e->forces = f;
}

//--------------------------------------------------------------
void ThrustForce::set(ofVec3f f) {
	this->f = f;
}

//--------------------------------------------------------------
void ThrustForce::set(float magnitude) {
	this->magnitude = magnitude;
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

	// load image for penguin
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
	//gui.add(enemyRateSlider.setup("enemy rate", 0.5, 0.1, 2));
	gui.add(lifeSlider.setup("life", 5, .1, 10));
	gui.add(gunRateSlider.setup("penguin rate", 10, 1, 50));
	gui.add(velocity.setup("enemy velocity", ofVec3f(0, 0, 0), ofVec3f(-1000, 0, -1000), ofVec3f(1000, 1000, 1000)));
	gui.add(thrustSlider.setup("Thrust", 500, 10, 2000));

	// create enemy emitters
	numLinearEnemies = 2;
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
		//emit->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
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
	penguin = new Emitter(new SpriteSystem());
	penguin->setPosition(ofVec3f(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0));
	penguin->drawable = true;
	penguin->setImage(gunImage);
	penguin->setChildImage(gunSpriteImage);
	thrustForce = new ThrustForce(thrustSlider);


	// explosions
	pEmitter = new ParticleEmitter(new ParticleSystem());
	pEmitter->setOneShot(true);
	pEmitter->setEmitterType(RadialEmitter);
	pEmitter->setLifespan(2);
	pEmitter->setVelocity(ofVec3f(50, 50, 0));
	pEmitter->setParticleRadius(2);
	pEmitter->setGroupSize(15);

	bHide = false;
	gameStarted = false;

	gameStartTime = 0;
	score = 0;
	penguinLives = 100;

	openingText.load("8bitfont.ttf", 20);
	scoreText.load("8bitfont.ttf", 20);

	
	musicPlayer.load("sounds/background-music.wav");
	musicPlayer.setMultiPlay(true);
	musicPlayer.setVolume(0.1f);

	sfx.load("sounds/hit.wav");
	sfx.setMultiPlay(true);
	sfx.setVolume(0.5f);

	musicPlayer.play();
	musicPlayer.setLoop(true);

}

//--------------------------------------------------------------
void ofApp::update() {
	//penguin->heading = heading();
	penguin->setRate(gunRateSlider);
	penguin->setLifespan(lifeSlider * 1000);    // convert to milliseconds 
	//penguin->setVelocity(ofVec3f(0, -100, 0));
	penguin->update();
	penguin->integrate();

	// update enemies
	for (int i = 0; i < enemies.size(); i++) {
		Emitter *enemy = enemies[i];
		enemy->setRate(ofRandom(0.1, 2));
		enemy->setLifespan(lifeSlider * 1000);  // convert to milliseconds 
		//enemy->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
		enemy->update();
	}

	// update particle emitter
	pEmitter->update();

	checkCollisions();

	// game runs for 120 seconds OR while lives > 0
	float t = ofGetElapsedTimeMillis();
	if ((t - gameStartTime > (120 * 1000)) || penguinLives <= 0) {
		gameOver = true;
		gameStarted = false;
		penguin->stop();
	}

}


//--------------------------------------------------------------
void ofApp::draw() {
	background.resize(ofGetWindowWidth(), ofGetWindowHeight());
	background.draw(0, 0);

	pEmitter->draw();

	if (gameStarted) {
		penguin->heading = heading();
		penguin->draw();
		//ofDrawLine(penguin->pos, penguin->pos + 150 * heading());

		for (int i = 0; i < enemies.size(); i++) {
			enemies[i]->draw();
		}

		// draw current score text
		string scoreStr;
		scoreStr += "Score: " + std::to_string(score);
		scoreText.drawString(scoreStr, ofGetWindowWidth() / 2, 20);

		// draw lives text
		string livesStr;
		livesStr += "HP: " + std::to_string(penguinLives);
		scoreText.drawString(livesStr, ofGetWindowWidth() / 2, 40);
		
	}
	else if (gameOver) {
		string gameOverStr;
		gameOverStr += "GAME OVER. SCORE: " + std::to_string(score);
		openingText.drawString(gameOverStr, 360, ofGetWindowHeight() / 2);
	}
	else {
		openingText.drawString("PRESS SPACE TO START", 325, ofGetWindowHeight() / 2);
	}

	if (!bHide) {
		gui.draw();
	}
}

void ofApp::explode(glm::vec3 p) {
	pEmitter->setPosition(p);
	pEmitter->sys->reset();
	pEmitter->start();
}

void ofApp::checkCollisions() {
	// find the distance at which the two sprites (missles and invaders) will collide
	// detect a collision when we are within that distance.
	//
	float collisionDist = penguin->childHeight / 2 + enemies[0]->childHeight / 2;

	// Loop through all the missiles, then remove any invaders that are within
	// "collisionDist" of the missiles.  the removeNear() function returns the
	// number of missiles removed.
	//
	for (int i = 0; i < penguin->sys->sprites.size(); i++) {

		glm::vec3 shot = penguin->sys->sprites[i].pos;

		//cout << "shot pos = " << shot << endl;
		for (int j = 0; j < enemies.size(); j++) {

			int spritesHit = enemies[j]->sys->removeNear(shot, collisionDist);

			score += spritesHit;

			if (spritesHit > 0) {
				explode(shot);
				sfx.play();
				penguin->sys->remove(i); // remove the penguin's sprit
				break;
			}
		}
	}

	// are any poops near penguin
	for (int i = 0; i < enemies.size(); i++) {

		glm::vec3 shot = penguin->pos;

		//cout << "shot pos = " << shot << endl;
		for (int j = 0; j < enemies[i]->sys->sprites.size(); j++) {

			int spritesHit = enemies[i]->sys->removeNear(shot, collisionDist);

			if (spritesHit > 0) {			
				penguinLives -= 7;
				break;
			}

		}

	}
}


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {


}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	if (penguin->bSelected) {
		penguin->pos.x = x;
		penguin->pos.y = y;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	glm::vec3 mouse = glm::vec3(x, y, 1);

	glm::vec4 point = glm::inverse(penguin->getMatrix()) * glm::vec4(mouse, 1);

	int halfOfWidth = penguin->width;
	int halfOfHeight = penguin->height;

	if (point.x < halfOfWidth && point.x > -halfOfWidth && point.y < halfOfHeight && point.y > -halfOfHeight)
	{
		penguin->bSelected = true;
	}
	else {
		//cout << "not inside" << endl;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	penguin->bSelected = false;
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
			penguinLives = 100;
			for (int i = 0; i < enemies.size(); i++) {
				enemies[i]->start();
			}

		}
		penguin->start();
		break;
	case OF_KEY_LEFT:
		penguin->angularVelocity -= 10;
		break;
	case OF_KEY_RIGHT:
		penguin->angularVelocity += 10;
		break;
	case OF_KEY_UP:
		if (penguin->pos.x > 0 && penguin->pos.x < ofGetWindowWidth() &&
			penguin->pos.y > 0 && penguin->pos.y < ofGetWindowHeight()) {
			//penguin->pos.y -= 15.0;
			thrustForce->set(ofVec3f(heading().x * thrustSlider, heading().y * thrustSlider, heading().z * thrustSlider));
			thrustForce->updateForce(penguin);
			penguin->velocity -= 2 * heading();
		}
		break;
	case OF_KEY_DOWN:
		if (penguin->pos.x > 0 && penguin->pos.x < ofGetWindowWidth() &&
			penguin->pos.y > 0 && penguin->pos.y < ofGetWindowHeight()) {
			thrustForce->set(ofVec3f(-heading().x * thrustSlider, -heading().y * thrustSlider, -heading().z * thrustSlider));
			thrustForce->updateForce(penguin);
			penguin->velocity += 2 * heading();
		}
		break;
	}
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case ' ':
		if (gameStarted) penguin->stop();
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