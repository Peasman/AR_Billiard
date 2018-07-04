#include "Calibration.h"
#include <iomanip>

Calibration::Calibration()
	: _cameraMatrix(3, 3, CV_64F)
	, _distortionCoeffs(8, 1, CV_64F)
	, _patternSize(15, 7)
	, _calibrationValid(false)
{}

Calibration::~Calibration(){}

void Calibration::run(std::list< cv::Mat >& inputImages)
{
	int _idximg = 0;
	// erzeuge die Weltkoordinaten fuer das Kalibrierbild
	// Weltkoordinaten sind nur in relativer Position der Eckpunkte
	// wichtig, daher wird das Pattern eben in der xy-Ebene angenommen
	float a = 10.0f; //Kantenlaenge
	for (int y = 0; y < 7; y++){
		for (int x = 0; x < 15; x++){
			// static_cast<float>()?
			_patternWorldCoordinates.push_back(cv::Point3f(x*a, y*a, 0)); //z = 0
		}
	}

	// Speicher fuer die Patterneckpunkte in Bildkoordinaten (object)
	std::vector< std::vector< cv::Point2f > > patternCorners;

	// Speicher fuer die Patterneckpunkte in Weltkoordinaten (image)
	std::vector< std::vector< cv::Point3f > > patternWorldBuffer;

	// Anzahl der Bilder, in denen das Pattern gefunden wurde
	int goodCount = 0;
	for (std::list < cv::Mat >::iterator img = inputImages.begin()
		; img != inputImages.end()
		; ++img)
	{
		// Zwischenspeicher fuer Eckpunkte in Bildkoordinaten
		std::vector< cv::Point2f > pointBuffer;
		cv::Mat pointBuf;
		bool found = actually_findChessboardCorners(*img, _patternSize, pointBuf, 0);
		//cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);

		pointBuffer.assign((cv::Point2f*)pointBuf.datastart, (cv::Point2f*)pointBuf.dataend);
		// wurde das Pattern gefunden?
		if (!found){
			// aktuelles Image-Index erhoehen
			_idximg++;
			std::cout << "CALIB: " << _idximg << " / " << inputImages.size() << " Pattern NICHT gefunden" << std::endl;
		}
		else
		{
			goodCount++;
			// Zwischespeicher enthaelt valide Werte
			// in globalen Puffer einfuegen
			patternCorners.push_back(pointBuffer);
			// ebenso fuer Weltkoordinaten
			patternWorldBuffer.push_back(_patternWorldCoordinates);
			// Einzeichnen der Eckpunkte in das Bild fuer spaetere Wiedergabe
			//			cv::drawChessboardCorners(*img, _patternSize, pointBuffer, found);
			//std::cout << "CALIB: " << ++_idximg << " / " << inputImages.size() << " Pattern gefunden -> Undistortion .." << std::endl;
		}
	}

	// genug gute Bilder gefunden?
	if (goodCount > 0)
	{
		// Speicher fuer extrinsische Kalibrierungen reservieren
		_rvecs.resize(goodCount);
		_tvecs.resize(goodCount);

		// Kalibrierung durchfuehren
		cv::calibrateCamera(patternWorldBuffer, patternCorners, _patternSize, _cameraMatrix, _distortionCoeffs, _rvecs, _tvecs);

		cv::Point2f undistortedPoint;
		for (int i = 0; i < patternCorners[0].size(); i++){
			std::cout << "CALIB: Distorted Point, x: " << patternCorners[0][i].x << ", y: " << patternCorners[0][i].y << std::endl;
			undistortedPoint = undistortPoint(patternCorners[0][i]);
			std::cout << "CALIB: Undistorted Point: " << undistortedPoint.x << ", y: " << undistortedPoint.y << std::endl;
		}

		/* Erstes gutes Bild wird entzerrt */
		cv::Mat img = *(inputImages.begin());
		cv::Mat undistorted = undistort(img);

		cv::Mat R;
		cv::Rodrigues(_rvecs[0], R);
		std::cout << R << std::endl;
		cv::Mat rotated;
		std::cout << "Rotating" << std::endl;
		cv::Mat r1;
		cv::vconcat(R.row(0), R.row(1), r1);
		cv::invertAffineTransform(r1, r1);
		std::cout << r1 << std::endl;
		cv::imshow("Distorted", img);
		cv::Mat undistord;
		cv::undistort(img, undistord, _cameraMatrix, _distortionCoeffs);
		cv::imshow("Undistorted", undistord);
		cv::warpAffine(undistord, rotated, r1, img.size());
		cv::imshow("Rotated", rotated);
		/* Undistrotion + Rotation completed */

		/* Eckpunkte ganz aussen berechnen */
		cv::Point2f obenLinks = patternCorners[0][(1 * 1) - 1];
		cv::Point2f obenLinksInnen = patternCorners[0][(2 * 2) - 1];
		cv::Point2f untenRechtsInnen = patternCorners[0][(14 * 6) - 1];
		cv::Point2f untenRechts = patternCorners[0][(15 * 7) - 1];
		/*
		obenLinks = undistortPoint(obenLinks);
		obenLinksInnen = undistortPoint(obenLinks);
		untenRechtsInnen = undistortPoint(untenRechtsInnen);
		untenRechts = undistortPoint(untenRechts);
		*/
		int xDiffOL = obenLinksInnen.x - obenLinks.x;
		int yDiffOL = obenLinksInnen.y - obenLinks.y;
		int xNeuOL = obenLinks.x - xDiffOL;
		int yNeuOL = obenLinks.y - yDiffOL;
		eckpunktObenLinks = cv::Point2f(xNeuOL, yNeuOL);

		int xDiffUR = untenRechts.x - untenRechtsInnen.x;
		int yDiffUR = untenRechts.y - untenRechtsInnen.y;
		int xNeuUR = untenRechtsInnen.x - xDiffOL;
		int yNeuUR = untenRechtsInnen.y - yDiffOL;
		eckpunktUntenRechts = cv::Point2f(xNeuUR, yNeuUR);

		/* Eckpunkte innen entzerren */
		cv::Point2f upperLeftPt = patternCorners[0][0];
		cv::Point2f lowerRightPt = patternCorners[0][(15 * 7) - 1];
		std::cout << "CALIB: Distorted: " << std::endl;
		std::cout << "CALIB: Upper Left, x: " << upperLeftPt.x << ", y: " << upperLeftPt.y << std::endl;
		std::cout << "CALIB: Lower Right, x: " << lowerRightPt.x << ", y: " << lowerRightPt.y << std::endl;

		upperLeftPt = undistortPoint(upperLeftPt);
		lowerRightPt = undistortPoint(lowerRightPt);

		std::cout << "CALIB: Undistorted: " << std::endl;
		std::cout << "CALIB: Upper Left, x: " << upperLeftPt.x << ", y: " << upperLeftPt.y << std::endl;
		std::cout << "CALIB: Lower Right, x: " << lowerRightPt.x << ", y: " << lowerRightPt.y << std::endl;

		/* Kalibrierung fertig */
		_calibrationValid = true;
	}
	else// nicht genug Daten -> keine gueltige Kalibrierung
	{
		std::cout << "CALIB: Nicht genug gute Bilder gefunden" << std::endl;
	}
}

cv::Point2f Calibration::mapPoint(cv::Point2f point){
	return cv::Point2f(0, 0);
}

bool Calibration::actually_findChessboardCorners(cv::Mat& frame, cv::Size& size, cv::Mat& corners, int flags) {
	int count = size.area();
	corners.create(count, 1, CV_32FC2);
	CvMat _image = frame;
	return cvFindChessboardCorners(&_image, size, reinterpret_cast<CvPoint2D32f*>(corners.data), &count, flags) > 0;
}

cv::Point2f Calibration::undistortPoint(cv::Point2f point)
{
	std::vector<cv::Point2f> in;
	in.push_back(point);

	std::vector<cv::Point2f> out;

	cv::undistortPoints(in, out, _cameraMatrix, _distortionCoeffs);

	return *(out.begin());
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
		std::cout << "CALIB: Calibration is invalid." << std::endl;
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