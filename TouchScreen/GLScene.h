#ifndef GLSCENE_H
#define GLSCENE_H

#include <QtOpenGL/QGLWidget>
#include <QtWidgets/QShortcut>
#include <QEvent>
#include <QTouchEvent>
#include <QTimer>

class GLScene : public QGLWidget
{

  Q_OBJECT

public:

  GLScene( QWidget *parent );


public slots:

  void updateFrame();
  void resetGame();
  void enableMouse(bool);
  void startGame(bool);

protected:
  
  QPoint currentPos; // erste Mausposition
  QPoint lastPos; // letzte Mausposition

  void initializeGL();
  void resizeGL( int w, int h );
  void paintGL();

  bool event( QEvent * e );

  void updatePhysics();

  void handleTouchPoints( const QList<QTouchEvent::TouchPoint> &points );

  // Maus-Event-Methoden
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

  
  QTimer *_timer;
  QShortcut *_shrtReset;

  float _w, _h, _xpos, _ypos;

  enum Color{
	  Orange=0, Blue, LightBlue, Red, Brown, Green, Yellow, Black, White, Length
  };
  struct Pool 
  {
    float leftBound, rightBound,              //Linkes Ende des Spielfelds/Pools immer 0, rechtes ist Fenstergröße bzw 1920 im Normalfall
          topBound, bottomBound,              //Oben immer 0 bzw (1080 - 960)/2 = 60 (für das 2:1 Verhältnis) und unten 1020 
          midX, midY;                         //Fenstergröße/2, wird benötigt für Start und Löcher
    
  };
  struct Puck
  {
    
    float x, y,   // position
          vx, vy, // geschwindigkeit
          omega,  // winkelgeschwindigket der rotation in 2 * pi / sek
                  // positive werte -> mathematisch positive rotation ( gegen den uhrzeiger )
                 // negative werte -> mathematisch negative rotation ( mit dem uhrzeiger )
          angle;  // aktueller Winkel des Pucks
  };

  struct Ball
  {
	  //glColor3f color;
	  Color color;                        //Farbe
	  bool full,                          //Halb oder ganz?
		  exists;                        // Noch im spiel?
	  int number;                         //Noetig? Nummer der Kugel 
	  float x = 0, y = 0,                          //Position x und y der Kugel
		  xLast, yLast,                  //Letzte Position
		  vx, vy,                        //Momentane Geschwindigkeit
		  omega,                        //Momentane Winkelgeschwindigkeit
		  angle, angleLast;              //Momentaner Winkel und letzter Winkel
  };
  struct Racket
  {
    float x, y,             // aktuelle position
          xLast, yLast,     // letzte position
          vx, vy,           // geschwindigkeit
          omega,            // winkelgeschwindigket der rotation in 2 * pi / sek
                            // positive werte -> mathematisch positive rotation ( gegen den uhrzeiger )
                            // negative werte -> mathematisch negative rotation ( mit dem uhrzeiger )
          angle, angleLast; // aktueller und letzter Winkel des Rackets
          
    int   tpid1, tpid2;     // touchpoint ids f�r rotationsverfolgung
    float tpx1, tpy1,       // touchpoint positionen rotationsverfolgung
          tpx2, tpy2;
    float tpx1Last, tpy1Last, // touchpoint positionen rotationsverfolgung, letzte werte
          tpx2Last, tpy2Last;
  };
  inline float d( float x1, float y1, float x2, float y2 )
  {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return sqrt( dx * dx + dy * dy );
  }
  inline void normalize( float &x, float &y )
  {
    float n = norm( x, y );
    x /= n;
    y /= n;
  }
  inline float dot( float x1, float y1, float x2, float y2 )
  {
    return x1 * x2 + y1 * y2;
  }
  inline float norm( float x, float y )
  {
    return sqrt( x * x + y * y );
  }
  inline void scalarMult( float scalar, float& x, float& y )
  {
    x *= scalar;
    y *= scalar;
  }
  inline void rotate( float angle, float& x, float &y )
  {
    float xnew, ynew, cosa = cos( angle ), sina = sin( angle );
    xnew = cosa * x - sina * y;
    ynew = sina * x + cosa * y;
    x = xnew;
    y = ynew;
  }

  Puck _puck;
  Racket _racketLeft;
  Racket _racketRight;

  float _puckSize, _racketSize,_ballSize;

  void renderPuck();
  void renderBall(Ball const& ball);
  void renderRacket( Racket const& racket );
  void collidePuckRacket( Racket const& racket );
  void updateBallVelocity(Ball& ball);
  void updateBallCollision(Ball& ball, int index);
  void CollisionWithHole(Ball& ball);
  void initStandardBalls();

  const int _timerPeriod;
  std::vector<Ball> _balls; //Liste der Kugeln die Momentan aktiv sind
  //Generell um Berechnungen während des Spielens zu verringern
  int _gameInit;
  static const float eps;
};

#endif