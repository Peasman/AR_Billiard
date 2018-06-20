#ifndef CAMERA_H
#define CAMERA_H

#include "Calibration.h"

#include <opencv2/opencv.hpp>
#include <vector>

#include <QObject>
#include <QTimer>

class Camera : public QObject
{
	Q_OBJECT

public:

	Camera();
	~Camera();

	inline bool getCalib(){
		return _calibration;
	}

	// Kamerahauptschleife
	inline void startCalibration(){
		_calibration = true;
		std::cout << "CAM: Calibration from Image start until finish" << std::endl;
	}

signals:

	void calibrationValid();

private slots:

	void capture();

private:

	Calibration _calibrationObject;
	std::list< cv::Mat > _images;
	void eval(cv::Mat img);
	void calibrate(cv::Mat img);

	bool _calibration = false;
	cv::VideoCapture _camera;

	QTimer *_timer;

};
#endif