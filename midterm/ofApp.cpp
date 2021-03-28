//
//   CS134 - Spring 2021 Midterm Starter File
//
//
//   (c) Kevin M. Smith 2021
//
//   Unauthorized copying or distribution of this source code is prohibited by law
//
//

#include "ofApp.h"

void TriangleShape::draw() {


	if (bSelected)
		ofSetColor(ofColor::white);
	else
		ofSetColor(color);


	// tell OpenGL to transform all points using the "current transformation matrix" (CTM);
	//
	ofPushMatrix();
	ofMultMatrix(getMatrix());   // get the current transform for the object.

	// now everything I draw will be transformed by the matrix "T"
	ofDrawTriangle(verts[0], verts[1], verts[2]);

	ofPopMatrix();


}


// inside() test method - check to see if point p is inside triangle.
// how could you improve the design of this method ?
//
bool TriangleShape::inside(glm::vec3 p) {
	glm::vec3 v1 = glm::normalize(verts[0] - p);
	glm::vec3 v2 = glm::normalize(verts[1] - p);
	glm::vec3 v3 = glm::normalize(verts[2] - p);
	float a1 = glm::orientedAngle(v1, v2, glm::vec3(0, 0, 1));
	float a2 = glm::orientedAngle(v2, v3, glm::vec3(0, 0, 1));
	float a3 = glm::orientedAngle(v3, v1, glm::vec3(0, 0, 1));
	if (a1 < 0 && a2 < 0 && a3 < 0) return true;
	else return false;
}

void ImageShape::draw() {


	ofPushMatrix();
	ofMultMatrix(getMatrix());  // get the current transform for the object


	image.draw(-image.getWidth() / 2.0, -image.getHeight() / 2.0);

	ofPopMatrix();

}


void TriShip::setColor(ofColor color) {
	this->color = color;
}

// Integrator for simple trajectory physics
//
void TriShip::integrate() {

	// interval for this step 
	//
	float dt = 1.0 / 60.0;

	// update position based on velocity
	//
	pos += (velocity * dt);
	rotation += angularVelocity * dt;

	// update acceleration with accumulated paritcles forces
	// remember :  (f = ma) OR (a = 1/m * f)
	//
	ofVec3f accel = acceleration;    // start with any acceleration already on the particle

	accel += (forces * (1.0 / mass));
	cout << "integrate() = " << forces << endl;
	velocity += accel * dt;

	cout << "integrate() velocity = " << velocity << endl;

	// add a little damping for good measure
	//
	velocity *= damping;
	angularVelocity *= damping;

	// clear forces on particle (they get re-added each step)
	//
	forces.set(0, 0, 0);
}


//--------------------------------------------------------------
ThrustForce::ThrustForce(float magnitude) {
	this->magnitude = magnitude;
}


//--------------------------------------------------------------
void ThrustForce::updateForce(TriShip *ship) {
	cout << "before updateForce() = " << ship->forces << endl;
	//ship->forces += magnitude;
	//ship->forces.x = magnitude;
	//ship->forces.y += magnitude;
	ship->forces = f;
	cout << "after updateForce() = " << ship->forces << endl;
}


void ThrustForce::set(ofVec3f f) {
	this->f = f;
}



void ThrustForce::set(float magnitude) {
	this->magnitude = magnitude;
}


void ofApp::attack() {
	for (int i = 0; i < attackers.size(); i++) {
		ofVec3f distance = tri.pos - attackers[i].pos;
		if (distance.length() <= distThreshold) {
			attackers[i].rotation = glm::degrees(atan2(distance.x, distance.y));
			attackers[i].velocity += distance;
			attackers[i].integrate();
		}
	}
}


void ofApp::run() {
	for (int i = 0; i < runners.size(); i++) {
		ofVec3f distance = tri.pos - runners[i].pos;
		if (distance.length() <= distThreshold) { 
			runners[i].rotation = tri.rotation;
			runners[i].velocity -= 15 * heading();
			runners[i].integrate();
		}
	}
}


//--------------------------------------------------------------
void ofApp::resetScreen() {
	tri.forces = ofVec3f(0, 0, 0);
	tri.velocity = ofVec3f(0, 0, 0);
	tri.angularVelocity = 0;
	tri.pos = glm::vec3(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, 1);
	runners.clear();
	attackers.clear();
}


//--------------------------------------------------------------
void ofApp::addAttacker() {
	TriShip attacker;
	float width = ofRandom(0, ofGetWindowWidth());
	float height = ofRandom(0, ofGetWindowHeight());

	attacker.pos = glm::vec3(width, height, 1);
	attacker.scale = glm::vec3(1, 1, 1);
	attacker.color = ofColor::red;

	attackers.push_back(attacker);
}


//--------------------------------------------------------------
void ofApp::addRunner() {
	TriShip runner;
	float width = ofRandom(0, ofGetWindowWidth());
	float height = ofRandom(0, ofGetWindowHeight());

	runner.pos = glm::vec3(width, height, 1);
	runner.scale = glm::vec3(1, 1, 1);
	runner.color = ofColor::green;

	runners.push_back(runner);
}


