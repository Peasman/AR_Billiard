#include "Camera.h"

#include <iostream>

Camera::Camera()
{
	_maximgs = 20;

	_camera.open(0);

	if (!_camera.isOpened()){
		std::cout << "CAM: Öffnen der Kamera fehlgeschlagen!" << std::endl;
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

// Undistort gegebenen X,Y-Punkt und gibt diesen zuruek
cv::Point2f Camera::camera2world(float x, float y){
	cv::Point2f pt(x, y);

	return _calibrationObject.undistortPoint(pt);
}

// Bild auswerten (Kalibrieren oder Erkennen)
void Camera::run()
{
	// Falls _calibration durch startCalibration() == true
	if (_calibration)
	{
		// Speichere ein Bild in Liste
		_camera.read(img);
		cv::Mat dest = img.clone();
		_images.push_back(dest);
		//cv::imshow("Check image", dest); // Zeige Bild zur Ueberpruefung

		std::cout << "." << std::ends;
		if (_images.size() >= _maximgs) // Falls Anzahl aufgenommener Bilder >= _maximgs
		{
			std::cout << " (size:" << _images.size() << ")" << std::endl;
			// Werte BIlder aus in Calibration
			std::cout << "CAM: Run calibration" << std::endl;
			_calibrationObject.run(_images);
			_images.clear();
			if (_calibrationObject.valid())//Falls erfolgreich
			{
				std::cout << "CAM: Calibration finished" << std::endl;
				std::cout << std::endl;
				_calibration = false; //Beende Calibration
			}
			else//Falls nicht erfolgreich
			{
				std::cout << "CAM: Calibration failed, try again .." << std::endl;
				std::cout << std::endl;
				_calibration = true; //Erneut Calibration
			}
		}
	}
}