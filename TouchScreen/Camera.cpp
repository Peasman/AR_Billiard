#include "Camera.h"

#include <iostream>

Camera::Camera()
{
	// oeffne die erste Kamera
	_camera.open(0);

	if (!_camera.isOpened()){
		std::cout << "CAM: Öffnung der Kamera fehlgeschlagen!" << std::endl;
		exit(0);
	}

	//Timer erstellt alle 16ms ein Bild
	std::cout << "CAM: Calibration Timer started" << std::endl;
	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), this, SLOT(capture()));
	_timer->start(16);
}

Camera::~Camera(){}

// Bild aufnehmen
void Camera::capture()
{
	cv::Mat img;
	_camera.read(img);

	if (_calibration)
		calibrate(img);
	else
		eval(img);

}

void Camera::eval(cv::Mat img)
{

}

// Image versuchen zu kalibrieren
void Camera::calibrate(cv::Mat img)
{
	std::cout << "CAM: Calibration from Image start until finish" << std::endl;
	bool success = false;
	_images.clear();
	_images.push_back(img);

	//Calibrate
	_calibrationObject.run(_images);

	// calibration valid then leave calibration mode
	if (success)
	{
		_calibration = false;
		emit calibrationValid();
	}
}