//--------------------------------------------------------------
void ofApp::setup() {

	ofSetBackgroundColor((ofColor::black));
	//tri = new TriShip();
	tri.pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 1);
	tri.scale = glm::vec3(3.0, 3.0, 3.0);


	// setup GUI
	//
	gui.setup();
	gui.add(thrustSlider.setup("Thrust", 500, 10, 5000));
	gui.add(runnerThrustSlider.setup("Runner/Attacker Thrust", 600, 400, 1000));
	gui.add(distThreshold.setup("Distance Threshold", 150, 20, 1000));
	gui.add(drawHeading.setup("Draw heading", false));
	gui.add(triScale.setup("Triangle Scale", 2.0, 1.0, 3));

	thrustForce = new ThrustForce(thrustSlider);	// pass value from thrust slider
	runnerThrustForce = new ThrustForce(runnerThrustSlider); // pass value from runnerThrust slider

}


//--------------------------------------------------------------
void ofApp::update() {
	float s = triScale;  // get from slider; cast float
	tri.scale = glm::vec3(s, s, s);


	// integrator function(s) called dhere
	//
	if (bStartSim) {

		// update forces
		cout << "slider value = " << thrustSlider << endl;
		//thrustForce->set(thrustSlider);
		//thrustForce->updateForce(&tri);

		tri.integrate();
		run();
		attack();

		

		for (int i = 0; i < runners.size(); i++) {
			runnerThrustForce->set(runnerThrustSlider);
			runnerThrustForce->updateForce(&runners[i]);
		}
		for (int i = 0; i < attackers.size(); i++) {
			runnerThrustForce->set(runnerThrustSlider);
			runnerThrustForce->updateForce(&attackers[i]);
		}
	}
	
}

//--------------------------------------------------------------
void ofApp::draw() {

	//ofDrawLine();

	// Draw triangle ship
	tri.draw();

	// Draw other objects here
	if (bStartSim) {
		for (int i = 0; i < attackers.size(); i++) {
			attackers[i].draw();
			cout << "draw() " << attackers[i].verts[1] << endl;
		}
		for (int i = 0; i < runners.size(); i++) {
			runners[i].draw();
		}
	}

	// Draw heading if checked
	if (drawHeading) ofDrawLine(tri.pos, tri.pos - 100 * heading());

	// draw GUI if not hidden
	//
	if (!bHide) gui.draw();

	// draw some status to indicate flight modes
	//
	// draw screen data
	//
	string str, state;
	state = (bStartSim ? "ON" : "OFF");
	str += "Simulation : " + state;
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str, ofGetWindowWidth() - 170, 15);

}



//--------------------------------------------------------------
//
void ofApp::keyPressed(int key) {

	switch (key) {
	case 'h':   // show/hide GUI panel
		bHide = !bHide;
		break;
	case 'f':
		bFullscreen = !bFullscreen;
		ofSetFullscreen(bFullscreen);
		break;
	case 'r':   // add runner
		addRunner();
		break;
	case 'a':   // add attacker
		addAttacker();
		break;
	case 'n':   // delete all runners and attackers; reposition tri to center of window	
		resetScreen();
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_LEFT:   // turn left
		tri.angularVelocity -= 10;
		break;
	case OF_KEY_RIGHT:  // turn right
		tri.angularVelocity += 10;
		break;
	case OF_KEY_UP:     // go forward

		f = ofVec3f(-heading().x * thrustSlider, -heading().y * thrustSlider, -heading().z * thrustSlider);
		//ofVec3f x = heading() * thrustSlider;

		thrustForce->set(f);
		thrustForce->updateForce(&tri);
		tri.velocity -= 2 * heading();
		break;
	case OF_KEY_DOWN:   // go backwards

		f = ofVec3f(heading().x * thrustSlider, heading().y * thrustSlider, heading().z * thrustSlider);
		//ofVec3f x = heading() * thrustSlider;

		thrustForce->set(f);

		//thrustForce->set(thrustSlider);
		thrustForce->updateForce(&tri);
		tri.velocity += 2 * heading();
		break;
	case ' ':
		bStartSim = !bStartSim;
		break;
	default:
		break;
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

	switch (key) {
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_LEFT:   // turn left
		break;
	case OF_KEY_RIGHT:  // turn right
		break;
	case OF_KEY_UP:     // go forward
		break;
	case OF_KEY_DOWN:   // go backward
		break;
	default:
		break;
	}

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	if (bStartSim) return;

	glm::vec3 mouse = glm::vec3(x, y, 1);

	if (!tri.bSelected) return;

	// calculate a difference vector between the current mouse and the last mouse
	//
	glm::vec3 delta = mouse - mouseLast;

	// add the difference to the mouse's current position
	//
	tri.pos += delta;

	// store last mouse position for next time
	mouseLast = mouse;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	if (bStartSim) return;

	glm::vec3 mouse = glm::vec3(x, y, 1);

	//  test to see if mouse is "inside" triangle. We must accoutn for the fact that the triangle has been transformed
	//  so convert the mouse point so that it is in the triangle's local coordinate system by multiplying
	//  the mouse point by the matrix inverse of the triangle's matrix.
	//

	if (tri.inside(glm::inverse(tri.getMatrix()) * glm::vec4(mouse, 1))) {
		tri.bSelected = true;
		mouseLast = mouse;   // store initial position of mouse
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	tri.bSelected = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

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
