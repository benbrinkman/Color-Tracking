#include "ofApp.h"
#include "affine.hpp"

//--------------------------------------------------------------
void ofApp::setup()
{
	m_numPaintBlobs = 255;
	m_paintCounter = 0;
	m_obj1 = m_obj2 = m_obj1Activate = m_obj2Activate = false;
	m_numObjects = 0;
	m_close = false;
	m_colorLoc1 = m_colorLoc2 = m_middleLoc = m_particlePos = m_particlePos1 = m_particlePos2 = ofVec2f(0, 0);
	m_threshold = 20;
	m_minRadius = 10.0f;
	m_maxRadius = 150.0f;
	m_threshold2 = 20;
	m_minRadius2 = 10.0f;
	m_maxRadius2 = 150.0f;
	
	m_particleOn = false;
	for (int i = 0; i < 20; i++) {
		//randomly initiate the particles
		m_particleMix[i].randomize();
		m_particleObj1[i].randomize();
		m_particleObj2[i].randomize();
	}
	for (int i = 0; i < m_numPaintBlobs; i++) {
		
		m_obj1Paint[i].pos = ofVec2f(-10, -10);
		m_obj1Paint[i].diamater = ofRandom(5, 30);
		m_obj1Paint[i].opacity = 255;


		m_obj2Paint[i].pos = ofVec2f(-10, -10);
		m_obj2Paint[i].diamater = ofRandom(5, 30);
		m_obj2Paint[i].opacity = 255;
	}
	
	m_trackedColor1 = NULL;
	m_trackedColor2 = NULL;
	m_colorCombine = NULL;
	m_gui.setup();

	if ( USE_WEB_CAM ) {
		m_webcam.setup( 640, 480 );
	}
	else {
		//if no webcam, trackable video
		m_video.load("color_small.mov");

		m_video.setLoopState(OF_LOOP_NORMAL);
		m_video.play();
	}
}

