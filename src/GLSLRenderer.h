#pragma once

#include "ofMain.h"

#include "ofxXmlSettings.h"
#include "ofxImGui.h"

#include "Config.h"
#include "StdCapture.h"

class GLSLRenderer {
public:
	
	void setup() {
		loadShader( ofToDataPath("test-shader.frag") );
	}
	
	void loadShader(string path) {
		
		StdCapture capture;
		
		capture.BeginCapture();
		
		compileSucceed = shader.setupShaderFromFile(GL_FRAGMENT_SHADER, path);
		
		capture.EndCapture();
		
		if (!compileSucceed) {
			ofLogNotice() << "failed";
			errorMessage = capture.GetCapture();
		}
		
		
		shader.linkProgram();
		
		file.open(path);
	}
	
	
	void loadSettings(ofxXmlSettings &settings) {
		
		settings.pushTag("renderer");
		
		int w = settings.getValue("width", 512);
		int h = settings.getValue("height", 512);
		setSize(w, h);
		
		settings.popTag();
	}
	
	void saveSettings(ofxXmlSettings &settings) {
		
		settings.addTag("renderer");
		settings.pushTag("renderer");
		
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
			ofLogNotice() << "reload!";
			lastModified = lm;
			
			loadShader(file.getAbsolutePath());
		}
	}
	
	void render(float time) {
		
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
//			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2);
//			ImGui::Begin("", NULL, ImVec2(0,0), -1.0f, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
//			{
				ImGui::Text("ERROR=%s", errorMessage.c_str());
//			}
//			ImGui::End();
//			ImGui::PopStyleVar();
		}
	}
	
	float getWidth()	{ return target.getWidth(); }
	float getHeight()	{ return target.getHeight(); }
	
	void readToPixels(ofPixels &pixels) {
		target.getTexture().readToPixels(pixels);
	}
	
private:
	
	stringstream	ss;
	string			errorMessage;
	
	bool			compileSucceed;
	
	int				targetSize[2];
	
	int				lastModified;
	
	ofFile			file;
	
	ofShader		shader;
	ofFbo			target;
	
};
