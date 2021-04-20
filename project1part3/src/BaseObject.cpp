#include "BaseObject.h"
#include "ofApp.h"

// BaseObject class

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