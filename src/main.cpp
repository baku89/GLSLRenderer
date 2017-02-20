#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

#include "WindowUtils.h"
#include "Config.h"

//========================================================================
int main( ){
	
	ofAppGLFWWindow win;
	
	GLFWwindow *gw = win.getGLFWWindow();
	ofSetupOpenGL(&win, 512+GUI_WIDTH,512,OF_WINDOW);			// <-------- setup the GL context
	
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
	
}
