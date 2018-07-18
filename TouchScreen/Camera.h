#ifndef CAMERA_H
#define CAMERA_H

#include "Calibration.h"
#include <QObject>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include <vector>


class Camera : public QObject
{
	Q_OBJECT

public:

	Camera();
	~Camera();

	void setupCam();
	cv::Mat capture();
	cv::Mat img;
	cv::Mat img1, img2,img3, img4;
	int current = 0;
	cv::Point2f camera2world(float x, float y);

	inline void startCalibration(){
		std::cout << "CAM: Get image: " << std::ends;
		_calibration = true;
	}

	inline bool getCalibStatus(){
		return _calibration;
	}

signals:

	void calibrationValid();

	private slots:

	void run();

private:

	int _maximgs;
	Calibration _calibrationObject;
	std::list< cv::Mat > _images;

	bool _calibration = false;
	cv::VideoCapture _camera;

	QTimer *_timer;

};
#endif