#include "GLScene.h"

#include <QMetaEnum>
#include <iostream>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

const float GLScene::eps = 0.01;

GLScene::GLScene( QWidget *parent )
  : QGLWidget( parent )
  , _timerPeriod( 4 )
  , _gameInit( 60 )
{
  setAttribute( Qt::WA_AcceptTouchEvents );

  _shrtReset = new QShortcut( QKeySequence( "Ctrl+R" ), this );
  connect( _shrtReset, SIGNAL( activated() ), this, SLOT( resetGame() ) );

  _timer = new QTimer( this );
  connect( _timer, SIGNAL( timeout() ), this, SLOT( updateFrame() ) );

  _timer->start( _timerPeriod );

  

  // initialize physics struct with zero
  resetGame(0);
  
}



void GLScene::updateFrame()
{
  if ( _gameInit > 0 )
  {
    _gameInit--;
    if ( _gameInit == 0 )
      resetGame(0);
  }
  updatePhysics();
  update();
}



void GLScene::initializeGL()
{
  // Set up the rendering context, define display lists etc.:
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);

	// Flags für genauere Berechnungen setzen
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	// Schattierungsmodell setzen
	glShadeModel(GL_SMOOTH);

  resetGame(0);
}

void GLScene::resizeGL(int w, int h)
{
  
  float scaleW = static_cast< float >( w ) / _w;
  float scaleH = static_cast< float >( h ) / _h;

  _racketLeft.x *= scaleW;
  _racketLeft.y *= scaleH;
  _racketLeft.vx *= scaleW;
  _racketLeft.vy *= scaleH;

  _racketRight.x *= scaleW;
  _racketRight.y *= scaleH;
  _racketRight.vx *= scaleW;
  _racketRight.vy *= scaleH;

  _puck.x *= scaleW;
  _puck.y *= scaleH;
  

  _w = static_cast< float >( w );
  _h = static_cast< float >( h );

  _puckSize = _h / 30.0f;
  _racketSize = _h / 10.0f;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
  glOrtho( 0.0f
         , _w
         , 0.0f
         , _h
         ,  1.0f
         , -1.0f );

}

void GLScene::paintGL()
{
  // Fensterinhalt löschen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Löschfarbe setzen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Farb und Tiefenpuffer löschen

	// zur ModelView-Matrix wechseln
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // Identitätsmatrix laden

    
  renderPuck();
  renderRacket( _racketLeft );
  renderRacket( _racketRight );

}



bool GLScene::event( QEvent *e )
{
  switch ( e->type() )
  {
  case QEvent::TouchBegin:
  case QEvent::TouchUpdate:
  case QEvent::TouchEnd:
    handleTouchPoints( static_cast< QTouchEvent* >( e )->touchPoints() );
    break;
  default:
    return QWidget::event( e );
  }

  return true;
  
}