//--------------------------------------------------------------
void ofApp::update()
{
	ofSetFrameRate(60);
	//find location inbetween the selected colors
	m_middleLoc = (m_colorLoc1 + m_colorLoc2) / 2.0f;
	
	//tweaking the variables for finding the color
	m_contourFinder1.setThreshold( m_threshold );	
	m_contourFinder1.setMinAreaRadius( m_minRadius );
	m_contourFinder1.setMaxAreaRadius( m_maxRadius );
	m_contourFinder1.setTargetColor( m_trackedColor1, ofxCv::TRACK_COLOR_RGB );

	m_contourFinder2.setThreshold(m_threshold2);	
	m_contourFinder2.setMinAreaRadius(m_minRadius2);	
	m_contourFinder2.setMaxAreaRadius(m_maxRadius2);
	m_contourFinder2.setTargetColor(m_trackedColor2, ofxCv::TRACK_COLOR_RGB);


	//get average color of selected colors for the particles when they meet
	m_colorCombine.r = (m_trackedColor1.r + m_trackedColor2.r) / 2;
	m_colorCombine.g = (m_trackedColor1.g + m_trackedColor2.g) / 2;
	m_colorCombine.b = (m_trackedColor1.b + m_trackedColor2.b) / 2;

	//update the webcam
	if ( USE_WEB_CAM ) {
		m_webcam.update();

		if ( true == m_webcam.isFrameNew() ) {
			m_contourFinder1.findContours(m_webcam);
			m_contourFinder2.findContours(m_webcam);
		}
	}
	//alternate code if video used instead of webcam
	else {
		m_video.update();

		if (true == m_video.isFrameNew()) {
			m_contourFinder1.findContours(m_video);
			m_contourFinder2.findContours(m_video);
		}
	}

	
	/******************************Particle 1*********************************/
	if (m_particleOn) {	
		bool isOver = true;
		for (int i = 0; i < 20; i++) {
			if (m_particleMix[i].opacity > 0) {
				m_particleMix[i].opacity -= m_particleMix[i].decay;			//make the particles fade
				m_particleMix[i].diamater -= m_particleMix[i].decay * 10;	//make them get smaller
			}
			else {
				m_particleMix[i].opacity = 0;								//stop fading/shrinking when it gets to 0
				m_particleMix[i].diamater = 0;
			}
			m_particleMix[i].pos.x += m_particleMix[i].speed;				//move the particles
			if (m_particleMix[i].opacity > 0)
				isOver = false;												//once all particles have dissapeared, end particle effect
		}
		if (isOver) {
			m_particleOn = false;
		}
	}


	/******************************Particle 2*********************************/

	if (m_particleOn1) {
		bool isOver = true;
		for (int i = 0; i < 20; i++) {
			if (m_particleObj1[i].opacity > 0) {
				m_particleObj1[i].opacity -= m_particleObj1[i].decay;
				m_particleObj1[i].diamater -= m_particleObj1[i].decay * 10;
			}
			else {
				m_particleObj1[i].opacity = 0;
				m_particleObj1[i].diamater = 0;
			}
			m_particleObj1[i].pos.x += m_particleObj1[i].speed;
			if (m_particleObj1[i].opacity > 0)
				isOver = false;
		}
		if (isOver) {
			m_particleOn1 = false;
		}
	}
	
	/******************************Particle 3*********************************/

	if (m_particleOn2) {
		bool isOver = true;
		for (int i = 0; i < 20; i++) {
			if (m_particleObj1[i].opacity > 0) {
				m_particleObj2[i].opacity -= m_particleObj2[i].decay;
				m_particleObj2[i].diamater -= m_particleObj2[i].decay * 10;
			}
			else {
				m_particleObj2[i].opacity = 0;
				m_particleObj2[i].diamater = 0;
			}
			m_particleObj2[i].pos.x += m_particleObj2[i].speed;
			if (m_particleObj2[i].opacity > 0)
				isOver = false;
		}
		if (isOver) {
			m_particleOn2 = false;
		}
	}

	/******************************Paint blobs*********************************/

	m_paintCounter++;									//change which blob is being changed every frame
	if (m_paintCounter == m_numPaintBlobs)
		m_paintCounter = 0;								//cap at max amount of blobs being used
	if (m_obj1 && m_obj1Activate) {
		m_obj1Paint[m_paintCounter].pos = m_colorLoc1;	//set blob location to current position of tracked color
		m_obj1Paint[m_paintCounter].opacity = 255;		//set at full opacity
	}
	else {
		m_obj1Paint[m_paintCounter].pos = ofVec2f(-100,-100);	//draw off screen if not being used

	}

	//same thing as above for different color blobs
	if (m_obj2 && m_obj2Activate) {
		m_obj2Paint[m_paintCounter].pos = m_colorLoc2;
		m_obj2Paint[m_paintCounter].opacity = 255;
	}
	else {
		m_obj2Paint[m_paintCounter].pos = ofVec2f(-100, -100);

	}


	/******************************Distance Finder*********************************/

	m_distance = sqrt((m_colorLoc1.x - m_colorLoc2.x) * (m_colorLoc1.x - m_colorLoc2.x) + (m_colorLoc1.y - m_colorLoc2.y) * (m_colorLoc1.y - m_colorLoc2.y));

	//ofLogNotice("Distance " + ofToString(m_distance));
	if (m_distance < 60 && m_distance != 0) {			//particle initiater
		m_close = true;
		for (int i = 0; i < m_numPaintBlobs; i++) {
			m_obj1Paint[i].pos = ofVec2f(-100, -100);
			m_obj2Paint[i].pos = ofVec2f(-100, -100);
		}

		if (!m_particleOn) {
			m_particleOn = true;
			for (int i = 0; i < 20; i++) {
				m_particleMix[i].randomize();
				m_particlePos = m_middleLoc;
			}
		}
	}
	else {
		m_close = false;
	}

	//reduce paint's opacity as time progresses
	for (int i = 0; i < m_numPaintBlobs; i++) {
		if (m_obj1Paint[i].opacity != 0)
			m_obj1Paint[i].opacity--;
		if (m_obj2Paint[i].opacity != 0)
			m_obj2Paint[i].opacity--;

	}
}

