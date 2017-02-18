//
//
////--------------------------------------------------------------
//void ofShader::checkShaderInfoLog(GLuint shader, GLenum type, ofLogLevel logLevel) {
//	GLsizei infoLength;
//	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);
//	if (infoLength > 1) {
//		ofBuffer infoBuffer;
//		infoBuffer.allocate(infoLength);
//		glGetShaderInfoLog(shader, infoLength, &infoLength, infoBuffer.getData());
//		ofLog(logLevel, "ofShader: %s shader reports:\n%s", nameForType(type).c_str(), infoBuffer.getText().c_str());
//#if (!defined(TARGET_LINUX) || defined(GCC_HAS_REGEX))
//		if (shaders.find(type) != shaders.end()) {
//			// The following regexp should match shader compiler error messages by Nvidia and ATI.
//			// Unfortunately, each vendor's driver formats error messages slightly different.
//			std::regex nvidia_ati("^.*[(:]{1}(\\d+)[:)]{1}.*");
//			std::regex intel("^[0-9]+:([0-9]+)\\([0-9]+\\):.*$");
//			std::smatch matches;
//			string infoString = ofTrim(infoBuffer);
//			if (std::regex_search(infoString, matches, intel) || std::regex_search(infoString, matches, nvidia_ati)){
//				ofBuffer buf;
//				buf.set(shaders[type].source.expandedSource);
//				ofBuffer::Line line = buf.getLines().begin();
//				int  offendingLineNumber = ofToInt(matches[1]);
//				ostringstream msg;
//				msg << "ofShader: " + nameForType(type) + ", offending line " << offendingLineNumber << " :"<< endl;
//				for(int i=0; line != buf.getLines().end(); line++, i++ ){
//					string s = *line;
//					if ( i >= offendingLineNumber -3 && i < offendingLineNumber + 2 ){
//						msg << "\t" << setw(5) << (i+1) << "\t" << s << endl;
//					}
//				}
//				ofLog(logLevel) << msg.str();
//			}else{
//				ofLog(logLevel) << shaders[type].source.expandedSource;
//			}
//		}
//#endif
//	}
//}
