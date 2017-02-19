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
		
		
		string watchPath = settings.getValue("watchPath", "");
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
			if (ImGui::Button("Reload")) {
				reloadDirectory();
			}
			
			if (watchDir.size() > 0 && selected >= 0) {
				ImGui::SameLine();
				ImGui::Button("Edit");
			
				ImGui::SameLine();
				if (ImGui::Button("Duplicate")) {
					duplicateSelected();
				}
			}
		
			const char **cNames = const_cast<const char**>(fileNames);
			
			ImGui::PushItemWidth(-1);
			if (ImGui::ListBox("", &selected, cNames, watchDir.size() == 0 ? 1 : watchDir.size(), 8)) {
				
				if (selected < watchDir.size()) {
					string path = watchDir.getPath(selected);
					ofNotifyEvent(shaderFileSelected, path, this);
				}
			}
			ImGui::PopItemWidth();
		
		ImGui::Separator();
		}
	}
	
private:
	
	void reloadDirectory() {
		setWatchDirectory(watchDir.getAbsolutePath());
	}
	
	void setWatchDirectory(string path) {
		
		watchDir.open(path);
		
		if (!watchDir.exists()) {
			ofFilePath filePath;
			watchDir.open( filePath.getUserHomeDir() );
		}
		
		watchDir.listDir();
		
		size_t numFiles = watchDir.size();
		
		if (numFiles == 0) {
			
			string empty = "(No Shader Files)";
			
			fileNames = new char*[1];
			fileNames[0] = new char[empty.size() + 1];
			strcpy(fileNames[0], empty.c_str());
			
		} else {
			fileNames = new char*[numFiles];
			
			for (int i = 0; i < numFiles; i++) {
				string name = watchDir.getName(i);
				fileNames[i] = new char[name.size() + 1];
				strcpy(fileNames[i], name.c_str());
			}
		}
	}
	
	void duplicateSelected(bool alreadyExists = false) {
		
		string newName = ofSystemTextBoxDialog(alreadyExists ? "Specified file aloready exists. Set another filename." : "Set new filename.");
		
		if (newName == "") {
			return;
		}
		
		string newPath = watchDir.getAbsolutePath() + "/" + newName + ".frag";
		ofFile duplicated(newPath);
		
		if (duplicated.exists()) {
			duplicateSelected(true);
			return;
		}
		
		// copy
		duplicated.create();
		
		ofBuffer buffer;
		ofFile original(watchDir.getPath(selected));
		buffer = original.readToBuffer();
		
		duplicated.setWriteable(true);
		duplicated.writeFromBuffer(buffer);
		
		duplicated << "test";
		
		duplicated.close();
		
		reloadDirectory();
	}
	
	void openSelected() {
		
	}
	
	stringstream	ss;
	
	int				selected = -1;
	char			**fileNames;
	
	ofDirectory		watchDir;
};