void ofApp::draw()
{
	ofSetRectMode(OF_RECTMODE_CENTER);
	ofBackground( ofColor::black );
	ofSetColor( ofFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ) );

	if ( USE_WEB_CAM ) {
		m_webcam.draw(320, 240);
	}
	else {
		m_video.draw(320, 240);
	}
	
	
	if ((m_obj1 && !m_obj2) || (m_obj2 && !m_obj1)) {	//if only one object selected
		
		if (m_obj1 && m_obj1Activate) {
			ofSetColor(m_trackedColor1);
			ofDrawRectangle(m_colorLoc1, 60, 60);		//draw as squares if alone
		}
		else {
			ofDrawCircle(-200, -200, 30);
		}


		if (m_obj2 && m_obj2Activate) {
			ofSetColor(m_trackedColor2);
			ofDrawRectangle(m_colorLoc2, 60, 60);
		}
		else {
			ofDrawCircle(-100, -100, 30);
		}


	}
	else {
		

		if (m_obj1 && m_obj1Activate) {			//if both active, draw as circles
			ofSetColor(m_trackedColor1);
			ofDrawCircle(m_colorLoc1, 30);
		}
		else {
			ofDrawCircle(-200, -200, 30);
		}


		if (m_obj2 && m_obj2Activate) {
			ofSetColor(m_trackedColor2);
			ofDrawCircle(m_colorLoc2, 30);
		}
		else {
			ofDrawCircle(-100, -100, 30);
		}

	}

	for (int i = 0; i < m_numPaintBlobs; i++) {			//draw all the paint blobs in their respective positions
		if (m_obj2 && m_obj1) {
			ofSetColor(m_trackedColor1, m_obj1Paint[i].opacity);
			ofDrawCircle(m_obj1Paint[i].pos, m_obj1Paint[i].diamater);
			ofSetColor(m_trackedColor2, m_obj2Paint[i].opacity);
			ofDrawCircle(m_obj2Paint[i].pos, m_obj2Paint[i].diamater);
		}
		else{
			ofSetColor(m_trackedColor1, m_obj1Paint[i].opacity);
			ofDrawRectangle(m_obj1Paint[i].pos, m_obj1Paint[i].diamater * 2, m_obj1Paint[i].diamater * 2);
			ofSetColor(m_trackedColor2, m_obj2Paint[i].opacity);
			ofDrawRectangle(m_obj2Paint[i].pos, m_obj2Paint[i].diamater * 2, m_obj2Paint[i].diamater * 2);
		}
	}



	ofFill();


	ofSetColor(m_trackedColor1);		//draw the color selected
	ofDrawRectangle(670, 40, 60, 60);

	ofSetColor(m_trackedColor2);
	ofDrawRectangle(730, 40, 60, 60);
	

	drawContour1(m_contourFinder1, ofFloatColor(0, 1, 0));	
	drawContour2(m_contourFinder2, ofFloatColor(1, 0, 0));

	ofFill();
	ofSetColor(ofColor::white);
	
	if (m_particleOn) {					//draw the particles
		for (int i = 0; i < 20; i++) {
			ofPushMatrix();
				ofSetColor(m_colorCombine, m_particleMix[0].opacity * 255);
				ofTranslate(m_particlePos);
				ofRotate(m_particleMix[i].angle);
				ofTranslate(m_particleMix[i].pos);
				ofDrawCircle(0, 0, m_particleMix[i].diamater);
			ofPopMatrix();
		}
	}

	if (m_particleOn1) {
		for (int i = 0; i < 20; i++) {
			ofPushMatrix();
			ofSetColor(m_trackedColor1, m_particleObj1[i].opacity*255);
			ofTranslate(m_particlePos1);
			ofRotate(m_particleObj1[i].angle);
			ofTranslate(m_particleObj1[i].pos);
			ofDrawCircle(0, 0, m_particleObj1[i].diamater);
			ofPopMatrix();
		}
	}
	
	if (m_particleOn2) {
		for (int i = 0; i < 20; i++) {
			ofPushMatrix();
			ofSetColor(m_trackedColor2, m_particleObj2[i].opacity * 255);
			ofTranslate(m_particlePos2);
			ofRotate(m_particleObj2[i].angle);
			ofTranslate(m_particleObj2[i].pos);
			ofDrawCircle(0, 0, m_particleObj2[i].diamater);
			ofPopMatrix();
		}
	}
	

	ofSetColor(ofColor::white);
	

	//draw the gui screen
	m_gui.begin();
		ImGui::SliderInt( "Threshold 1", &m_threshold, 0, 255 );
		ImGui::SliderFloat( "Min Radius 1", &m_minRadius, 0.0f, 1000.0f );
		ImGui::SliderFloat( "Max Radius 1", &m_maxRadius, 0.0f, 1000.0f );
		
		ImGui::SliderInt("Threshold 2", &m_threshold2, 0, 255);
		ImGui::SliderFloat("Min Radius 2", &m_minRadius2, 0.0f, 1000.0f);
		ImGui::SliderFloat("Max Radius 2", &m_maxRadius2, 0.0f, 1000.0f);

	m_gui.end();

}

