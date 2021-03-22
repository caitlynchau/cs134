#pragma once

#include "ofMain.h"
#include "ofxGui.h"

typedef enum { MoveStop, MoveLeft, MoveRight, MoveUp, MoveDown, MoveCircle, MoveSine} MoveDir;
typedef enum { LinearEnemy, CircularEnemy, SineEnemy} EnemyType;

// This is a base object that all drawable object inherit from
// It is possible this will be replaced by ofNode when we move to 3D
// 
class BaseObject {
public:
	BaseObject();
	glm::vec3 scaleVector;
	float rot;
	bool bSelected;
	glm::vec3 pos; //added
	glm::vec3 heading;
	void setPosition(glm::vec3);
	glm::mat4 getMatrix();
	bool isEnemy = false;
};

//  General Sprite class  (similar to a Particle)
//
class Sprite : public BaseObject {
public:
	Sprite();
	void draw();
	float age();
	void setImage(ofImage);
	float speed;    //   in pixels/sec
	ofVec3f velocity; // in pixels/sec
	ofImage image;
	float birthtime; // elapsed time in ms
	float lifespan;  //  time in ms
	string name;
	bool haveImage;
	float width, height;
};

//  Manages all Sprites in a system.  You can create multiple systems
//
class SpriteSystem {
public:
	SpriteSystem();
	void add(Sprite);
	void remove(int);
	int removeNear(ofVec3f point, float dist);
	void update();
	void draw();
	vector<Sprite> sprites;
	ofSoundPlayer sfx;


};


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
	ofVec3f velocity;
	float lifespan;
	bool started;
	float lastSpawned;
	ofImage childImage;
	ofImage image;
	bool drawable;
	bool haveChildImage;
	bool haveImage;
	float width, height;
	float childWidth, childHeight;
	ofSoundPlayer shootSound;
	bool left;
	EnemyType type;
};


class ofApp : public ofBaseApp {

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


	Emitter  *gun = NULL;
	vector<Emitter *> enemies;
	int numLinearEnemies;

	ofImage gunSpriteImage; // gun sprite
	ofImage gunImage; // gun
	ofImage enemyImage;  // enemy
	ofImage enemySpriteImage; 


	bool spriteImageLoaded; 
	bool turretImageLoaded;
	bool enemyImageLoaded; 
	bool enemySpriteImageLoaded;

	bool bHide;

	ofImage background;
	ofSoundPlayer musicPlayer;
	ofTrueTypeFont text;

	ofxFloatSlider enemyRateSlider; 
	ofxFloatSlider lifeSlider;
	ofxVec3Slider velocity;
	ofxFloatSlider gunRateSlider; // our character

	ofxLabel screenSize;
	ofxPanel gui;

	bool gameStarted;
	bool gameOver = false;
	int score;
	float gameStartTime;

	glm::vec3 heading() {
		glm::vec3 initialHeading = glm::vec3(0, -1, 0);
		glm::mat4 Mrot = glm::rotate(glm::mat4(1.0), glm::radians(gun->rot), glm::vec3(0, 0, 1));
		glm::vec3 h = Mrot * glm::vec4(initialHeading, 1);
		return glm::normalize(h);
	}

	void checkCollisions();

};