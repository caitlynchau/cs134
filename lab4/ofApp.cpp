//  CS 134 - In-Class exercise - Getting started with Particle Physics
//
//  Caitlyn Chau - CS 134 - SJSU CS

#include "ofApp.h"






//--------------------------------------------------------------
//  Setup Application data
//
void ofApp::setup(){
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	ofSetFrameRate(60);


	// some simple sliders to play with parameters
	//
	gui.setup();
	//gui.add(velocity.setup("Initial Velocity", ofVec3f(0, 20, 0), ofVec3f(0, 0, 0), ofVec3f(100, 100, 100)));	
	gui.add(lifespan.setup("Lifespan", 2.0, .1, 10.0));
	gui.add(rate.setup("Rate", 1.0, .5, 60.0));
	gui.add(damping.setup("Damping", .99, .1, 1.0));
    gui.add(gravity.setup("Gravity", -10, -20, 20));
	gui.add(radius.setup("Radius", .05, .01, .3));
	gui.add(turbMin.setup("Turbulence Min", ofVec3f(0, 0, 0), ofVec3f(-20, -20, -20), ofVec3f(20, 20, 20)));
	gui.add(turbMax.setup("Turbulence Max", ofVec3f(0, 0, 0), ofVec3f(-20, -20, -20), ofVec3f(20, 20, 20)));
	gui.add(radialForceVal.setup("Radial Force", 1000, 100, 5000));
	gui.add(height.setup("Radial Height", 0.01, 0.01, 0.4));
	gui.add(cyclic.setup("Cyclic Force", 0, 0, 500));
	

	bHide = false;

	// set up the emitter forces
	//
	tForce1 = new TurbulenceForce(ofVec3f(-20, -20, -20), ofVec3f(20, 20, 20));
	gForce1 = new GravityForce(ofVec3f(0, -10, 0)); //changed gravity
	rForce1 = new ImpulseRadialForce(300, 1);
	rForce1->setHeight(1);

	emitter1.sys->addForce(tForce1);
	emitter1.sys->addForce(gForce1);
	emitter1.sys->addForce(rForce1);

	emitter1.setVelocity(ofVec3f(0, 0, 0));
	emitter1.setOneShot(true);
	emitter1.setEmitterType(RadialEmitter);
	emitter1.setGroupSize(3000);
	

	tForce2 = new TurbulenceForce(ofVec3f(turbMin->x, turbMin->y, turbMin->z), ofVec3f(turbMax->x, turbMax->y, turbMax->z));
	gForce2 = new GravityForce(ofVec3f(0, -10, 0));
	rForce2 = new ImpulseRadialForce(radialForceVal, 1);
	rForce2->setHeight(0.02);
	cForce2 = new CyclicForce(cyclic);
	
	emitter2.sys->addForce(tForce2);
	emitter2.sys->addForce(gForce2);
	emitter2.sys->addForce(rForce2);
	emitter2.sys->addForce(cForce2);
	
	emitter2.setVelocity(ofVec3f(0, 0, 0));
	emitter2.setOneShot(true);
	emitter2.setEmitterType(RadialEmitter);
	emitter2.setGroupSize(1000);
	

}

//--------------------------------------------------------------
//
void ofApp::update() {
	ofSeedRandom();

	emitter1.setLifespan(lifespan);
	emitter1.setRate(rate);
	emitter1.setParticleRadius(radius);
	emitter1.update();

	emitter2.setLifespan(lifespan);
	emitter2.setRate(rate);
	emitter2.setParticleRadius(radius);
	gForce2->set(ofVec3f(0, gravity, 0)); 
	tForce2->set(ofVec3f(turbMin->x, turbMin->y, turbMin->z), ofVec3f(turbMax->x, turbMax->y, turbMax->z));
	rForce2->setHeight(height);
	cForce2->setMagnitude(cyclic);
	emitter2.update();
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetBackgroundColor(ofColor::black);

	// draw the GUI
	if (!bHide) gui.draw();

	// begin drawing in the camera
	//
	cam.begin();

	// draw a grid
	//
	ofPushMatrix();
	ofRotateDeg(90, 0, 0, 1);
	ofSetLineWidth(1);
	ofSetColor(ofColor::dimGrey);
	ofDrawGridPlane();
	ofPopMatrix();

	// draw particle emitter here..
	//
	emitter1.draw();
	emitter2.draw();

	//  end drawing in the camera
	// 
	cam.end();

	// draw screen data
	//
	string str;
	str += "Frame Rate: " + std::to_string(ofGetFrameRate());
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str, ofGetWindowWidth() -170, 15);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key) {
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'b':
		break;
	case 'f':
		ofToggleFullscreen();
		break;
	case 'h':
		bHide = !bHide;
	case ' ':
		emitter1.sys->reset();
		emitter1.start();

		emitter2.sys->reset();
		emitter2.start();
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
