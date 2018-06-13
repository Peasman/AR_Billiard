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
		cv::Mat imageAxis = cv::Mat::zeros(imageTemp.size(), CV_8UC3);;
		cv::Mat imageOriginal;

		videoCapture.read(imageOriginal);

		cv::Mat imageGray;

		//convert the capture fram from BGR TO HSV
		cv::Mat imageThresholded;

		cv::inRange(imageOriginal, cv::Scalar(lowValueBlue, lowValueGreen, lowValueRed), cv::Scalar(highValueBlue, highValueGreen, highValueRed), imageThresholded);

		cv::erode(imageThresholded, imageThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
		cv::dilate(imageThresholded, imageThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
		
		cv::dilate(imageThresholded, imageThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
		cv::erode(imageThresholded, imageThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
		
		cv::Mat nonZeroCoordinates;
		cv::findNonZero(imageThresholded, nonZeroCoordinates);

		vector<cv::Point> pts;

		for (int i = 0; i < nonZeroCoordinates.total(); i++){
			pts.push_back(nonZeroCoordinates.at<cv::Point>());
		}

		getOrientation(pts, imageAxis);

		imageOriginal = imageThresholded;
		// Anzeige des Ausgabebildes
		cv::imshow(_windowName, imageOriginal);
		// Auf Benutzereingabe warten und die Benutzereingabe verarbeiten
		int key = cv::waitKey(waitKeyValue);
		if (key == 'e'){
			return;
		}
	}
}
void Detection::drawAxis(cv::Mat& img, cv::Point p, cv::Point q, cv::Scalar colour)
{
	double angle;
	double hypotenuse;
	angle = atan2((double)p.y - q.y, (double)p.x - q.x); // angle in radians
	hypotenuse = sqrt((double)(p.y - q.y) * (p.y - q.y) + (p.x - q.x) * (p.x - q.x));
	//    double degrees = angle * 180 / CV_PI; // convert radians to degrees (0-180 range)
	//    cout << "Degrees: " << abs(degrees - 180) << endl; // angle in 0-360 degrees range
	// Here we lengthen the arrow by a factor of scale
	q.x = (int)(p.x - 0.2 * hypotenuse * cos(angle));
	q.y = (int)(p.y - 0.2 * hypotenuse * sin(angle));
	cv::line(img, p, q, colour, 1, CV_AA);
	// create the arrow hooks
	p.x = (int)(q.x + 9 * cos(angle + CV_PI / 4));
	p.y = (int)(q.y + 9 * sin(angle + CV_PI / 4));
	cv::line(img, p, q, colour, 1, CV_AA);
	p.x = (int)(q.x + 9 * cos(angle - CV_PI / 4));
	p.y = (int)(q.y + 9 * sin(angle - CV_PI / 4));
	cv::line(img, p, q, colour, 1, CV_AA);
}
double Detection::getOrientation(std::vector<cv::Point> & pts, cv::Mat& img)
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
		eigen_val[i] = pca_analysis.eigenvalues.at<double>(0, i);
	}
	cv::circle(img, cntr, 3, cv::Scalar(255, 0, 255), 2);
	cv::Point p1 = cntr + 0.02 * cv::Point(static_cast<int>(eigen_vecs[0].x * eigen_val[0]), static_cast<int>(eigen_vecs[0].y * eigen_val[0]));
	cv::Point p2 = cntr - 0.02 * cv::Point(static_cast<int>(eigen_vecs[1].x * eigen_val[1]), static_cast<int>(eigen_vecs[1].y * eigen_val[1]));
	drawAxis(img, cntr, p1, cv::Scalar(0, 255, 0));
	drawAxis(img, cntr, p2, cv::Scalar(255, 255, 0));
	double angle = atan2(eigen_vecs[0].y, eigen_vecs[0].x); // orientation in radians
	return angle;
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
