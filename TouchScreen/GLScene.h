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
  void resetGame(int player);

protected:
  
  void initializeGL();
  void resizeGL( int w, int h );
  void paintGL();

  bool event( QEvent * e );

  void updatePhysics();
  
  void handleTouchPoints( const QList<QTouchEvent::TouchPoint> &points );
  
  QTimer *_timer;
  QShortcut *_shrtReset;

  float _w, _h, _xpos, _ypos;

  struct Puck
  {
    
    float x, y,   // position
          vx, vy, // geschwindigkeit
          omega,  // winkelgeschwindigket der rotation in 2 * pi / sek
                  // positive werte -> mathematisch positive rotation ( gegen den uhrzeiger )
                  // negative werte -> mathematisch negative rotation ( mit dem uhrzeiger )
          angle;  // aktueller Winkel des Pucks
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
          
    int   tpid1, tpid2;     // touchpoint ids für rotationsverfolgung
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

  float _puckSize, _racketSize;

  void renderPuck();
  void renderRacket( Racket const& racket );
  void collidePuckRacket( Racket const& racket );
  
  const int _timerPeriod;
  
  int _gameInit;
  static const float eps;
};

#endif