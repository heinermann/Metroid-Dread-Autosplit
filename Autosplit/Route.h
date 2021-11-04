#pragma once

#include"SplitImage.h"
#include"RouteSegment.h"
#include<vector>

class Route {
private:
	std::vector<RouteSegment> segments;
	SplitImage getBlackSplitImage();
	SplitImage getNonBlackSplitImage();
public:
	Route();
	QString toString();
	void insertSegment(RouteSegment segment, int index);
	void addSegment(RouteSegment segment);
	void removeSegment(int index);
	void clear();
	std::vector<RouteSegment> getSegments();
	int getIndexFromRouteSegmentFrame(QFrame* button);
	std::vector<SplitImage> routeToSplitImages();
};
