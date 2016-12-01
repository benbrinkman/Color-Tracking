#pragma once

#include "particle.h"



void particle::randomize() {
	angle = ofRandom(-180, 180);
	decay = ofRandom(0.01f, 0.05f);
	diamater = ofRandom(5, 20);
	pos = ofVec2f(0, 0);
	speed = ofRandom(0.5f, 3.0f);
	opacity = 1.0f;

}