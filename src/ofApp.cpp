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
	
	// set codecs
	codecs.push_back((Codec){"PNG", "png", "mov"});
	codecs.push_back((Codec){"MPEG4", "mpeg4", "mov"});
	
	// setup
	glsl.setup();
	shaderFile.setup();
	
	// event
	ofAddListener(glsl.frameRateUpdated, this, &ofApp::frameRateUpdated);
	ofAddListener(shaderFile.shaderFileSelected, this, &ofApp::shaderFileSelected);
	ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
	
	// load settings
	ofxXmlSettings settings("settings.xml");
	
	selectedCodec	= settings.getValue("selectedCodec", selectedCodec);
	bitrate			= settings.getValue("bitrate", bitrate);
	
	glsl.loadSettings(settings);
	shaderFile.loadSettings(settings);
}

//--------------------------------------------------------------
void ofApp::update(){
	
	glsl.update();
	shaderFile.update();
	
	if (exportingStatus == exporting) {
		
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
void ofApp::beginExport(Codec codec, int bitrate) {
	
	ofFileDialogResult result = ofSystemSaveDialog("export." + codec.extension, "Save");
	
	if (!result.bSuccess) {
		return;
	}
	
	vidRecorder.setVideoCodec(codec.name);
	vidRecorder.setVideoBitrate(ofToString(bitrate) + "k");
		
	int w = glsl.getWidth(), h = glsl.getHeight();
	int frameRate = glsl.getFrameRate();
	
	currentFrame = 0;
	pixels.allocate(w, h, GL_RGB);
	
	vidRecorder.setup(result.getPath(), w, h, frameRate);
	vidRecorder.start();
	
	glsl.setRecording(true);
	exportingStatus = exporting;
	ofSetFrameRate(MAX_FPS);
}

//--------------------------------------------------------------
void ofApp::endExport() {
	vidRecorder.close();
	exportingStatus = saving;
	glsl.setRecording(false);
	
	ofSetFrameRate(glsl.getFrameRate());
}


//--------------------------------------------------------------
// events

void ofApp::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args) {
	exportingStatus = stopped;
	glsl.resetPlay();
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
		ImGui::Separator();
		
		static stringstream ss;
		ss.str("");
		ss << "Time:" << setw(11) << glsl.getTimeText();
		
		ImGui::PushItemWidth(-1);
		ImOf::PushMonospaceLargeFont();
		ImGui::Text("%s", ss.str().c_str());
		ImGui::PopFont();
		ImGui::PopItemWidth();
		
		ImGui::Separator();
		
		static const int codecNum = codecs.size();
		
		static const char** codecLabels = [](vector<Codec> &codecs) {
			char** labels = new char*[codecs.size()];
			for (int i = 0; i < codecs.size(); i++) {
				labels[i] = new char[codecs[i].label.size() + 1];
				strcpy(labels[i], codecs[i].label.c_str());
			}
			return const_cast<const char**>(labels);
		}(codecs);
		
		ImGui::PushItemWidth(80);
		ImGui::Combo("", &selectedCodec, codecLabels, codecNum);
		
		ImGui::SameLine();
		ImGui::PushItemWidth(50);
		ImGui::DragInt("", &bitrate, 100.0f, 100.0f, 2000.0f, "%.0fk");
		ImGui::PopItemWidth();
		
		ImGui::SameLine();
		if (ImGui::Button("Export", ImVec2(-1, 0))) {
			Codec &codec = codecs[selectedCodec];
			beginExport(codec, bitrate);
		}
		
		ImGui::Separator();
		
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
	
	settings.setValue("selectedCodec", selectedCodec);
	settings.setValue("bitrate", bitrate);
	
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
