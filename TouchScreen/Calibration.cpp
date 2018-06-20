#include "Calibration.h"

#include <iomanip>


Calibration::Calibration()
	: _patternWorldCoordinates(7 * 7)
	, _cameraMatrix(3, 3, CV_64F)
	, _distortionCoeffs(8, 1, CV_64F)
	, _patternSize(7, 7)
	, _calibrationValid(false)
{
	// erzeuge die Weltkoordinaten fuer das Kalibrierbild
	// Kantenlänge ist 29 mm
	// Weltkoordinaten sind nur in relativer Position der Eckpunkte
	// wichtig, daher wird das Pattern eben in der xy-Ebene angenommen
	for (int row = 0; row < 7; ++row)
	{
		for (int col = 0; col < 7; ++col)
		{
			int index = row * 7 + col;
			_patternWorldCoordinates[index][0] = static_cast<float>(col)* 29.0f;
			_patternWorldCoordinates[index][1] = static_cast<float>(row)* 29.0f;
			_patternWorldCoordinates[index][2] = 0.0f;
		}
	}
}


Calibration::~Calibration()
{}




void Calibration::run(std::list< cv::Mat > inputImages)
{
	// Speicher fuer die Patterneckpunkte in Bildkoordinaten
	std::vector< std::vector< cv::Point2f > > patternCorners;

	// Speicher fuer die Patterneckpunkte in Weltkoordinaten
	std::vector< std::vector< cv::Vec3f > > patternWorldBuffer;

	// Anzahl der Bilder, in denen das Pattern gefunden wurde
	int goodCount = 0;
	size_t i = 0;
	for (std::list < cv::Mat >::iterator img = inputImages.begin()
		; img != inputImages.end()
		; ++img, ++i)
	{
		// Zwischenspeicher fuer Eckpunkte in Bildkoordinaten
		std::vector< cv::Point2f > pointBuffer;

		// suche das Pattern
		bool found = cv::findChessboardCorners(*img, _patternSize, pointBuffer);

		// wurde das Pattern gefunden?
		// Nein: Ausgabe generieren
		if (!found){
			std::cout << "CALIB: Pattern nicht gefunden" << std::endl;
		}
		// Ja: hier weiter
		else
		{

			std::cout << "CALIB: Muster gefunden" << std::endl;
			// Counter erhoehen
			goodCount++;
			// Zwischespeicher enthaelt valide Werte
			// in globalen Puffer einfuegen
			patternCorners.push_back(pointBuffer);

			// ebenso fuer Weltkoordinaten
			patternWorldBuffer.push_back(_patternWorldCoordinates);

			// Einzeichnen der Eckpunkte in das Bild fuer spaetere Wiedergabe
			cv::drawChessboardCorners(*img, _patternSize, pointBuffer, found);
		}

	}

	// genug gute Bilder gefunden?
	if (goodCount > 0)
	{

		std::cout << "CALIB: Gute bilder gefunden" << std::endl;
		// schalte die Kalibrierung gueltig
		_calibrationValid = true;

		// Speicher fuer extrinsische Kalibrierungen reservieren
		_rvecs.resize(goodCount);
		_tvecs.resize(goodCount);

		// Kalibrierung durchfuehren
		cv::calibrateCamera(patternWorldBuffer, patternCorners, _patternSize, _cameraMatrix, _distortionCoeffs, _rvecs, _tvecs, 0);

	}
	else{
		// nicht genug Daten -> keine gueltige Kalibrierung
		std::cout << "CALIB: Nicht genug Daten gefunden!" << std::endl;
	}
}



cv::Mat Calibration::undistort(cv::Mat img)
{
	// Ergebnisspeicher reservieren
	cv::Mat result;

	// Bild entzerren
	cv::undistort(img, result, _cameraMatrix, _distortionCoeffs);

	// Rueckgabe
	return result;
}



void Calibration::printCalibration()
{

	// keine Kalibrierung -> keine Asgabe
	if (!_calibrationValid)
	{
		std::cout << "Calibration is invalid." << std::endl;
		return;
	}


	// Kameramatrix ausgeben
	std::cout << "Camera Matrix:" << std::endl;

	std::cout.precision(3);

	for (int i = 0; i < 3; ++i)
	{
		double *row = _cameraMatrix.ptr< double >(i);
		for (int j = 0; j < 3; ++j)
			std::cout << std::setw(10) << row[j];
		std::cout << std::endl;
	}


	// Radiale Verzerrung ausgeben
	std::cout << "radial distortion" << std::endl;
	for (int i = 0; i < _distortionCoeffs.rows; ++i)
	{
		if (i != 2 && i != 3)
			std::cout << std::setw(14) << _distortionCoeffs.ptr< double >(i)[0];
	}
	std::cout << std::endl;


	// tangentiale Verzerrung ausgeben
	std::cout << "tangential distortion: " << std::endl;
	std::cout << std::setw(14) << _distortionCoeffs.ptr< double >(2)[0]
		<< std::setw(14) << _distortionCoeffs.ptr< double >(3)[0] << std::endl;


	// extrinsische Kalibrierung ausgeben
	for (size_t t = 0; t < _rvecs.size(); ++t)
	{
		std::cout << "mapping " << t << std::endl;
		cv::Mat matrix;
		cv::Rodrigues(_rvecs[t], matrix);
		for (int i = 0; i < 3; ++i)
		{
			double *row = matrix.ptr< double >(i);
			for (int j = 0; j < 3; ++j)
				std::cout << std::setw(14) << row[j];

			std::cout << std::setw(14) << _tvecs[t].ptr< double >(i)[0] << std::endl;
		}
	}
}