void ofApp::drawContour1(ofxCv::ContourFinder & _contourFinder, ofFloatColor trackCol)
{
	ofSetRectMode(OF_RECTMODE_CENTER);
	int numContours = _contourFinder.getContours().size();
	m_numObjects = numContours;
	if (numContours > 0) {
		int indexLargest = 0;
		float largestArea = 0.0f;
		for (int i = 0; i < numContours; ++i)
		{
			if (_contourFinder.getContourArea(i) > largestArea) {
				indexLargest = i;
			}
		}

		ofNoFill();
		ofSetColor(trackCol);

		cv::Point2f center = _contourFinder.getCenter(indexLargest);
		

		m_colorLoc1 = ofVec2f(center.x, center.y);
		m_obj1 = true;
	}
	else {
		m_colorLoc1 = ofVec2f(-200, -200);
		m_obj1 = false;
	}

}

void ofApp::drawContour2(ofxCv::ContourFinder & _contourFinder, ofFloatColor trackCol)
{
	ofSetRectMode(OF_RECTMODE_CENTER);
	int numContours = _contourFinder.getContours().size();

	if (numContours > 0) {
		int indexLargest = 0;
		float largestArea = 0.0f;
		for (int i = 0; i < numContours; ++i)
		{
			if (_contourFinder.getContourArea(i) > largestArea) {
				indexLargest = i;
			}
		}

		ofNoFill();
		ofSetColor(trackCol);

		cv::Point2f center = _contourFinder.getCenter(indexLargest);


		m_colorLoc2 = ofVec2f(center.x, center.y);
		m_obj2 = true;
	}
	else {
		m_colorLoc2 = ofVec2f(-100, -100);
		m_obj2 = false;
	}
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if ( key == 's' )
	{
		if ( !USE_WEB_CAM ) {
			m_video.setPaused(!m_video.isPaused());	//pause video
		}
	}
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

	if ( button == OF_MOUSE_BUTTON_RIGHT )
	{
		m_obj1Activate = true;
		m_particleOn1 = true;
			for (int i = 0; i < 20; i++) {
				m_particleObj1[i].randomize();
				m_particlePos1 = ofVec2f(x, y);
			}
		
			//color selection
		if (USE_WEB_CAM)  {
			int index = (x + y * m_webcam.getWidth()) * m_webcam.getPixels().getBytesPerPixel();
			m_trackedColor1.r = m_webcam.getPixels()[index];
			m_trackedColor1.g = m_webcam.getPixels()[index + 1];
			m_trackedColor1.b = m_webcam.getPixels()[index + 2];
		}
		else {
			int index = (x + y * m_video.getWidth()) * m_video.getPixels().getBytesPerPixel();
			m_trackedColor1.r = m_video.getPixels()[index];
			m_trackedColor1.g = m_video.getPixels()[index + 1];
			m_trackedColor1.b = m_video.getPixels()[index + 2];
		}
	}

	if (button == OF_MOUSE_BUTTON_MIDDLE)
	{
		m_obj2Activate = true;
		if (!m_particleOn2) {

			m_particleOn2 = true;
			for (int i = 0; i < 20; i++) {
				m_particleObj2[i].randomize();
				m_particlePos2 = ofVec2f(x, y);
			}
		}

		if (USE_WEB_CAM) {
			int index = (x + y * m_webcam.getWidth()) * m_webcam.getPixels().getBytesPerPixel();
			m_trackedColor2.r = m_webcam.getPixels()[index];
			m_trackedColor2.g = m_webcam.getPixels()[index + 1];
			m_trackedColor2.b = m_webcam.getPixels()[index + 2];
		}
		else {
			int index = (x + y * m_video.getWidth()) * m_video.getPixels().getBytesPerPixel();
			m_trackedColor2.r = m_video.getPixels()[index];
			m_trackedColor2.g = m_video.getPixels()[index + 1];
			m_trackedColor2.b = m_video.getPixels()[index + 2];
		}
	}
}


