#pragma once

#include<vector>
#include<string>
#include<array>
#include<unordered_map>
#include<QString>
#include<QFrame>


class RouteSegment {
	/*const std::array<std::string, 8> AREAS{
		"Artaria","Burenia","Cataris","Dairon","Elun","Furenia","Ghavoran","Hanubia"
	};
	const std::array<std::string, 12> TRANSPORTS{ 
		"Orange", "Red", "Purple", "Dark blue", "Yellow", "Light blue", "Green",
		"Shuttle", "Upper shuttle", "Lower shuttle", "Left shuttle", "Right shuttle"
	};
	*/
	
	
private:
	QString source;
	QString destination;
	QString transport;
	QFrame* routeSegmentFrame;
public:
	RouteSegment(QString source, QString destination, QString transport, QFrame* button);
	static std::vector<QString> getPossibleTransports(QString source, QString destination);
	QString toString();
	QString toRichText();
	void setRouteSegmentButton(QFrame* routeSegmentFrame);
	QFrame* getRouteSegmentFrame();
	std::string getMatchingImagePath();
	std::string getTransport();
};