void GLScene::handleTouchPoints( const QList<QTouchEvent::TouchPoint> &points )
{
  // Sortiere die Touchpunkte je nach Racket
  // racketPoints[ 0 ] -> linkes Racket
  // racketPoints[ 1 ] -> rechtes Racket
  std::vector< QTouchEvent::TouchPoint > racketPoints[ 2 ];

  for ( QList<QTouchEvent::TouchPoint>::const_iterator point = points.constBegin()
      ; point != points.constEnd()
      ; ++point )
  {


	// speichere Punktpositionen aus i in _xpos und _ypos
	// _xpos = 
	// _ypos =

	  _xpos = point->pos().x();
	  _ypos = _h - point->pos().y();

	  
	// ein Punkt gehört zum Racket wenn der Abstand des Punktes zum Mittelpunkt des Rackets
	// kleiner ist als der Radius des Rackets
	// Verwenden Sie die Funktion d( ... ) für die Berechnung des Abstandes
	// Hinweis: if ( d( ... ) < radius des Rackets )....

	if (d(_racketLeft.x, _racketLeft.y, _xpos, _ypos) < _racketSize){
		racketPoints[0].push_back(*point);
	}
	else if (d(_racketRight.x, _racketRight.y, _xpos, _ypos) < _racketSize){
		racketPoints[1].push_back(*point);
	}
  }

  // Führe folgende Berechnungen nacheinander für beide Punktmengen aus
  for ( int i = 0
      ; i < 2
      ; ++i )	
  {
    
	// Speichere einen Pointer für das aktuelle Racket
	// Das spart später viele if( ... )-Abfragen	
    Racket* racket;
    if ( i == 0 )
      racket = &_racketLeft;		
    else
      racket = &_racketRight;


	QTouchEvent::TouchPoint tp1;
	QTouchEvent::TouchPoint tp2;

	int count = 0;
	float xPos = 0;
	float yPos = 0.0f;
	for (std::vector< QTouchEvent::TouchPoint >::const_iterator point = racketPoints[i].begin()
		; point != racketPoints[i].end()
		; ++point){
		
		xPos += point->pos().x();
		yPos += _h - point->pos().y();
		count++;
		
		if (racket->tpid1 == point->id()){
			tp1 = *point;
		}
		else if (racket->tpid2 = point->id()){
			tp2 = *point;
		}
	}

	// falls überhaupt Punkte existieren
	if (count > 0){

		
		racket->x = fmax(fmin(xPos / count, _w / 2 * (i + 1) - _racketSize), _w / 2 * i + _racketSize);
		racket->y = yPos / count;
	}
	// falls mindestens zwei Punkte existieren --> Rotationsverfolgung
	if (count > 1){
		// Start der Rotationsverfolgung
		if (racket->tpid1 == -1){
			racket->tpid1 = (racketPoints[i])[0].id();
			racket->tpid2 = (racketPoints[i])[1].id();
		}
		else{
			// TouchPoints von letzten Durchlauf vorhanden
			racket->tpx1 = tp1.pos().x();
			racket->tpy1 = tp1.pos().y();
			racket->tpx2 = tp2.pos().x();
			racket->tpy2 = tp2.pos().y();

			float vecX = racket->tpx1 - racket->tpx2;
			float vecXLast = racket->tpx1Last - racket->tpx2Last;
			float vecY = racket->tpy1 - racket->tpy2;
			float vecYLast = racket->tpy1Last - racket->tpy2Last;
			
			float angle = acos(dot(vecX, vecY, vecXLast, vecYLast) / (norm(vecX, vecY) * norm(vecXLast, vecYLast))) * 180.0 / 3.14159265 ;

			float cpz = vecX * vecYLast - vecY * vecXLast;
			float sgn = (cpz > 0) - (cpz < 0);

			
			racket->angle += sgn * fmax(-180, fmin(180, angle)) * 0.10;
		}
		
		racket->tpx1Last = racket->tpx1;
		racket->tpy1Last = racket->tpy1;
		racket->tpx2Last = racket->tpx2;
		racket->tpy2Last = racket->tpy2;
	}
	else{
		racket->tpid1 = -1;
		racket->tpid2 = -1;
	}

	// Berechne die Rotation des Rackets
	// Hinweis: Beachten Sie die Funktionen 
	// normalize : Vektor normalisieren
	// dot       : Punktprodukt zweier Vektoren
	// norm      : Die Standardnorm eines Vektors
	// scalarMult: Skalarmultiplikation eines Vektors
	// rotate    : Rotation es Vektors um einen Winkel
  }
}





void GLScene::updatePhysics()
{
  const float friction = 0.975;
  const float rfriction = 0.001;

  // update the rackets

  // compute velocities
  _racketLeft.vx        = _racketLeft.x - _racketLeft.xLast + 0.2 * _racketLeft.vx;
  _racketLeft.vy        = _racketLeft.y - _racketLeft.yLast + 0.2 * _racketLeft.vy;
  _racketLeft.omega     = _racketLeft.angle  - _racketLeft.angleLast + 0.1 * _racketLeft.omega; 

  // shift the time dependent values
  _racketLeft.xLast     = _racketLeft.x;
  _racketLeft.yLast     = _racketLeft.y;
  _racketLeft.angleLast = _racketLeft.angle;

  // compute velocities
  _racketRight.vx         = _racketRight.x - _racketRight.xLast + 0.2 * _racketRight.vx;
  _racketRight.vy         = _racketRight.y - _racketRight.yLast + 0.2 * _racketRight.vy;
  _racketRight.omega      = _racketRight.angle - _racketRight.angleLast + 0.1 * _racketRight.omega;

  // shift the time dependent values
  _racketRight.xLast      = _racketRight.x;
  _racketRight.yLast      = _racketRight.y;
  _racketRight.angleLast  = _racketRight.angle;
  
  // update the puck
  if ( _puck.vx != 0.0 || _puck.vy != 0.0 )
    rotate( rfriction * _puck.omega, _puck.vx, _puck.vy );
  _puck.x     = _puck.x     + _puck.vx;
  _puck.y     = _puck.y     + _puck.vy;
  _puck.angle = _puck.angle + _puck.omega;

  // introduce some friction so game stays playable
  _puck.vx    *= friction;
  _puck.vy    *= friction;
  _puck.omega *= friction;
  
  // check for collision of borders
  if ( _puck.y + _puckSize > _h )
  {
    _puck.vy *= -1.0f;
    _puck.y -= 2.0 * ( _puck.y + _puckSize - _h );
  }

  if ( _puck.y - _puckSize < 0.0f )
  {
    _puck.vy *= -1.0f;
    _puck.y -= 2.0 * ( _puck.y - _puckSize );
  }

  /**************************************/
  /* Hier Code für Seitenbande einfügen */
  /**************************************/
  float goalScale = 0.5;

  if (_puck.x - _puckSize < 0.0f){
	  if (_puck.y > goalScale / 2 * _h && _puck.y < (1 - goalScale / 2) * _h){
		  
		  resetGame(1);
	  }
	  else{
		  _puck.vx *= -1.0f;
		  _puck.x -= 2.0 * (_puck.x - _puckSize);
	  }
  }

  if (_puck.x + _puckSize > _w){
	  if (_puck.y > goalScale / 2 * _h && _puck.y < (1 - goalScale / 2) * _h) {
		  resetGame(0);
	  }
	  else{
		  _puck.vx *= -1.0f;
		  _puck.x -= 2.0 * (_puck.x + _puckSize - _w);
	  }
  }


  /**************************************/

  // racket puck action
  if ( d( _puck.x, _puck.y, _racketLeft.x, _racketLeft.y ) < _puckSize + _racketSize )
  {
    collidePuckRacket( _racketLeft );
  }

  if ( d( _puck.x, _puck.y, _racketRight.x, _racketRight.y ) < _puckSize + _racketSize )
  {
    collidePuckRacket( _racketRight );
  }

}



