#pragma once

#include "ofMain.h"

#include "ofxXmlSettings.h"
#include "ofxImGui.h"

#include "ImOf.h"
#include "Config.h"

#define DEFAULT_SHADER_PATH		ofToDataPath("default.frag")

class GLSLManager {
public:
	
	ofEvent<int> frameRateUpdated;
	
	void setup() {
		loadShader(DEFAULT_SHADER_PATH);
	}
	
	void loadShader(string path) {
		
		ss.str("");
		std::streambuf *old = std::cerr.rdbuf(ss.rdbuf());
		
		compileSucceed = shader.setupShaderFromFile(GL_FRAGMENT_SHADER, path);
		
		std::cerr.rdbuf(old);
		
		shader.linkProgram();
		
		if (!compileSucceed) {
			// get error
			GLuint frag = shader.getShader(GL_FRAGMENT_SHADER);
			GLsizei infoLength;
			
			ofBuffer infoBuffer;
			glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &infoLength);
			infoBuffer.allocate(infoLength);
			glGetShaderInfoLog(frag, infoLength, &infoLength, infoBuffer.getData());
			
			// remove lines inserted by ofLog
			string lines = ss.str();

			size_t pos;
			pos = lines.find(":\n");
			if (pos != string::npos) {
				lines.erase(0, pos + 2);
			}

			errorMessage = infoBuffer.getText() + "\n" + lines;
		}
		
		file.open(path);
		lastModified = filesystem::last_write_time(file);
	}
	
	
	void loadSettings(ofxXmlSettings &settings) {
		
		settings.pushTag("renderer");
		
		duration = settings.getValue("duration", duration);
		frameRate = settings.getValue("frameRate", frameRate);
		ofNotifyEvent(frameRateUpdated, frameRate, this);
		
		int w = settings.getValue("width", 512);
		int h = settings.getValue("height", 512);
		setSize(w, h);
		
		string path = settings.getValue("shaderPath", DEFAULT_SHADER_PATH);
		loadShader(path);
		
		settings.popTag();
	}
	
	void saveSettings(ofxXmlSettings &settings) {
		
		settings.addTag("renderer");
		settings.pushTag("renderer");
		
		settings.setValue("duration", duration);
		settings.setValue("frameRate", frameRate);
		
		settings.setValue("width", (int)target.getWidth());
		settings.setValue("height", (int)target.getHeight());
		
		settings.setValue("shaderPath", file.getAbsolutePath());
		
		settings.popTag();
	}
	
	void setSize(int w, int h) {
		target.allocate(w, h, GL_RGB);
		targetSize[0] = w;
		targetSize[1] = h;
	}
	
	void update() {
		
		int lm = filesystem::last_write_time(file);
		
		if (lm != lastModified) {
			loadShader(file.getAbsolutePath());
		}
		
		// update
		int frame = (int)(ofGetElapsedTimef() * frameRate) % duration;
		renderFrame(frame);
	}
	
	void draw() {
		
		ofSetColor(255);
		
		if (compileSucceed) {
			ofPushMatrix();
			{
				float screenW = ofGetWidth() - GUI_WIDTH;
				float screenH = ofGetHeight();
				
				float w = target.getWidth();
				float h = target.getHeight();
				
				float sx = screenW / w;
				float sy = screenH / h;
				
				float s = min(sx, sy);
				
				float tx = (screenW - w * s) * .5;
				float ty = (screenH - h * s) * .5;
				
				ofTranslate(GUI_WIDTH + tx, ty + h * s);
				
				ofScale(1, -1);
				target.draw(0, 0, w * s, h * s);
			}
			ofPopMatrix();
		
		}
	}
	
	void drawImGui() {
	
		static bool isOpen = true;
		
		ImGui::SetNextTreeNodeOpen(isOpen);
		
		if ((isOpen = ImGui::CollapsingHeader("Renderer"))) {
			
			// progress bar
			float progress = (float)lastRenderedFrame / duration;
			float time = (float)lastRenderedFrame / frameRate;
			static char timeOverlay[512];
			sprintf(timeOverlay, "Current: %.1fs", time);
			ImGui::ProgressBar(progress, ImVec2(-1, 0), timeOverlay);
			
			ImGui::Separator();
			
			// render settings
			ImGui::DragInt("Duration", &duration, 1.0f, 1, 9000, "%.0fF");
			
			if (ImGui::SliderInt("Frame Rate", &frameRate, 8, 90)) {
				ofNotifyEvent(frameRateUpdated, frameRate, this);
			}
			
			ImGui::DragInt2("", targetSize, 1.0f, 4, 4096);
			ImGui::SameLine();
			
			if (target.getWidth() != targetSize[0] || target.getHeight() != targetSize[1]) {
				if (ImGui::Button("Update Size")) {
					setSize(targetSize[0], targetSize[1]);
				}
			} else {
				ImGui::Text("Size");
			}
			
			ImGui::Separator();
		}
		
		if (!compileSucceed) {
			
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
			ImOf::PushMonospaceFont();
			
			ImGui::SetNextWindowPos(ImVec2(GUI_WIDTH, 0));
			ImGui::SetNextWindowSize(ImVec2(ofGetWidth() - GUI_WIDTH, ofGetHeight()));
			ImGui::Begin("", NULL, ImVec2(0,0), -1.0f, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
			{
				ImGui::Text("%s", errorMessage.c_str());
			}
			ImGui::End();
			
			ImGui::PopFont();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
		}
	}
	
	float getWidth()	{ return target.getWidth(); }
	float getHeight()	{ return target.getHeight(); }
	int getFrameRate()	{ return frameRate; }
	int getDuration()	{ return duration; }
	
	void readToPixelsAtFrame(int frame, ofPixels &pixels) {
		renderFrame(frame);
		target.getTexture().readToPixels(pixels);
	}
	
private:
	
	void renderFrame(int frame) {
		
		float time = (float)frame / frameRate;
		
		target.begin();
		{
			ofBackground(0);
			ofSetColor(255);
			
			shader.begin();
			shader.setUniform1f("u_time", time);
			shader.setUniform2f("u_resolution", target.getWidth(), target.getHeight());
			
			ofDrawRectangle(0, 0, target.getWidth(), target.getHeight());
			
			shader.end();
		}
		target.end();
		
		lastRenderedFrame = frame;
	}
	
	
	stringstream	ss;
	string			errorMessage;
	
	int				duration = 120;
	int				frameRate = 30;
	
	int				lastRenderedFrame = 0;
	
	bool			compileSucceed;
	
	int				targetSize[2];
	
	int				lastModified;
	
	ofFile			file;
	
	ofShader		shader;
	ofFbo			target;
	
};
