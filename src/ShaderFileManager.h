#pragma once

#include "ofMain.h"

#include "ofxXmlSettings.h"
#include "ofxImGui.h"

#include "ImOf.h"
#include "Config.h"

class ShaderFileManager {
public:
	
	ofEvent<string> shaderFileSelected;
	
	void setup() {
		watchDir.allowExt("frag");
		watchDir.allowExt("fs");
	}
	
	
	void loadSettings(ofxXmlSettings &settings) {
		
		settings.pushTag("shaderFile");
		
		ofFilePath filePath;
		string watchPath = settings.getValue("watchPath", filePath.getUserHomeDir());
		setWatchDirectory(watchPath);
		
		settings.popTag();
	}
	
	void saveSettings(ofxXmlSettings &settings) {
		
		settings.addTag("shaderFile");
		settings.pushTag("shaderFile");
		
		settings.setValue("watchPath", watchDir.getAbsolutePath());
		
		settings.popTag();
	}
	
	void update() {
		
	}
	
	void draw() {
		
	}
	
	void drawImGui() {
		
		static bool isOpen = true;
		
		ImGui::SetNextTreeNodeOpen(isOpen);
		
		if ((isOpen = ImGui::CollapsingHeader("File"))) {
			
			if (ImGui::Button("Set Folder")) {
				ofFileDialogResult result = ofSystemLoadDialog("Set Folder", true);
				if (result.bSuccess) {
					setWatchDirectory(result.getPath());
				}
			}
			
			ImGui::SameLine();
			ImGui::Text("%s", watchDir.getAbsolutePath().c_str());
			
			{
				static int index = 1;
				const char **cNames = const_cast<const char**>(fileNames);
				
				ImGui::PushItemWidth(-1);
				if (ImGui::ListBox("", &index, cNames, IM_ARRAYSIZE(cNames), 8)) {
					string path = watchDir.getPath(index);
					
					ofNotifyEvent(shaderFileSelected, path, this);
				}
				ImGui::PopItemWidth();
			}
			
			ImGui::Separator();
		}
	}
	
private:
	
	void setWatchDirectory(string path) {
		watchDir.open(path);
		watchDir.listDir();
		
		// show shader files
		size_t numFiles = watchDir.size();
		fileNames = new char*[numFiles];
		
		for (int i = 0; i < numFiles; i++) {
			string name = watchDir.getName(i);
			fileNames[i] = new char[name.size() + 1];
			strcpy(fileNames[i], name.c_str());
		}
	}
	
	char			**fileNames;
	
	ofDirectory		watchDir;
};
