#include<string>
#include<array>
#include<vector>
#include<map>
#include"RouteSegment.h"
#include<QString>
#include<iostream>
#include<QFrame>
#include<QRegularExpression>

static const std::vector<std::map<std::array<QString, 2>, QString> > CONNECTIONS = {

	{{{"Cataris","Dairon"},			"Shuttle"}},
	{{{"Elun","Ghavoran"},			"Shuttle"}},
	{{{"Ghavoran","Hanubia"},		"Shuttle"}},
	{{{"Ghavoran","Ferenia"},		"Shuttle"}},
	{{{"Ghavoran","Burenia"},		"Elevator"}},
	{{{"Dairon","Ghavoran"},		"Elevator"}},
	{{{"Artaria","Burenia"},		"Elevator"}},
	{{{"Artaria","Cataris"},		"Elevator"}},
	{{{"Ferenia","Hanubia"},		"Elevator"}},
	{{{"Artaria","Dairon"},			"Elevator"}},
	{{{"Hanubia","Itorash"},		"Elevator"}},

	{{{"Burenia","Dairon"},			"Upper shuttle"}},
	{{{"Dairon","Burenia"},			"Lower shuttle"}},
	{{{"Ferenia","Dairon"},			"Left elevator"}},
	{{{"Dairon","Ferenia"},			"Right elevator"}},

	{{{"Artaria","Cataris"},		"Dark blue"}},
	{{{"Burenia","Ferenia"},		"Light blue"}},
	{{{"Ghavoran","Burenia"},		"Green"}},
	{{{"Ghavoran","Cataris"},		"Orange"}},
	{{{"Dairon","Cataris"},			"Purple"}},
	{{{"Cataris","Artaria"},		"Red"}},
	{{{"Artaria","Dairon"},			"Yellow"}}
};

RouteSegment::RouteSegment(QString src, QString dest, QString trsport, QFrame* button)
	:source(src), destination(dest), transport(trsport), routeSegmentFrame(button) {}


std::vector<QString> RouteSegment::getPossibleTransports(QString source, QString destination)
{
	std::vector<QString> possibleTransports;
	if (source != destination) {
		for (auto& maps : CONNECTIONS) {
			for (auto& mapEntry : maps) {
				std::array<QString, 2> areas = mapEntry.first;
				if (find(begin(areas), end(areas), source) != end(areas) and
					find(begin(areas), end(areas), destination) != end(areas)) {
					possibleTransports.push_back(mapEntry.second);
				}
			}
		}
	}
	return possibleTransports;
}

QString RouteSegment::toString()
{
	qDebug() << transport;
	return source + QString(" ") + destination + QString(" ") + transport;
}

QString RouteSegment::toRichText(){
	return "<b>" + source.replace(0,1,source[0].toUpper()) + "</b>"
		+ " to " + "<b>" + destination.replace(0, 1, destination[0].toUpper()) + "</b>"
		+ " by " + "<b>" + transport.replace(0, 1, transport[0].toUpper()) + "</b>" + ".";
}

void RouteSegment::setRouteSegmentButton(QFrame* routeSegFrame){
	routeSegmentFrame = routeSegFrame;
}

QFrame* RouteSegment::getRouteSegmentFrame()
{
	return routeSegmentFrame;
}

std::string RouteSegment::getMatchingImagePath() {
	return toString().replace(QRegularExpression("\\s"), "-").toStdString();
}

std::string RouteSegment::getTransport(){
	return transport.toLower().toStdString();
}





