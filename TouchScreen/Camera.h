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

	// Kamerahauptschleife
	inline void startCalibration(){
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

	Calibration _calibrationObject;
	std::list< cv::Mat > _images;

	bool _calibration = false;
	cv::VideoCapture _camera;

	QTimer *_timer;

};
#endif