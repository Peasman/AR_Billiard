/**
  *
  * Universitaet Dortmund 
  * Fakultaet fuer Informatik
  * Lehrstuhl fuer Graphische System 
  *
  * Author: Thomas Kehrt
  * 12.04.2016
*/


#include "Camera.h"

#include <iostream>

Camera::Camera()
              : _terminate( false )
              , _windowName( "Camera Calibaration" )
              , _message( 1, "Type 'h' for help!" )
              , _showHelp( false )
              , _flipDisplay( true )
              , _applyCalibration( false )
              , _showEdges( false )
{}


Camera::~Camera()
{}


void Camera::mainLoop()
{
	int scale = 1;
	int ddepth = CV_16S;
  // Frequenz der Bildwiedergabe
  int waitKeyValue = 10; // 60 Hz

  // Speicher fue das naechste Kamerabild
  cv::Mat img;

  // oeffne die erste Kamera
  cv::VideoCapture videoCapture(0);
  
  // wenn Kamera erkannt
  if (videoCapture.isOpened())
  {
    // solange nicht beendet werden soll
    while ( !_terminate )
    {

      // lies ein Bild von der Kamera aus
		videoCapture.read(img);
      // Speicher für die Bildausgabe
      cv::Mat display;
      // wenn wir eine Kalibrierung haben und ein kalibriertes Bild sehen wollen
      if ( _calibration.valid() && _applyCalibration )
      {
        // entzerre das Bild
        img = _calibration.undistort( img );
      }

      // fuer die bessere Sichtbarkeit der Kanten im Bild
      if ( _showEdges )
      {
        // Speicher fuer ein Grauwertbild 
        // die erste Ableitung in x und y
        // und die Absulutwerte derselben
        cv::Mat gray
              , gradX, gradY
              , absGradX, absGradY;

        // Wandle das Bild in ein Grauwertbild
		cv::cvtColor(img,gray, CV_BGR2GRAY);
        // berechne die Ableitung in x
		cv::Sobel(gray, gradX, ddepth, 1, 0,3);
        // berechne den Absulutwert der Ableitung in x
		cv::convertScaleAbs(gradX, absGradX);
        // berechne die Ableitung in y
		cv::Sobel(gray, gradY, ddepth, 0, 1, 3);
        // berechne den Absulutwert der Ableitung in y
		cv::convertScaleAbs(gradY, absGradY);
        // addiere die beiden Bilder gewichtet mit 0.5 aufeinander
		cv::addWeighted(absGradX, 0.5, absGradY, 0.5,0, img);
      }

      // Komfortfunkton zur Spiegelung des Anzeigebildes
	  if (_flipDisplay) {
		  cv::flip(img,display,1);
	  }
	  else {
		  img.copyTo(display);
	  }

      // Einzeichnen der Informationstexte ins Bild
      

      // Anzeige des Ausgabebildes
	  cv::imshow("bild", display);
      // Auf Benutzereingabe warten und die Benutzereingabe verarbeiten
	  dispatchKey(cv::waitKey(waitKeyValue), display);
    }
  }

  {
    // Fehlermeldung, wenn keine Kamera gefunden wurde
  }
}



void Camera::dispatchKey( int key, cv::Mat img )
{

  switch ( key )
  {

  // Anwendung der Kalibrierung
  case 'a':
  case 'A':
    _applyCalibration = !_applyCalibration;
    break;

  // Bildaufnahme
  case 'c':
  case 'C':
  {
    cv::Mat copy;
    img.copyTo( copy );
    _images.push_back( copy );
    std::cout << "Images in container: " << _images.size() << std::endl;
    break;
  }
    
  // Loesche den Bildaufnahmespeicher
  case 'd':
  case 'D':
    _images.clear();
    break;

  // Anzeige der auf Kantenbild umstellen und zurueck
  case 'e':
  case 'E':
    _showEdges = !_showEdges;
    break;

  // Anzeigebild spiegeln oder nicht
  case 'f':
  case 'F':
    _flipDisplay = !_flipDisplay;
    break;

  // Hilfe anzeigen
  case 'h':
  case 'H':
  {
    _showHelp = !_showHelp;
    _message.clear();
    if ( _showHelp )
    {
      _message.push_back( "Available Options" );
      _message.push_back( "'a' apply undistortion" );
      _message.push_back( "'c' capture an image" );
      _message.push_back( "'e' show image edges" );
      _message.push_back( "'f' flip image display" );
      _message.push_back( "'p' playback images acquired" );
      _message.push_back( "'r' run calibration" );
      _message.push_back( "'d' delete images acquired" );
      _message.push_back( "'EXC' to quit" );
    }
    else
    {
      _message.push_back( "Type 'h' for help!" );
    }
    break;
  }

  // Bilder aus dem Speicher anzeigen
  case 'p':
  case 'P':
    playback();
    break;

  // Kalibrierung mit den Bildern aus dem Bildaufnahmespeicher durchfuehren
  case 'r':
  case 'R':
    _calibration.run( _images );
    _calibration.printCalibration();
    break;

  // Programm beenden
  case 27: // ASCII for Esc
  case 'q':
  case 'Q':
    _terminate = true;
    break;

  }
}



void Camera::printMessage( cv::Mat img )
{
  int i = 0;
  for ( std::vector< std::string >::iterator s = _message.begin()
    ; s != _message.end()
    ; ++s, ++i )
  {
    cv::putText( img, _message[ i ], cv::Point( 0, (i + 1) * 18 ), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, 0, 2 );
  }
}



void Camera::playback()
{
  
  int n = 1;

  // Iteration ueber den Bildaufnahmespeicher
  for ( std::list< cv::Mat >::iterator i = _images.begin()
      ; i != _images.end()
      ; ++i, ++n )
  {
    
    // Speicher fuer das Anzeigebild
    cv::Mat display;

    // Anzeigebild spiegeln oder nicht
    if ( _flipDisplay )
      cv::flip( *i, display, 1 );
    else
      i->copyTo( display );

    // Ausgabetext erstellen
    std::stringstream ss;
    ss << n << " / " << _images.size() << " Any key to continue";

    // Ausgabetext in das Bild zeichnen 
    cv::putText( display, ss.str() , cv::Point( 0, 16 ), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, 0, 2 );

    // Bild zur Anzeige bringen
    cv::imshow( _windowName, display );

    // auf Benutzereingabe warten
    // any key to continue
    while ( cv::waitKey( 16 ) == -1 );
  }

}