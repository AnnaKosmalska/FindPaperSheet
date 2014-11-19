/*!
 * \file
 * \brief
 */

#include <memory>
#include <string>

#include "CvHoughLines.hpp"
#include "Common/Logger.hpp"

#include <opencv2/imgproc/imgproc.hpp>

#include "Types/DrawableContainer.hpp"
#include "Types/Line.hpp"

namespace Processors {
namespace CvHoughLines {

using namespace cv;
using namespace std;

CvHoughLines_Processor::CvHoughLines_Processor(const std::string & name) :
	Base::Component(name),
	threshold("threshold", 80, "range"),
	minLineLength("minLineLength", 30, "range"),
	maxLineGap("maxLineGap", 10, "range")
{
	threshold.addConstraint("1");
	threshold.addConstraint("1000");
	registerProperty(threshold);

	minLineLength.addConstraint("0");
	minLineLength.addConstraint("1000");
	registerProperty(minLineLength);

	maxLineGap.addConstraint("0");
	maxLineGap.addConstraint("100");
	registerProperty(maxLineGap);
}

CvHoughLines_Processor::~CvHoughLines_Processor()
{
	LOG(LTRACE) << "Good bye CvHoughLines_Processor\n";
}

void CvHoughLines_Processor::prepareInterface() {
	// Register data streams, events and event handlers HERE!

	registerHandler("onNewImage", boost::bind(&CvHoughLines_Processor::onNewImage, this));
	addDependency("onNewImage", &in_img);

	registerStream("in_img", &in_img);
	registerStream("out_lines", &out_lines);
	registerStream("out_linesDrawable", &out_linesDrawable);
}

bool CvHoughLines_Processor::onInit()
{
	LOG(LTRACE) << "CvHoughLines_Processor::initialize\n";

	return true;
}

bool CvHoughLines_Processor::onFinish()
{
	return true;
}

bool CvHoughLines_Processor::onStop()
{
	return true;
}

bool CvHoughLines_Processor::onStart()
{
	return true;
}

void CvHoughLines_Processor::onNewImage()
{
	if (in_img.empty()) {
		LOG(LFATAL) << "Component " << name() << " " << "in_img input stream is empty.";
		return;
	}
	Mat image = in_img.read();

	if (image.channels() != 1) {
		LOG(LFATAL) << "Component " << name() << " " << "in_img received image must have only one channel.";
		return;
	}

	Types::DrawableContainer c;
	vector<Vec4i> lines;
	cv::HoughLinesP( image, lines, 1, CV_PI/180, threshold, minLineLength, maxLineGap);
	CLOG(LDEBUG) << "Found " << lines.size() << " lines";
	// Needed for visualization only
    for (int i = 0; i < lines.size(); i++)
    {
        cv::Vec4i v = lines[i];
        lines[i][0] = 0;
        lines[i][1] = ((float)v[1] - v[3]) / (v[0] - v[2]) * -v[0] + v[1];
        lines[i][2] = image.cols;
        lines[i][3] = ((float)v[1] - v[3]) / (v[0] - v[2]) * (image.cols - v[2]) + v[3];
    }
	for( size_t i = 0; i < lines.size(); i++ )
	{
		c.add(new Types::Line(cv::Point(lines[i][0], lines[i][1]), cv::Point(lines[i][2], lines[i][3])));
	}
	out_linesDrawable.write(c);
	out_lines.write(lines);
}

}//: namespace CvHoughLines
}//: namespace Processors