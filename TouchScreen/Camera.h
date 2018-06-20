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

	// Kamerahauptschleife
	inline void startCalibration(){
		_calibration = true;
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

	cv::VideoCapture _camera;

	QTimer *_timer;

	bool _calibration = false;
};
#endif