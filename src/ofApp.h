#pragma once

#include "ofMain.h"

#include "ofxXmlSettings.h"
#include "ofxImGui.h"
#include "ofxVideoRecorder.h"

#include "GLSLManager.h"
#include "ShaderFileManager.h"

enum ExportingStatus {
	stopped,
	exporting,
	saving
};

struct Codec {
	string	label;
	string	name;
	string	extension;
};


class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void drawImGui();
	
	void exit();
	
	void beginExport(Codec codec, int bitrate);
	void endExport();
	
	// event
	void frameRateUpdated(int &frameRate);
	void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args);
	void shaderFileSelected(string &path);

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
	
	GLSLManager			glsl;
	ShaderFileManager	shaderFile;
	
	ofxVideoRecorder    vidRecorder;
	ofPixels			pixels;
	
	// params
	
	ExportingStatus		exportingStatus = stopped;
	
	int					currentFrame;
	
	vector<Codec>		codecs;
	int					selectedCodec = 0;
	int					bitrate = 800;
	
	
};
