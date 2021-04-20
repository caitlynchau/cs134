#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Emitter.h"
#include "ParticleEmitter.h"

class Force {
protected:
public:
	bool applyOnce;
	bool applied = false;
	virtual void updateForce(Emitter *) = 0;
};

class ThrustForce : public Force {
	float magnitude;
	ofVec3f f;
public:
	ThrustForce(float magnitude);
	void updateForce(Emitter *emitter);
	void set(ofVec3f f);
	void set(float magnitude);
};

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	Emitter *penguin = NULL;
	vector<Emitter *> enemies;
	int numLinearEnemies;

	ofImage gunSpriteImage; // penguin sprite
	ofImage gunImage; // penguin
	ofImage enemyImage;  // enemy
	ofImage enemySpriteImage;


	bool spriteImageLoaded;
	bool turretImageLoaded;
	bool enemyImageLoaded;
	bool enemySpriteImageLoaded;

	bool bHide;

	ofImage background;

	ofSoundPlayer musicPlayer;
	ofSoundPlayer sfx;

	ofTrueTypeFont openingText;
	ofTrueTypeFont scoreText;
	ofTrueTypeFont livesText;

	ofxFloatSlider enemyRateSlider;
	ofxFloatSlider lifeSlider;
	ofxVec3Slider velocity;
	ofxFloatSlider gunRateSlider; // our character
	ofxFloatSlider thrustSlider;

	ofxLabel screenSize;
	ofxPanel gui;

	bool gameStarted;
	bool gameOver = false;
	int score;
	int penguinLives;
	float gameStartTime;

	// forces
	ThrustForce *thrustForce;

	glm::vec3 heading() {
		glm::vec3 initialHeading = glm::vec3(0, -1, 0);
		glm::mat4 Mrot = glm::rotate(glm::mat4(1.0), glm::radians(penguin->rot), glm::vec3(0, 0, 1));
		glm::vec3 h = Mrot * glm::vec4(initialHeading, 1);
		return glm::normalize(h);
	}

	// collisions & explosions
	void checkCollisions();
	void explode(glm::vec3 p);
	ParticleEmitter *pEmitter = NULL;
};
