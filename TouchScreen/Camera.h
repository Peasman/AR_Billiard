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

	cv::Mat capture();
	cv::Mat img;

	void run();

	inline bool getCalib(){
		return _calibration;
	}

	// Kamerahauptschleife
	inline void startCalibration(){
		_calibration = true;
	}

signals:

	void calibrationValid();

private slots:

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