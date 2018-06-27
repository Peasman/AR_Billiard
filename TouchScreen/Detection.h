#ifndef DETECTION_H
#define DETECTION_H

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

	double _curr_x_1, _curr_y_1, _last_x_1, _last_y_1;
	double _curr_x_2, _curr_y_2, _last_x_2, _last_y_2;
	cv::Scalar _thresholdsLow, _thresholdsHigh;
	bool _valid = false;


	void detectCue(cv::Mat img);
	void runTest();
	
private:
	std::vector<int> analysePoints(std::vector<cv::Point> &);
	std::string _windowName;
};

#endif