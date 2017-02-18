#pragma once

#include "ofMain.h"

#include "ofxXmlSettings.h"
#include "ofxImGui.h"

class GLSLRenderer {
public:
	
	void setup() {
		loadShader( ofToDataPath("test-shader.frag") );
	}
	
	void loadShader(string path) {
		shader.setupShaderFromFile(GL_FRAGMENT_SHADER, path);
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
		ofLogNotice() << w << " " << h;
		target.allocate(w, h, GL_RGB);
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
	
	void draw(float x, float y, float w, float h) {
		
		ofPushMatrix();
		ofScale(1, -1);
		ofTranslate(0, -h);
		
		target.draw(x, y, w, h);
		
		ofPopMatrix();
	}
	
	void draw(float x, float y) {
		draw(x, y, target.getWidth(), target.getHeight());
	}
	
	void drawImGui() {
		
		if (ImGui::CollapsingHeader("Renderer")) {
			
			static int w = target.getWidth(), h = target.getHeight();
			ImGui::InputInt("Width", &w);
			ImGui::InputInt("Height", &h);
			
			if (ImGui::Button("Set Size")) {
				setSize(w, h);
			}
			
			ImGui::Separator();
			
			if (ImGui::Button("Reload Shader")) {
				
			}
			
		}
	}
	
	float getWidth() { return target.getWidth(); }
	float getHeight() { return target.getHeight(); }

	
private:

	int				lastModified;
	
	ofFile			file;
	
	ofShader		shader;
	ofFbo			target;
	
};
