//
//   CS134 - Spring 2021 Midterm Starter File
//
//
//   (c) Kevin M. Smith 2021
//
//   Unauthorized copying or distribution of this source code is prohibited by law
//
#pragma once

#include "ofMain.h"
#include "ofxGui.h"



//  Shape base class
//
class Shape {
public:
	Shape() {}
	virtual void draw() {}
	virtual bool inside() { return false; }

	glm::vec3 pos;
	float rotation = 0.0;
	glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);

	// get transformation matrix for object (based on it's current pos, rotation and scale channels)
	//
	glm::mat4 getMatrix() {
		glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(pos));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));
		glm::mat4 scale = glm::scale(glm::mat4(1.0), this->scale);

		return (trans * rot * scale);
	}

	vector<glm::vec3> verts;
	bool bSelected = false;
	ofColor color = ofColor::yellow;

};

//  Imnage Class Example (that uses Shape transformations) (not used for midterm)
//
class ImageShape : public Shape {
public:
	ImageShape(ofImage image) {
		this->image = image;
	}
	ImageShape() {}
	bool inside(glm::vec3 pt) {
		int w = image.getWidth();
		int h = image.getHeight();
		glm::vec3 p = glm::inverse(getMatrix()) * glm::vec4(pt, 1);
		return (p.x > -w / 2 && p.x < w / 2 && p.y > -h / 2 && p.y < h / 2);
	}

	void draw();

	ofImage image;
};

//  TriangleShape Example (that uses Shape Transformations)
//
class TriangleShape : public Shape {
public:
	TriangleShape() {}
	TriangleShape(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		verts.push_back(p1);
		verts.push_back(p2);
		verts.push_back(p3);
	}
	bool inside(glm::vec3 p);

	void draw();

};

//  The Triangle Space Ship
//
class TriShip : public TriangleShape {
public:
	TriShip(glm::vec3 p1 = glm::vec3(-10, -10, 0),
		glm::vec3 p2 = glm::vec3(0, 20, 0),
		glm::vec3 p3 = glm::vec3(10, -10, 0),
		ofColor color = ofColor::yellow) {
		verts.push_back(p1);
		verts.push_back(p2);
		verts.push_back(p3);
		this->color = color;
	}

	void setColor(ofColor);

	//  Integrator Function;
	//
	void integrate();

	// Physics data goes here  (for integrate() );
	//
	ofVec3f velocity = ofVec3f(0, 0, 0); 
	float angularVelocity = 0;
	ofVec3f acceleration;
	ofVec3f forces;
	float damping = 0.99;
	float mass = 1.0;
	

};


// Pure virtual Function Class - must be subclassed to create new forces
class Force {
protected:
public:
	bool applyOnce = false;
	bool applied = false;
	virtual void updateForce(TriShip *) = 0;
};

class ThrustForce : public Force {
	float magnitude;
	ofVec3f f;
public:
	//ThrustForce() { magnitude = 0; }
	ThrustForce(float magnitude);
	void updateForce(TriShip *tri);
	void set(ofVec3f f);
	void set(float magnitude);
	//void setForce(ofVec3f f);
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


	// UI control data
	//
	bool bFullscreen = false;
	bool bCtrlKeyDown = false;
	bool bDrag = false;
	bool bStartSim = false;


	// GUI
	//
	bool bHide = true;
	ofxFloatSlider thrustSlider;
	ofxFloatSlider runnerThrustSlider;
	ofxFloatSlider distThreshold;
	ofxToggle drawHeading;
	ofxFloatSlider triScale;

	ofxPanel gui;
	glm::vec3 mouseLast;

	// App-specific data
	//
	ofVec3f f;
	ThrustForce *thrustForce;
	ThrustForce *runnerThrustForce; // attack & enemy thrust force
	TriShip tri;
	vector<TriShip> attackers;
	vector<TriShip> runners;
	
	glm::vec3 heading() {
		glm::vec3 initialHeading = glm::vec3(0, -1, 0);
		glm::mat4 Mrot = glm::rotate(glm::mat4(1.0), glm::radians(tri.rotation), glm::vec3(0, 0, 1));
		glm::vec3 h = Mrot * glm::vec4(initialHeading, 1);
		return glm::normalize(h);
	}

	// app functions
	void resetScreen();
	void addAttacker();
	void addRunner();
	void attack();
	void run();
};
