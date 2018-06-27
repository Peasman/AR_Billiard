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
	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), this, SLOT(run()));
	_timer->start(16);
}

Camera::~Camera(){}

// Bild aufnehmen und zuruekgeben
cv::Mat Camera::capture()
{
	_camera.read(img);
	return img;
}
// Bild auswerten (Kalibrieren oder Erkennen)
void Camera::run()
{
	if (_calibration)
	{
		_camera.read(img);
		cv::Mat dest=img.clone();
		_images.push_back(dest);
		std::cout << "CAM: Get image: " << _images.size() << std::endl;
		if (_images.size() > 50)
		{
			std::cout << "CAM: Run calibration" << std::endl;
			_calibrationObject.run(_images);
			if (_calibrationObject.valid())
			{
				std::cout << "CAM: Calibration finished" << std::endl;
				_calibration = false;
			}
			else
			{
				std::cout << "CAM: Calibration failed" << std::endl;
				_calibration = false;
			}

		}
	}
}