#include "ofApp.h"

#include "ImOf.h"
#include "Config.h"

#define MAX_FPS 90

//--------------------------------------------------------------
void ofApp::setup(){
	
	// setup window attributes
	ofSetWindowTitle("GLSL Renderer");
	ofEnableSmoothing();
	ofSetFrameRate(60);
	
	// setup imgui
	ImOf::SetFont();
	gui.setup();
	ImOf::SetStyle();
	
	vidRecorder.setVideoCodec("png");
	vidRecorder.setVideoBitrate("800k");
	
	// setup
	glsl.setup();
	shaderFile.setup();
	
	// event
	ofAddListener(glsl.frameRateUpdated, this, &ofApp::frameRateUpdated);
	ofAddListener(shaderFile.shaderFileSelected, this, &ofApp::shaderFileSelected);
	ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
	
	// load settings
	ofxXmlSettings settings("settings.xml");
	
	glsl.loadSettings(settings);
	shaderFile.loadSettings(settings);
	
	
}

//--------------------------------------------------------------
void ofApp::update(){
	
	shaderFile.update();
	
	if (exportingStatus == stopped) {
		glsl.update();
		
	} else if (exportingStatus == exporting) {
		
		glsl.readToPixelsAtFrame(currentFrame, pixels);
		vidRecorder.addFrame(pixels);
		
		if (++currentFrame == glsl.getDuration()) {
			endExport();
		}
	}
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofBackground(0);
	
	glsl.draw();
	shaderFile.draw();
	
	drawImGui();
}

//--------------------------------------------------------------
void ofApp::beginExport() {
	ofFileDialogResult result = ofSystemSaveDialog("export.mov", "Save");
	
	if (result.bSuccess && !vidRecorder.isInitialized()) {
		
		int w = glsl.getWidth(), h = glsl.getHeight();
		int frameRate = glsl.getFrameRate();
		
		currentFrame = 0;
		pixels.allocate(w, h, GL_RGB);
		
		vidRecorder.setup(result.getPath(), w, h, frameRate);
		vidRecorder.start();
		
		exportingStatus = exporting;
		ofSetFrameRate(MAX_FPS);
	}
		
}

//--------------------------------------------------------------
void ofApp::endExport() {
	vidRecorder.close();
	exportingStatus = saving;
	
	ofSetFrameRate(glsl.getFrameRate());
}


//--------------------------------------------------------------
// events

void ofApp::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args) {
	exportingStatus = stopped;
}

void ofApp::shaderFileSelected(string &path) {
	glsl.loadShader(path);
}

void ofApp::frameRateUpdated(int &frameRate) {
	ofSetFrameRate(frameRate);
}

//--------------------------------------------------------------
void ofApp::drawImGui(){
	
	gui.begin();
	
	static bool p_open = true;
	
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(GUI_WIDTH, ofGetHeight()));
	ImGui::Begin("Control", &p_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	ImGui::SetWindowSize(ImVec2(ImGui::GetWindowWidth(), ofGetHeight()));
	{
	
		static bool isOpen = true;
		
		ImGui::SetNextTreeNodeOpen(isOpen);
		
		if ((isOpen = ImGui::CollapsingHeader("General"))) {
			
			if (ImGui::Button("Export")) {
				
				beginExport();
			}
			
			ImGui::SameLine();
			
			switch (exportingStatus) {
				case stopped:
					ImGui::Text("");
					break;
				case exporting:
					static char overlay[512];
					sprintf(overlay, "Rendering.. (%d / %d)", currentFrame, glsl.getDuration());
					ImGui::ProgressBar((float)currentFrame / glsl.getDuration(), ImVec2(-1, 0), overlay);
					break;
				case saving:
					ImGui::ProgressBar(1.0f, ImVec2(-1, 0), "Saving..");
					break;
			}
			
			ImGui::Separator();
		}
		
		
		glsl.drawImGui();
		shaderFile.drawImGui();
		
		
	}
	ImGui::End();
	
	
	gui.end();
	
}

//--------------------------------------------------------------
void ofApp::exit() {
	
	vidRecorder.close();
	
	// save settings
	ofxXmlSettings settings;
	
	glsl.saveSettings(settings);
	shaderFile.saveSettings(settings);
	
	settings.saveFile("settings.xml");
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
