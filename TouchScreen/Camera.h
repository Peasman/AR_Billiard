/**
  *
  * Universitaet Dortmund 
  * Fakultaet fuer Informatik
  * Lehrstuhl fuer Graphische System 
  *
  * Author: Thomas Kehrt
  * 12.04.2016
*/


#ifndef CAMERA_H
#define CAMERA_H

#include "Calibration.h"

#include <opencv2/opencv.hpp>
#include <vector>

class Camera
{
public:


	Camera();
	~Camera();

  // Kamerahauptschleife
	void mainLoop();

private:

  // Verarbeitung der Eingabe
  void dispatchKey( int key, cv::Mat img );

  // Bildschirmtext ins ein Bild einzeichnen
  void printMessage( cv::Mat img );

  // Wiedergabe der aufgenommenen Bilder
  void playback();

  // einige Flags zur Programmkontrolle
  bool _terminate
      , _showHelp
      , _applyCalibration
      , _flipDisplay
      , _showEdges;

  // Bildspeicher für aufgenommene Bilder
  std::list< cv::Mat > _images;

  // Wie das Fenster heisst
  std::string _windowName;

  // Strings für Bildschirmausgabe
  // da OpenCV keinen Zeilenumbruch beherrscht
  // ein Vektor von Zeilen
  std::vector< std::string > _message;

  // Ein Kalibrationsobjekt
  Calibration _calibration;
};

#endif