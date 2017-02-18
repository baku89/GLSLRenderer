#pragma once

#include "ofMain.h"

class Common {
public:
	Common() {
		font.load("Karla-Regular.ttf", 9);
		font.setLetterSpacing(1.1);

	}

	static ofTrueTypeFont font;
};


Common c;
