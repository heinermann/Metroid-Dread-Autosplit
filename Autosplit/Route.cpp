#include "Route.h"


//Mainly used to check if a split image needs a teleportal mask. 
//Lowercase because the paths to split images are lowercase.
const std::vector<std::string> TELEPORTALS = {
	"dark blue","light blue","green","orange","purple","red","yellow"
};



const SplitImage BLACK_IMAGE{
	.name = "",
	.imageType = IMAGE_TYPE(BLACK),
	.mask = ""
};

SplitImage Route::getBlackSplitImage()
{
	SplitImage blackImage{
	.name = "",
	.imageType = IMAGE_TYPE(BLACK),
	.mask = "", 
	};

	return blackImage;
}

SplitImage Route::getNonBlackSplitImage()
{
	SplitImage nonBlackImage{
	.name = "",
	.imageType = IMAGE_TYPE(NON_BLACK),
	.mask = ""
	};

	return nonBlackImage;
}

Route::Route()
	: segments() {}

QString Route::toString(){
	QString ret;
	for (RouteSegment segment : segments) {
		ret.append(segment.toString()).append(QString("\n"));
	}
	return ret;
}

void Route::insertSegment(RouteSegment segment, int index){
	segments.insert(segments.begin()+index, segment);
}
void Route::addSegment(RouteSegment segment) {
	segments.push_back(segment);
}
void Route::removeSegment(int index) {
	delete segments[index].getRouteSegmentFrame();
	segments.erase(segments.begin() + index);
}
void Route::clear() {
	segments.clear();
}

std::vector<RouteSegment> Route::getSegments() {
	return segments;
}

int Route::getIndexFromRouteSegmentFrame(QFrame* frame) {

	for (int i{ 0 }; i < segments.size(); ++i) {
		RouteSegment segment = segments[i];
		if (segment.getRouteSegmentFrame() == frame) {
			return i;
		}
	}
	return -1;
}

std::vector<SplitImage> Route::routeToSplitImages() {
	//A series of pauses for an image looks like this:
	//Check split image -> pause next black image -> check for next black image -> pause first non-black image
	//Each split image (grabbed from a route segment object) should be converted to this series of 4 images.

	std::vector<SplitImage> images;

	//set beginning
	SplitImage firstBlackImage = getBlackSplitImage();
	firstBlackImage.cooldown = 4000;
	firstBlackImage.hotkey = HOTKEY_TYPE(NONE);

	SplitImage secondBlackImage = getBlackSplitImage();
	secondBlackImage.cooldown = 0;
	firstBlackImage.hotkey = HOTKEY_TYPE(NONE);

	SplitImage nonBlackImage = getNonBlackSplitImage();
	nonBlackImage.cooldown = 15000;
	nonBlackImage.hotkey = HOTKEY_TYPE(SPLIT);

	images.push_back(firstBlackImage);
	images.push_back(secondBlackImage);
	images.push_back(nonBlackImage);

	for (RouteSegment segment : segments) {
		SplitImage dummyImage{
			.name = segment.getMatchingImagePath(),
			.imageType = IMAGE_TYPE(COLOR),
			.hotkey = HOTKEY_TYPE(NONE),
			.cooldown = 0,
		};

		if (std::find(TELEPORTALS.begin(), TELEPORTALS.end(), segment.getTransport()) != TELEPORTALS.end()) {
			dummyImage.mask = "teleportal.png";
		}
		else if(dummyImage.name == "hanubia-itorash-elevator" || dummyImage.name == "itorash-hanubia-elevator") {
			dummyImage.mask = "hanubia-itorash-elevator.png";
		}

		SplitImage firstBlackImage = getBlackSplitImage();
		firstBlackImage.cooldown = 4000;
		firstBlackImage.hotkey = HOTKEY_TYPE(PAUSE);

		SplitImage secondBlackImage = getBlackSplitImage();
		secondBlackImage.cooldown = 0;
		secondBlackImage.hotkey = HOTKEY_TYPE(NONE);

		SplitImage nonBlackImage = getNonBlackSplitImage();
		nonBlackImage.cooldown = 15000;
		nonBlackImage.hotkey = HOTKEY_TYPE(PAUSE);

		images.push_back(dummyImage);
		images.push_back(firstBlackImage);
		images.push_back(secondBlackImage);
		images.push_back(nonBlackImage);
	}

	return images;
}


