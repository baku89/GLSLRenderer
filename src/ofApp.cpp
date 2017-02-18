#include "ofApp.h"

#include "ImOf.h"
#include "Config.h"

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
	
	// setup
	glsl.setup();
	
	// load settings
	ofxXmlSettings settings("settings.xml");

	duration = settings.getValue("duration", duration);
	frameRate = settings.getValue("frameRate", frameRate);
	ofSetFrameRate(frameRate);
	
	glsl.loadSettings(settings);
	
	ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
}

//--------------------------------------------------------------
void ofApp::update(){
	
	if (exportingStatus == exporting) {
		time = (float)currentFrame / frameRate;
	} else {
		time = fmod(ofGetElapsedTimef(), (float)duration / frameRate);
	}
	
	glsl.update();
	glsl.render(time);
	
	if (exportingStatus == exporting) {
		
		glsl.readToPixels(pixels);
		vidRecorder.addFrame(pixels);
		
		if (++currentFrame == duration) {
			endExport();
		}
	}
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofBackground(0);
	
	glsl.draw();
	
	drawImGui();
}

//--------------------------------------------------------------
void ofApp::beginExport() {
	ofFileDialogResult result = ofSystemSaveDialog("export.mov", "Save");
	
	if (result.bSuccess && !vidRecorder.isInitialized()) {
		
		int w = glsl.getWidth(), h = glsl.getHeight();
		
		currentFrame = 0;
		pixels.allocate(w, h, GL_RGB);
		
		vidRecorder.setup(result.getPath(), w, h, frameRate);
		vidRecorder.start();
		
		exportingStatus = exporting;
		
		ofLogNotice() << "Start!";
	}
		
}

//--------------------------------------------------------------
void ofApp::endExport() {
	vidRecorder.close();
	exportingStatus = saving;
}

void ofApp::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args) {
	exportingStatus = saved;
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
	
		
		ImGui::Text("Current Time: %.2f", time);
		
		ImGui::Separator();
		
		ImGui::InputInt("Duration", &duration);
		
		if (ImGui::InputInt("Frame Rate", &frameRate)) {
			ofSetFrameRate(frameRate);
		}
		
		if (ImGui::Button("Export")) {
			
			beginExport();
		}
		
		ImGui::SameLine();
		
		switch (exportingStatus) {
			case stopped:
				ImGui::Text("");
				break;
			case exporting:
				ImGui::Text("Rendering.. (%d / %d)", currentFrame, duration);
				break;
			case saving:
				ImGui::Text("Saving..");
				break;
			case saved:
				ImGui::Text("Saved");
				break;
		}
		
		glsl.drawImGui();
		
		
	}
	ImGui::End();
	
	gui.end();
	
}

//--------------------------------------------------------------
void ofApp::exit() {
	
	vidRecorder.close();
	
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
