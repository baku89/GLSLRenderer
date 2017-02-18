#pragma once

#include "ofMain.h"

#include "ofxXmlSettings.h"
#include "ofxImGui.h"
#include "ofxVideoRecorder.h"

#include "GLSLRenderer.h"

enum ExportingStatus {
	stopped,
	exporting,
	saving
};


class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void drawImGui();
	
	void exit();
	
	void beginExport();
	void endExport();
	
	void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args);
	

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
	
	ofxImGui::Gui		gui;
	GLSLRenderer		glsl;
	
	ofxVideoRecorder    vidRecorder;
	ofPixels			pixels;
	
	// params
	float				time;
	int					duration = 120;
	int					frameRate = 30;
	
	
	ExportingStatus		exportingStatus = stopped;
	
	int					currentFrame;
	
	
		
};
