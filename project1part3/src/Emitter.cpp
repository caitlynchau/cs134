#include "Emitter.h"

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
	velocity = ofVec3f(0, 100, 0);
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


//THIS WONT WORK ANYMORE
// Move the Enemy based off a direction: Stop, Left, Right, Up, Down
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

// Integrator for simple trajectory physics
//
void Emitter::integrate() {

	// interval for this step
	//
	float dt = 1.0 / 60.0;

	// update position based on velocity
	//
	pos += (velocity * dt);
	rot += angularVelocity * dt;

	// update acceleration with accumulated paritcles forces
	// remember :  (f = ma) OR (a = 1/m * f)
	//
	ofVec3f accel = acceleration;    // start with any acceleration already on the particle

	accel += (forces * (1.0 / mass));
	//cout << "integrate() = " << forces << endl;
	velocity += accel * dt;

	//cout << "integrate() velocity = " << velocity << endl;

	// add a little damping for good measure
	//
	velocity *= damping;
	angularVelocity *= damping;

	// clear forces on particle (they get re-added each step)
	//
	forces.set(0, 0, 0);
}
