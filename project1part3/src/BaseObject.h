#pragma once
#include "ofMain.h"
#include "ofxGui.h"

typedef enum { MoveStop, MoveLeft, MoveRight, MoveUp, MoveDown, MoveCircle, MoveSine } MoveDir;
typedef enum { LinearEnemy, CircularEnemy, SineEnemy } EnemyType;


class BaseObject {
public:
	BaseObject();
	glm::vec3 scaleVector;
	float rot;
	bool bSelected;
	glm::vec3 pos; //added
	glm::vec3 heading;
	bool isEnemy = false;
	
	void setPosition(glm::vec3);
	glm::mat4 getMatrix();
};

