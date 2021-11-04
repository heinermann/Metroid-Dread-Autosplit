#pragma once

#include<string>

enum IMAGE_TYPE { COLOR, BLACK, NON_BLACK };
enum HOTKEY_TYPE { NONE, SPLIT, PAUSE };

struct SplitImage {
	std::string name;
	IMAGE_TYPE imageType;
	HOTKEY_TYPE hotkey;
	std::string mask;
	int cooldown;
};