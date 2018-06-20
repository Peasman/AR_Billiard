#ifndef CALIBRATION_H
#define CALIBRATION_H


#include <opencv2\opencv.hpp>
#include <list>


class Calibration
{
public:

	Calibration();
	~Calibration();

	// Kalibrierung berechnen
	void run(std::list< cv::Mat > inputImages);

	// Kalibrierung anwenden
	cv::Mat undistort(cv::Mat img);

	// Ausgabe der Kalibrierung auf der Konsole
	void printCalibration();

	// Zustand der Kalibrierung veroeffentlichen
	inline bool valid() const { return _calibrationValid; }

private:

	// Flag fuer Zustand der Kalibrierung
	bool _calibrationValid;

	// 3D-Koordinaten des Kalibrierpatterns
	std::vector< cv::Vec3f > _patternWorldCoordinates;

	// Parameter der Kalibrierung
	cv::Mat _cameraMatrix       // Kameramatrix
		, _distortionCoeffs;  // Verzerrungskoeffizienten

	// extrinsische Kalibrierung fuer jedes einzelne Bild
	std::vector< cv::Mat> _rvecs    // Rotation
		, _tvecs;   // Translation

	// die Anzahl der Eckpunkte im Kalibrierpattern
	cv::Size _patternSize;

};

#endif