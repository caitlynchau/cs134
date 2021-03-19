#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Particle.h"
#include "ParticleEmitter.h"



class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofEasyCam    cam;
		ParticleEmitter emitter1;
		ParticleEmitter emitter2;

		// adding forces
		//
		TurbulenceForce *tForce1;
		GravityForce *gForce1;
		ImpulseRadialForce *rForce1;

		TurbulenceForce *tForce2;
		GravityForce *gForce2;
		ImpulseRadialForce *rForce2;
		CyclicForce *cForce2;


		// some simple sliders to play with parameters
		//
		bool bHide;
		ofxFloatSlider gravity;
		ofxFloatSlider damping;
		ofxFloatSlider radius;
		//ofxVec3Slider velocity;
		ofxFloatSlider lifespan;
		ofxFloatSlider rate;

		//
		// added more sliders for radial
		//
		ofxVec3Slider turbMin;
		ofxVec3Slider turbMax;
		ofxFloatSlider mass;
		ofxFloatSlider radialForceVal;
		ofxFloatSlider height;
		ofxFloatSlider cyclic;


		ofxPanel gui;
};
