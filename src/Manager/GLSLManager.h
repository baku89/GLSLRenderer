#pragma once

#include "ofMain.h"

#include "ofxXmlSettings.h"
#include "ofxImGui.h"

#include "ImOf.h"
#include "Config.h"
#include "BaseManager.h"

#define DEFAULT_SHADER_PATH		ofToDataPath("default.frag")
#define SEEKBAR_WIDTH			600
#define SEEKBAR_HEIGHT			41
#define SEEKBAR_MARGIN			30

#define SEEKBAR_PLAY_WIDTH		30
#define SEEKBAR_TIME_WIDTH		60

#define RELOAD_DISPLAY_DURATION	0.5

#define REC_COLOR				0xDD4444FF


enum TimeDisplayMode {
	TIMECODE,
	FRAMES
};

class GLSLManager : public BaseManager {
public:
	
	ofEvent<int>	frameRateUpdated;
	
	void setup() {
		loadShader(DEFAULT_SHADER_PATH);
		
		ofAddListener(ofEvents().keyPressed, this, &GLSLManager::keyPressed);
	}
	
	void loadShader(string path) {
		
		file.open(path);
		
		if (!file.exists()) {
			compileSucceed = false;
			errorMessage = "File does not exist";
			return;
		}
		
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

			static size_t pos;
			pos = lines.find(":\n");
			if (pos != string::npos) {
				lines.erase(0, pos + 2);
			}

			errorMessage = infoBuffer.getText() + "\n" + lines;
		}
			
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
		renderFbo.allocate(w, h, GL_RGB);
		targetSize[0] = w;
		targetSize[1] = h;
	}
	
	void resetPlay() {
		currentTime = 0.0001f;
	}
	
	void setRecording(bool flag) {
		isRecording = flag;
	}
	
	void update() {
		
		if (file.exists()) {
			static int lm;
			lm = filesystem::last_write_time(file);
			
			if (lm != lastModified) {
				reloadShader();
			}
		}
		
		// update
		static float elapsedTime = ofGetElapsedTimef();
		static float prevElapsedTime = elapsedTime;
		static float deltaTime = 0;
		
		elapsedTime = ofGetElapsedTimef();
		deltaTime = elapsedTime - prevElapsedTime;
		prevElapsedTime = elapsedTime;
		
		// frame
		if (isPlaying && !isRecording) {
			currentTime = fmod(currentTime + deltaTime, (float)duration / frameRate);
		}
		
		renderFrame(currentTime * frameRate);
		
		// reload display
		remainingReloadDisplayTime = std::max(0.0f, remainingReloadDisplayTime - deltaTime);
		
		// update timecode
		if (timeDisplayMode == TIMECODE) {
			static int seconds, minutes;
			seconds = lastRenderedFrame / frameRate;
			minutes = seconds / 60;
			sprintf(timeText, "%02d:%02d", minutes, seconds);
		} else {
			sprintf(timeText, "%dF", lastRenderedFrame);
		}
	}
	
	void draw() {
		
		ofSetColor(255);
		
		if (compileSucceed) {
			ofPushMatrix();
			{
				static float screenW, screenH, w, h, sx, sy, s, tx, ty, fw, fh;
				
				screenW = ofGetWidth() - GUI_WIDTH;
				screenH = ofGetHeight();
				
				w = target.getWidth();
				h = target.getHeight();
				
				sx = screenW / w;
				sy = screenH / h;
				
				s = min(sx, sy);
				
				tx = (screenW - w * s) * .5;
				ty = (screenH - h * s) * .5;
				
				fw = w * s;
				fh = h * s;
				
				ofTranslate(GUI_WIDTH + tx, ty + h * s);
				
				ofScale(1, -1);
				target.draw(0, 0, fw, fh);
				
				if (remainingReloadDisplayTime > 0 || isRecording) {
					ofPushStyle();
					
					if (isRecording) {
						ofSetHexColor(REC_COLOR >> 8);
					} else {
						ofSetColor(255, 255 * (remainingReloadDisplayTime / RELOAD_DISPLAY_DURATION));
					}
					
					const float l = 6;
					
					ofDrawRectangle(0, 0, fw - l, l);
					ofDrawRectangle(fw - l, 0, l, fh - l);
					ofDrawRectangle(l, fh - l, fw - l, l);
					ofDrawRectangle(0, l, l, fh - l);
					
					ofPopStyle();
				}
			}
			ofPopMatrix();
		
		}  
	}
	
	char* getTimeText() { return timeText; }
	
	void drawImGui() {
	
		static bool isOpen = true;
		
		ImGui::SetNextTreeNodeOpen(isOpen);
		
		if ((isOpen = ImGui::CollapsingHeader("Renderer"))) {
			
			if (ImGui::Button(file.getFileName().c_str(), ImVec2(-1, 30))) {
				#ifdef TARGET_OSX
				ofSystem("open " + file.getAbsolutePath());
				#endif
			}
			
			// render settings
			ImGui::PushItemWidth(-100);
			ImGui::DragInt("Duration", &duration, 1.0f, 1, 9000, "%.0fF");
			
			if (ImGui::SliderInt("Frame Rate", &frameRate, 8, 60)) {
				ofNotifyEvent(frameRateUpdated, frameRate, this);
			}
			
			ImGui::DragInt2("", targetSize, 1.0f, 4, 4096);
			ImGui::SameLine();
			
			if (target.getWidth() != targetSize[0] || target.getHeight() != targetSize[1]) {
				if (ImGui::Button("Update Size", ImVec2(-1, 0))) {
					setSize(targetSize[0], targetSize[1]);
				}
			} else {
				ImGui::Text("Size");
			}
			
			ImGui::PopItemWidth();
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
		
		// playbar
		static ofRectangle shaderArea(GUI_WIDTH, 0, 0, 0);
		shaderArea.width = ofGetWidth();
		shaderArea.height = ofGetHeight();
		static bool mouseOnCanvas;
		mouseOnCanvas = shaderArea.inside(ofGetMouseX(), ofGetMouseY());
		
		if (compileSucceed && (isRecording || mouseOnCanvas)) {
			float ww = min(ofGetWidth() - GUI_WIDTH - SEEKBAR_MARGIN * 2, SEEKBAR_WIDTH);
			
			ImVec2 pos( (GUI_WIDTH + ofGetWidth()) / 2.0f - ww / 2.0f, ofGetHeight() - SEEKBAR_HEIGHT - SEEKBAR_MARGIN);
			ImVec2 size(ww, SEEKBAR_HEIGHT);
			ImGui::SetNextWindowPos(pos);
			ImGui::SetNextWindowSize(size);
			
			ImGuiStyle& style = ImGui::GetStyle();
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.13f, 0.14f, 0.17f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			
			const float	prevItemSpacingX = style.ItemSpacing.x;
			style.ItemSpacing.x = 16;
			
			ImGui::Begin("", NULL, ImVec2(0,0), -1.0f, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
			{
				// toggle play/pause
				if (isRecording) {
					static ImVec2 pos, itemSize;
					pos = ImGui::GetCursorScreenPos();
					itemSize = ImOf::CalcItemSize(ImVec2(SEEKBAR_PLAY_WIDTH, -1));
					
					ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(pos.x + itemSize.x / 2, pos.y + itemSize.y / 2), 7.0f, REC_COLOR);
					ImGui::InvisibleButton("###Rec",ImVec2(SEEKBAR_PLAY_WIDTH, -1));
					
				} else {
					ImOf::PlayToggle("###PlayToggle", &isPlaying, ImVec2(SEEKBAR_PLAY_WIDTH, -1));
				}
				
				// seek bar
				static int frame = 0;
				frame = lastRenderedFrame;
				
				ImGui::SameLine();
				if (ImOf::Seekbar("###Seekbar", &frame, 0, duration - 1, ImVec2(-SEEKBAR_TIME_WIDTH, -1)) && !isRecording) {
					isPlaying = false;
					currentTime = (float)frame / frameRate;
				}
				
				// timecode
				static char timeDisplay[128];
				sprintf(timeDisplay, "%s###TimeDisplay", timeText);
				
				ImGui::SameLine();
				if (ImGui::Button(timeDisplay, ImVec2(-1, -1))) {
					timeDisplayMode = timeDisplayMode == TIMECODE ? FRAMES : TIMECODE;
				}
			
			}
			style.ItemSpacing.x = prevItemSpacingX;
			
			ImGui::End();
			ImGui::PopStyleColor(); ImGui::PopStyleColor();
			ImGui::PopStyleVar();
		}
	}
	
	float getWidth()	{ return target.getWidth(); }
	float getHeight()	{ return target.getHeight(); }
	int getFrameRate()	{ return frameRate; }
	int getDuration()	{ return duration; }
	
	void readToPixelsAtFrame(int frame, ofPixels &pixels) {
		renderFrame(frame);
		
		// fix vertical flip
		renderFbo.begin();
		ofPushMatrix();
		{
			ofBackground(0);
			ofSetColor(255);
			
			ofTranslate(0, target.getHeight());
			ofScale(1, -1);
			target.draw(0, 0);
		}
		ofPopMatrix();
		renderFbo.end();
		
		renderFbo.readToPixels(pixels);
	}
	
private:
	
	void renderFrame(int frame) {
		
		static float time;
		time = (float)frame / frameRate;
		
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
	
	void reloadShader() {
		remainingReloadDisplayTime = RELOAD_DISPLAY_DURATION;
		loadShader(file.getAbsolutePath());
	}
	
	void keyPressed(ofKeyEventArgs & args) {
		
		if (isRecording) {
			return;
		}
		
		float frameDuration = 1.0f / frameRate;
		
		switch (args.key) {
			case ' ':
				isPlaying = !isPlaying;
				break;
			case 'r':
				reloadShader();
				break;
			case OF_KEY_LEFT:
				isPlaying = false;
				currentTime = max((float)currentTime - frameDuration, 0.0f);
				break;
			case OF_KEY_RIGHT:
				isPlaying = false;
				currentTime = min(currentTime + frameDuration, (float)(duration - 1) / frameRate);
				break;
		}
	}
	
	ofFbo			renderFbo; // to fix vertical flip when rendering
	
	stringstream	ss;
	string			errorMessage;
	
	char			timeText[128];
	
	float			remainingReloadDisplayTime = 0.0f;
	
	int				duration = 120;
	int				frameRate = 30;
	
	float			currentTime = 0;
	
	int				lastRenderedFrame = 0;
	
	bool			compileSucceed;
	TimeDisplayMode	timeDisplayMode = FRAMES;
	bool			isPlaying = true;
	bool			isRecording = false;
	
	int				targetSize[2];
	
	int				lastModified;
	
	ofFile			file;
	
	ofShader		shader;
	ofFbo			target;
	
};
