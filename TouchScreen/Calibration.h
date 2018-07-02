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
	void run(std::list< cv::Mat >& inputImages);

	// Kalibrierung anwenden
	cv::Mat undistort(cv::Mat img);
	cv::Point2f undistortPoint(cv::Point2f point);
	// Ausgabe der Kalibrierung auf der Konsole
	void printCalibration();


	// Zustand der Kalibrierung veroeffentlichen
	inline bool valid() const { return _calibrationValid; }

private:

	// Flag fuer Zustand der Kalibrierung
	bool _calibrationValid;

	// 3D-Koordinaten des Kalibrierpatterns
	std::vector< cv::Point3f > _patternWorldCoordinates;

	// Parameter der Kalibrierung
	cv::Mat _cameraMatrix       // Kameramatrix
		, _distortionCoeffs;  // Verzerrungskoeffizienten

	// extrinsische Kalibrierung fuer jedes einzelne Bild
	std::vector< cv::Mat> _rvecs    // Rotation
		, _tvecs;   // Translation

	cv::vector< cv::Point2f> corners;


	// die Anzahl der Eckpunkte im Kalibrierpattern
	cv::Size _patternSize;
	bool actually_findChessboardCorners(cv::Mat& frame, cv::Size& size, cv::Mat& corners, int flags);

};

#endif