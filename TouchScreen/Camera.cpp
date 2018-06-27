#include "Camera.h"

#include <iostream>

Camera::Camera()
{
	// oeffne die erste Kamera
	
	_camera.open(0);

	if (!_camera.isOpened()){
		std::cout << "CAM: Öffnung der Kamera fehlgeschlagen!" << std::endl;
		std::string s;
		//exit(0);
		return;
	}
	
	//Timer erstellt alle 16ms ein Bild
	//std::cout << "CAM: Calibration Timer started" << std::endl;
	//_timer = new QTimer(this);
	//connect(_timer, SIGNAL(timeout()), this, SLOT(capture()));
	//_timer->start(16);
}

Camera::~Camera(){}

// Bild aufnehmen
cv::Mat Camera::capture()
{
	_camera.read(img);
	return img;
}

void Camera::run(){
	if (_calibration)
		calibrate(img);
	else
		eval(img);
}

void Camera::eval(cv::Mat img)
{
	// Moritz Methode 
}

// Image versuchen zu kalibrieren
void Camera::calibrate(cv::Mat img)
{
	_images.push_back(img);
	std::cout << "CAM: Calibrate images" << std::endl;
	// calibration valid then leave calibration mode
	if (_calibrationObject.run(_images))
	{
		std::cout << "CAM: Calibration finished" << std::endl;
		_calibration = false;
		//emit calibrationValid();
	}
	// calibration invalid then leave calibration mode
	else{
		std::cout << "CAM: Calibration failed" << std::endl;
		_calibration = false;
	}
	_images.clear();
}