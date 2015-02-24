/*!
 * \file
 * \brief
 * \author Anna Wujek
 */

#include <memory>
#include <string>

#include "ApplyCornerMask.hpp"
#include "Common/Logger.hpp"

#include <boost/bind.hpp>

namespace Processors {
namespace ApplyCornerMask {

ApplyCornerMask::ApplyCornerMask(const std::string & name) :
		Base::Component(name)  {

}

ApplyCornerMask::~ApplyCornerMask() {
}

void ApplyCornerMask::prepareInterface() {
	// Register data streams, events and event handlers HERE!
	registerStream("in_img", &in_img);
	registerStream("in_corners", &in_corners);
	registerStream("out_img", &out_img);
	// Register handlers
	h_applyMask.setup(boost::bind(&ApplyCornerMask::applyMask, this));
	registerHandler("applyMask", &h_applyMask);
	addDependency("applyMask", &in_img);
	addDependency("applyMask", &in_corners);

}

bool ApplyCornerMask::onInit() {

	return true;
}

bool ApplyCornerMask::onFinish() {
	return true;
}

bool ApplyCornerMask::onStop() {
	return true;
}

bool ApplyCornerMask::onStart() {
	return true;
}

void ApplyCornerMask::applyMask() {
    cv::Mat img = in_img.read().clone();
    std::vector<cv::Point2f> corners = in_corners.read();

    if(corners.size() == 4) {
        cv::Mat mask(img.size(), CV_8UC1, cv::Scalar::all(0));

        cv::Point points[1][4];

        points[0][0] = cv::Point(corners[0].x, corners[0].y);
        points[0][1] = cv::Point(corners[1].x, corners[1].y);
        points[0][2] = cv::Point(corners[3].x, corners[3].y);
        points[0][3] = cv::Point(corners[2].x, corners[2].y);


        const cv::Point* ppt[1] = { points[0] };
        int npt[] = { 4 };

        cv::fillPoly(mask, ppt, npt, 1, cv::Scalar(255,255,255), 8);
        cv::Mat new_img;
        img.copyTo(new_img, mask);

        out_img.write(new_img);
    }






}



} //: namespace ApplyCornerMask
} //: namespace Processors