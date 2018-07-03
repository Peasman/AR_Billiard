#include "Detection.h"

using namespace cv;
using namespace std;
Detection::Detection()
{
	_thresholdsLow = cv::Scalar(0, 0, 0);
	_thresholdsHigh = cv::Scalar(50, 50, 50);
}

void Detection::detectCue(cv::Mat imageOriginal){
	// convert the capture fram from BGR TO HSV
	cv::Mat imageThresholded;

	// threshold RGB picture (maybe HSV better)
	cv::inRange(imageOriginal, _thresholdsLow, _thresholdsHigh, imageThresholded);

	// morphological filtering to remove small gaps
	cv::erode(imageThresholded, imageThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7)));
	cv::dilate(imageThresholded, imageThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7)));

	cv::dilate(imageThresholded, imageThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7)));
	cv::erode(imageThresholded, imageThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7)));

	// get coordinates of detected object
	cv::Mat nonZeroCoordinates = cv::Mat::zeros(imageOriginal.size(), CV_8UC3);
	cv::findNonZero(imageThresholded, nonZeroCoordinates);

	// fill points in std::vector
	vector<cv::Point> pts;
	for (int i = 0; i < nonZeroCoordinates.total(); i++){
		pts.push_back(nonZeroCoordinates.at<cv::Point>(i));
	}

	std::vector<int> indices;
	if (pts.size() > 0){
		indices = analysePoints(pts);
	}
	
	// if no end points detected in picture, set detection to invalid
	if (indices.size() == 0){
		_valid = false;
		return;
	}

	_valid = true;
	// move old detection to last 
	_last_x_1 = _curr_x_1;
	_last_y_1 = _curr_y_1;
	_last_x_2 = _curr_x_2;
	_last_y_2 = _curr_y_2;

	cv::Point2d pt1 = pts[indices[0]];
	cv::Point2d pt2 = pts[indices[1]];

	// calculate distances of current point to last points
	double dx = (_last_x_1 - pt1.x) * (_last_x_1 - pt1.x);
	double dy = (_last_y_1 - pt1.y) * (_last_y_1 - pt1.y);
	double dist1_1 = sqrt(dx + dy);
	//double dist1_2 = (last_x_2 - pt1.x) * (last_x_2 - pt1.x) + (last_y_2 - pt1.x) * (last_y_2 - pt1.x);
	dx = (_last_x_1 - pt2.x) * (_last_x_1 - pt2.x);
	dy = (_last_y_1 - pt2.y) * (_last_y_1 - pt2.y);
	double dist2_1 = sqrt(dx + dy);
	//double dist2_2 = (last_x_1 - pt2.x) * (last_x_1 - pt2.x) + (last_y_1 - pt2.x) * (last_y_1 - pt2.x);

	// check which of the points is closer to last point #1 
	if (dist1_1 < dist2_1){
		_curr_x_1 = pt1.x;
		_curr_y_1 = pt1.y;
		_curr_x_2 = pt2.x;
		_curr_y_2 = pt2.y;
	}
	else{
		_curr_x_1 = pt2.x;
		_curr_y_1 = pt2.y;
		_curr_x_2 = pt1.x;
		_curr_y_2 = pt1.y;
	}
}

void Detection::runTest(){
	_windowName = "Test";
	int waitKeyValue = 10; // 100 Hz

	cv::VideoCapture videoCapture(0);
	if (!videoCapture.isOpened()) {
		std::cout << "Cannot open the web cam" << std::endl;
		return;
	}

	
	//Capture temporary imag from the camera
	cv::Mat imageTemp;
	videoCapture.read(imageTemp);

	//Capture a black image with size as the camera output
	cv::Mat imageLines = cv::Mat::zeros(imageTemp.size(), CV_8UC3);;

	while (true)
	{	
		cv::Mat imageAxis = cv::Mat::zeros(imageTemp.size(), CV_8UC3);;
		cv::Mat imageOriginal;

		videoCapture.read(imageOriginal);

		detectCue(imageOriginal);
		if (_valid){
			cv::circle(imageAxis, cv::Point((int)_curr_x_1, (int)_curr_y_1), 3, cv::Scalar(0, 0, 255), CV_FILLED, 8, 0);
			cv::circle(imageAxis, cv::Point((int)_curr_x_2, (int)_curr_y_2), 3, cv::Scalar(0, 255, 0), CV_FILLED, 8, 0);
		}
		
		// Anzeige des Ausgabebildes
		imageOriginal = imageOriginal + imageAxis;
			cv::imshow(_windowName, imageOriginal);
		// Auf Benutzereingabe warten und die Benutzereingabe verarbeiten
		int key = cv::waitKey(waitKeyValue);
		if (key == 'e'){
			return;
		}
	}
}
std::vector<int> Detection::analysePoints(std::vector<cv::Point> & pts)
{
	//Construct a buffer used by the pca analysis
	int sz = static_cast<int>(pts.size());
	cv::Mat data_pts = cv::Mat(sz, 2, CV_64FC1);
	for (int i = 0; i < data_pts.rows; ++i)
	{
		data_pts.at<double>(i, 0) = pts[i].x;
		data_pts.at<double>(i, 1) = pts[i].y;
	}
	//Perform PCA analysis
	cv::PCA pca_analysis(data_pts, cv::Mat(), CV_PCA_DATA_AS_ROW);
	//Store the center of the object
	cv::Point cntr = cv::Point(static_cast<int>(pca_analysis.mean.at<double>(0, 0)),
		static_cast<int>(pca_analysis.mean.at<double>(0, 1)));
	//Store the eigenvalues and eigenvectors
	vector<cv::Point2d> eigen_vecs(2);
	vector<double> eigen_val(2);
	for (int i = 0; i < 2; ++i)
	{
		eigen_vecs[i] = cv::Point2d(pca_analysis.eigenvectors.at<double>(i, 0),
			pca_analysis.eigenvectors.at<double>(i, 1));
		eigen_val[i] = pca_analysis.eigenvalues.at<double>(i);
	}

	double norm = cv::norm(eigen_vecs[0]);

	int biggestIndex = -1, smallestIndex = -1;
	double biggestScalar, smallestScalar;

	for (int i = 0; i < data_pts.rows; ++i){
		//cv::Point2d pts_centered = cv::Point2d(pts[i].x - cntr.x, pts[i].y - cntr.y);
		double ptc_x = pts[i].x - cntr.x;
		double ptc_y = pts[i].y - cntr.y;
		//double dot = eigen_vecs[0].dot(pts_centered);
		double dot = eigen_vecs[0].x*ptc_x + eigen_vecs[0].y*ptc_y;
		double d1 = dot / norm;

		//cv::Point2d ap = cv::Point2d(d1 * princ.x, d1 * princ.y);
		//cv::Point2d p = cv::Point2d(ap.x + cntr.x, ap.y + cntr.y);
		double px = d1 * eigen_vecs[0].x + cntr.x;
		double py = d1 * eigen_vecs[0].y + cntr.y;
		
		pts[i].x = (int)px;
		pts[i].y = (int)py;

		if (smallestIndex == -1 || d1 < smallestScalar){
			smallestIndex = i;
			smallestScalar = d1;
		}
		if (biggestIndex == -1 || d1 > biggestScalar){
			biggestIndex = i;
			biggestScalar = d1;
		}
	}
	std::vector<int> indices;
	indices.push_back(smallestIndex);
	indices.push_back(biggestIndex);
	return indices;
}





/*

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




*/




Detection::~Detection()
{
}
