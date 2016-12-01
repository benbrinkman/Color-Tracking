#pragma once
#include "ofMain.h"


class particle
{
public:

	ofVec2f pos;
	float diamater;
	float angle;
	float speed;
	float decay;
	float opacity;

	void randomize();
};

