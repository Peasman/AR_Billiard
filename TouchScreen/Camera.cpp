#include "Camera.h"

#include <iostream>

Camera::Camera()
{
	_camera.open(0);

	if (!_camera.isOpened()){
		std::cout << "CAM: Öffnen der Kamera fehlgeschlagen!" << std::endl;
		std::string s;
		//exit(0);
		return;
	}
	
	//Timer erstellt alle 16ms ein Bild ab Start des Programms
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
cv::Point2f Camera::camera2world(float x, float y){
	cv::Point2f pt(x, y);

	


	return pt;
}

// Bild auswerten (Kalibrieren oder Erkennen)
void Camera::run()
{
	// Falls _calibration durch startCalibration() == true
	if (_calibration)
	{
		// Speichere Bilder in Liste
		_camera.read(img);
		cv::Mat dest=img.clone();
		_images.push_back(dest);
		//cv::imshow("Check image", dest);

		std::cout << "CAM: Get image: " << _images.size() << std::endl;
		if (_images.size() >= 25)
		{
			// und werte diese aus
			std::cout << "CAM: Run calibration" << std::endl;
			_calibrationObject.run(_images);
			if (_calibrationObject.valid())//Falls erfolgreich
			{
				std::cout << "CAM: Calibration finished" << std::endl;
				_calibration = false;
			}
			else//Falls nicht erfolgreich
			{
				std::cout << "CAM: Calibration failed" << std::endl;
				_calibration = false;
			}

		}
	}
}