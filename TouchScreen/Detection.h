#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <stdio.h>
#include <iostream>


class Detection
{
public:
	Detection();
	~Detection();

	struct DetectedCue{
		float pos_x;
		float pos_y;
		float dir_x;
		float dir_y;
	};
	DetectedCue detectCue(cv::Mat img);
	void runTest();
	void drawAxis(cv::Mat& img, cv::Point p, cv::Point q, cv::Scalar colour);
	double getOrientation(std::vector<cv::Point> &, cv::Mat&);
private:
	std::string _windowName;
};