void GLScene::resetGame(int player)
{
  _racketLeft.x = 0.25 * _w;
  _racketLeft.y = 0.5 * _h;
  _racketLeft.xLast = _racketLeft.x;
  _racketLeft.yLast = _racketLeft.y;
  _racketLeft.vx = 0.0f;
  _racketLeft.vy = 0.0f;
  _racketLeft.angle = 0.0f;
  _racketLeft.omega = 0.0f;
  _racketLeft.tpid1 = -1;
  _racketLeft.tpid2 = -1;

  _racketRight.x = 0.75 * _w;
  _racketRight.y = 0.5 * _h;
  _racketRight.xLast = _racketRight.x;
  _racketRight.yLast = _racketRight.y;
  _racketRight.vx = 0.0f;
  _racketRight.vy = 0.0f;
  _racketRight.angle = M_PI / 2.0;
  _racketRight.omega = 0.0f;
  _racketRight.tpid1 = -1;
  _racketRight.tpid2 = -1;
  if (player == 0)
	_puck.x = 0.35 * _w;
  else
	_puck.x = 0.65 * _w;
  _puck.y = 0.5 * _h;

  _puck.vx = 0.0f;
  _puck.vy = 0.0f;
  _puck.angle = M_PI;
  _puck.omega = 0.0f;
  
}



void GLScene::renderPuck()
{

  glPushMatrix();
    glLoadIdentity();
    const int k = 32;
    glTranslatef( _puck.x, _puck.y, 0.0f );
    glRotatef( _puck.angle, 0.0, 0.0, 1.0 );
    glScalef( _puckSize, _puckSize, 1.0f );
    
    

    glBegin( GL_TRIANGLE_FAN );
    
      glColor3f( 1.0f, 1.0f, 1.0f );
      glVertex3f( 0.0f, 0.0f, 0.0f );

      for ( int i = 0
          ; i <= k
          ; ++i )
      {
        float x = cos( 2.0 * M_PI * static_cast< float >( i ) / k );
        float y = sin( 2.0 * M_PI * static_cast< float >( i ) / k );
        glColor3f( 1, 1, 1 );
        glVertex3f( x, y, 0.0f );
      }
    glEnd();

  glPopMatrix();
}




void GLScene::renderRacket( Racket const& racket )
{
    glPushMatrix();
    glLoadIdentity();
    const int k = 32;
    glTranslatef( racket.x, racket.y, 0.0f );
    glRotatef( 360 * racket.angle / ( 2 * M_PI ), 0.0, 0.0, 1.0 );
    glScalef( _racketSize, _racketSize, 1.0f );
    
    

    glBegin( GL_TRIANGLE_FAN );
    
      glColor3f( 1.0f, 1.0f, 1.0f );
      glVertex3f( 0.0f, 0.0f, 0.0f );

      for ( int i = 0
          ; i <= k
          ; ++i )
      {
        float x = cos( 2.0f * M_PI * static_cast< float >( i ) / k );
        float y = sin( 2.0f * M_PI * static_cast< float >( i ) / k );
        glColor3f( abs( x ), 0.2f, abs( y ) );
        glVertex3f( x, y, 0.0f );
      }
    glEnd();

  glPopMatrix();
}



void GLScene::collidePuckRacket( Racket const& racket )
{
  const float slip = 0.6;
  float nx, ny, tx, ty;

  // normal
  nx = _puck.x - racket.x;
  ny = _puck.y - racket.y;
  normalize( nx, ny );
  
  // tangent pointing to the left of normal
  tx = -ny;
  ty = nx;

  // relative velocity
  float vsumx = racket.vx - _puck.vx;
  float vsumy = racket.vy - _puck.vy;

  // coordinates in radial tangential coordinate frame
  float vn = nx * vsumx + ny * vsumy;
  float vt = tx * vsumx + ty * vsumy;

  _puck.x += nx * ( vn + 0.1 );
  _puck.y += ny * ( vn + 0.1 );

  _puck.vx += vn * nx;
  _puck.vy += vn * ny;

  Q_ASSERT( d( _puck.x, _puck.y, racket.x, racket.y ) >= _puckSize + _racketSize );
  _puck.omega = slip * -vt + _puck.omega - _racketSize / _puckSize * racket.omega;
  
}
