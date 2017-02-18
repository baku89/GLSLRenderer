#include "ofApp.h"

#include "ImOf.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	// setup window attributes
//	ofEnableNormalizedTexCoords();
	ofSetWindowTitle("GLSL Renderer");
	ofEnableSmoothing();
	
	// setup imgui
	ImOf::SetFont();
	gui.setup();
	ImOf::SetStyle();
	
	
	vidRecorder.setVideoCodec("mpeg4");
	vidRecorder.setVideoBitrate("800k");
	vidRecorder.setAudioCodec("mp3");
	vidRecorder.setAudioBitrate("192k");

	
	// setup
	glsl.setup();
	
	// load settings
	ofxXmlSettings settings("settings.xml");

	duration = settings.getValue("duration", duration);
	frameRate = settings.getValue("frameRate", frameRate);
	ofSetFrameRate(frameRate);
	
	glsl.loadSettings(settings);
	
	
}

//--------------------------------------------------------------
void ofApp::update(){
	
	if (isExporting) {
		time = (float)currentFrame++ / frameRate;
		
		if (currentFrame == duration) {
			
		}
	}
		time = fmod(ofGetElapsedTimef(), (float)duration / frameRate);
	}
	
	
	glsl.update();
	glsl.render(time);
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofBackgroundHex(0x1a2b35);
	
	ofSetColor(255);
	
	glsl.draw(ofGetWidth() - glsl.getWidth(), 0);
	
	drawImGui();
}

//--------------------------------------------------------------
void ofApp::beginExport() {
	ofFileDialogResult result = ofSystemSaveDialog("export.mov", "Save");
	
	if (result.bSuccess) {
		
		currentFrame = 0;
	}
		
}
//--------------------------------------------------------------
void ofApp::drawImGui(){
	
	gui.begin();
	
	static bool p_open = true;
	
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSizeConstraints(ImVec2(250, ofGetHeight()), ImVec2(ofGetWidth(), ofGetHeight()));
	ImGui::Begin("Control", &p_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
	ImGui::SetWindowSize(ImVec2(ImGui::GetWindowWidth(), ofGetHeight()));
	{
	
		
		ImGui::Text("Current Time: %.2f", time);
		
		ImGui::Separator();
		
		ImGui::InputInt("Duration", &duration);
		
		if (ImGui::InputInt("Frame Rate", &frameRate)) {
			ofSetFrameRate(frameRate);
		}
		
		if (ImGui::Button("Export")) {
			
			beginExport();
		}
		
		if (isExporting) {
			ImGui::SameLine();
			ImGui::Text("Rendering.. (%d / %d", currentFrame, duration);
		}
	
		
		
		glsl.drawImGui();
		
		
	}
	ImGui::End();
	
	gui.end();
	
}

//--------------------------------------------------------------
void ofApp::exit() {
	
	// save settings
	ofxXmlSettings settings;
	
	settings.setValue("duration", duration);
	settings.setValue("frameRate", frameRate);
	
	glsl.saveSettings(settings);
	
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
