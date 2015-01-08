/*!
 * \file
 * \brief
 * \author Anna Wujek
 */

#include <memory>
#include <string>

#include "FindCorners.hpp"
#include "Common/Logger.hpp"

#include <boost/bind.hpp>
#include <math.h>

namespace Processors {
namespace FindCorners {

FindCorners::FindCorners(const std::string & name) :
		Base::Component(name)  {

}

FindCorners::~FindCorners() {
}

void FindCorners::prepareInterface() {
	// Register data streams, events and event handlers HERE!
	registerStream("in_lines", &in_lines);
	registerStream("out_corners", &out_corners);
	// Register handlers
	h_FindIntersection.setup(boost::bind(&FindCorners::FindIntersection, this));
	registerHandler("FindIntersection", &h_FindIntersection);
	addDependency("FindIntersection", &in_lines);

}

bool FindCorners::onInit() {

	return true;
}

bool FindCorners::onFinish() {
	return true;
}

bool FindCorners::onStop() {
	return true;
}

bool FindCorners::onStart() {
	return true;
}

cv::Point2f computeIntersect(cv::Vec4i a, cv::Vec4i b) {
    int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3];
    int x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];

    if (float d = ((float)(x1-x2) * (y3-y4)) - ((y1-y2) * (x3-x4))) {
        cv::Point2f pt;
        pt.x = ((x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4)) / d;
        pt.y = ((x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4)) / d;
        return pt;
    }
    else {
        return cv::Point2f(-1, -1);
    }
}

struct myComp {
    bool operator() (cv::Point2f a, cv::Point2f b) {
        //if(abs(a.x - b.x) > 10) return a.x < b.x;
        //else return a.y < b.y;
        return (a.x*a.x + a.y*a.y) < (b.x*b.x + b.y*b.y);

    }
} myComp;


void FindCorners::FindIntersection() {
    std::vector<cv::Point2f> corners;
    std::vector<cv::Vec4i> lines = in_lines.read();

    for(int i = 0; i < lines.size(); ++i) {
        for(int j = i+1; j < lines.size(); ++j) {
            cv::Point2f pt = computeIntersect(lines[i], lines[j]);
            if(pt.x >= 0 && pt.y >= 0 && pt.x < 1296 && pt.y < 1032) {
                corners.push_back(pt);
            }
        }
    }
    if(corners.size() == 4) {
        std::sort(corners.begin(), corners.end(), myComp);
        //std::cout<<corners.size()<<std::endl;

        out_corners.write(corners);
        std::cout<<"\ncorners: "<<corners.size()<<"\n";
    }



}



} //: namespace FindCorners
} //: namespace Processors
