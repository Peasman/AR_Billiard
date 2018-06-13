#include "Detection.h"

using namespace cv;
using namespace std;

Detection::Detection()
	:
	_windowName("Test")
{}


Detection::DetectedCue Detection::detectCue(cv::Mat imageOriginal){
	
	DetectedCue p = {
		0.0f, 0.0f, 0.0f, 0.0f
	};
	return p;
}

void Detection::runTest(){
	int waitKeyValue = 10; // 60 Hz

	cv::VideoCapture videoCapture(0);
	if (!videoCapture.isOpened()) {
		std::cout << "Cannot open the web cam" << std::endl;
		return;
	}

	int lowValueBlue = 0;
	int highValueBlue = 40;

	int lowValueGreen = 0;
	int highValueGreen = 40;

	int lowValueRed = 0;
	int highValueRed = 40;

	int iLastX = -1;
	int iLastY = -1;

	//Capture temporary imag from the camera
	cv::Mat imageTemp;
	videoCapture.read(imageTemp);

	//Capture a black image with size as the camera output
	cv::Mat imageLines = cv::Mat::zeros(imageTemp.size(), CV_8UC3);;

	while (true)
	{	
		cv::Mat imageContours = cv::Mat::zeros(imageTemp.size(), CV_8UC3);;
		cv::Mat imageOriginal;

		videoCapture.read(imageOriginal);

		cv::Mat imageGray;

		//convert the capture fram from BGR TO HSV
		//cv::cvtColor(imageOriginal, imageGray, cv::COLOR_BGR2GRAY);

		cv::Mat imageThresholded;

		cv::inRange(imageOriginal, cv::Scalar(lowValueBlue, lowValueGreen, lowValueRed), cv::Scalar(highValueBlue, highValueGreen, highValueRed), imageThresholded);
		//cv::threshold(imageGray, imageThresholded, 30, 255, THRESH_BINARY);

		cv::erode(imageThresholded, imageThresholded, cv::getStructuringElement(cv:: MORPH_ELLIPSE, cv::Size(5, 5)));
		cv::dilate(imageThresholded, imageThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
		
		cv::dilate(imageThresholded, imageThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
		cv::erode(imageThresholded, imageThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));


		std::cout << imageThresholded.channels() << std::endl;
		vector<vector<Point> > contours;
		cv::findContours(imageThresholded, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		//Calculate the moments of the threshold image
		cv::Moments moments = cv::moments(imageThresholded);

		double dM01 = moments.m01;
		double dM10 = moments.m10;
		double dArea = moments.m00;

		// if the area <= 10000, lets consider that there are no objects in the image because of the noise
		
		if (dArea> 10000)
		{
			//calculate the position of the ball
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;

			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
			{
				//Draw a red line from th previous point to the current point
				cv::line(imageLines, cv::Point(posX, posY), cv::Point(iLastX, iLastY), cv::Scalar(0, 0, 255), 2);
			}

			//then swap the new position to the first position

			iLastX = posX;
			iLastY = posY;
		}

		for (vector<vector<Point>>::const_iterator contour = contours.begin();
			contour != contours.end();
			contour++){

			for (vector<Point>::const_iterator point = contour->begin();
				point != contour->end();
				point++){
				circle(imageContours, (*point), 1.0f, cv::Scalar(0, 255, 0));
			}

		}

		
		imageOriginal = imageOriginal + imageLines + imageContours;
		// Anzeige des Ausgabebildes
		cv::imshow(_windowName, imageOriginal);
		// Auf Benutzereingabe warten und die Benutzereingabe verarbeiten
		int key = cv::waitKey(waitKeyValue);
		if (key == 'e'){
			return;
		}
	}
}



Detection::~Detection()
{
}
