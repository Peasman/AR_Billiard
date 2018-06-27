#include "Camera.h"

#include <iostream>

Camera::Camera()
{
	// oeffne die erste Kamera
	
	_camera.open(0);

	if (!_camera.isOpened()){
		std::cout << "CAM: �ffnung der Kamera fehlgeschlagen!" << std::endl;
		std::string s;
		//exit(0);
		return;
	}
	
	//Timer erstellt alle 16ms ein Bild
	std::cout << "CAM: Calibration Timer started" << std::endl;
	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), this, SLOT(testSignal()));
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
	bool success = false;
	_images.clear();
	_images.push_back(img);

	//Calibrate
	_calibrationObject.run(_images);

	// calibration valid then leave calibration mode
	if (success)
	{
		std::cout << "CAM: Calibration finished" << std::endl;
		_calibration = false;
		emit calibrationValid();
	}
}