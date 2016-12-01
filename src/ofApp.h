#pragma once

#include "ofMain.h"

#include "ofxImGui.h"
#include "ofxCv.h"
#include "particle.h"

class ofApp : public ofBaseApp{
	//change to false if you wish to use a video instead of live webcam feed
	#define USE_WEB_CAM true


public:
	void setup();
	void update();
	void draw();

	void drawContour1(ofxCv::ContourFinder & _contourFinder, ofFloatColor trackCol);
	void drawContour2(ofxCv::ContourFinder & _contourFinder, ofFloatColor trackCol);



	void keyPressed(int key);
	void mousePressed(int x, int y, int button);


	bool					m_obj1Activate;
	bool					m_obj2Activate;
	ofxImGui				m_gui;

	ofVideoPlayer			m_video;
	ofVideoGrabber			m_webcam;

	ofxCv::ContourFinder	m_contourFinder1;
	ofxCv::ContourFinder	m_contourFinder2;

	ofColor                 m_trackedColor1;
	ofColor                 m_trackedColor2;
	ofColor					m_colorCombine;

	int						m_numObjects;
	int  					m_threshold;
	float					m_minRadius;
	float					m_maxRadius;

	int  					m_threshold2;
	float					m_minRadius2;
	float					m_maxRadius2;

	ofVec2f					m_middleLoc;
	ofVec2f					m_colorLoc1;
	ofVec2f					m_colorLoc2;

	float					m_distance;
	bool					m_close;

	bool					m_obj1;
	bool					m_obj2;

	//particles for when color one is selected
	bool					m_particleOn;
	ofVec2f					m_particlePos;
	particle				m_particleMix[20];

	//particles for when color 2 is selected
	bool					m_particleOn1;
	ofVec2f					m_particlePos1;
	particle				m_particleObj1[20];

	//particles for when the colors meet
	bool					m_particleOn2;
	ofVec2f					m_particlePos2;
	particle				m_particleObj2[20];

	//for the painting onscreen
	int						m_numPaintBlobs;
	particle				m_obj1Paint[255];
	particle				m_obj2Paint[255];
	int						m_paintCounter;

};
