#include "GLScene.h"

#include <QMetaEnum>
#include <iostream>
#include <vector>
#include <QtWidgets/QApplication>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL\glut.h>

const float GLScene::eps = 0.01;
GLScene::GLScene(QWidget *parent)
	: QGLWidget(parent), _timerPeriod(4), _gameInit(60)
{
	setAttribute(Qt::WA_AcceptTouchEvents);

	_shrtReset = new QShortcut(QKeySequence("Ctrl+R"), this);
	connect(_shrtReset, SIGNAL(activated()), this, SLOT(resetGame()));

	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), this, SLOT(updateFrame()));

	_timer->start(_timerPeriod);

	// initialize physics struct with zero
	//resetGame();
}
//TODO ?
void initGame()
{

}
bool alreadyStarted = false;
bool isMouseEnable = false;

//Die Funktion der Mouse wird aktiviert
void GLScene::enableMouse(bool isEnable){
	isMouseEnable = isEnable;
}
//========================================================================================
// Event für das Drücken einer Maustaste
//========================================================================================
void GLScene::mousePressEvent(QMouseEvent *event)
{
	if (!isMouseEnable){
		currentPos = event->pos();
	}
}
//========================================================================================
// Event für Mausbewegungen
//========================================================================================
void GLScene::mouseMoveEvent(QMouseEvent *event)
{
	if (!isMouseEnable){
		lastPos = currentPos;
		float currentx = event->pos().x();
		float currenty = event->pos().y()+40.0f;
		currentPos = QPoint(currentx, currenty);
		std::cout << currentPos.x() << " y: " << currentPos.y() << std::endl;
	}
}

//========================================================================================
// Event für das Loslassen einer Maustaste
//========================================================================================
void GLScene::mouseReleaseEvent(QMouseEvent *event)
{
	if (!isMouseEnable){

	}
}


void GLScene::startGame(bool gameStarted){
	if (!alreadyStarted){
		alreadyStarted = gameStarted;
		resetGame();
	}
	else{
		const int result = MessageBox(nullptr, TEXT("A game is already started, do you want to start a new Game?"), TEXT("Restart Game"), MB_YESNO);
		switch (result)
		{
		case IDYES:
			// RESET GAME WIRD AUSGEFÜHRT
			resetGame();
			break;
		}
	}
}

void GLScene::updateFrame()
{
	if (!alreadyStarted) {

		const int result = MessageBox(nullptr, TEXT("Start a new game?"), TEXT("New Game"), MB_YESNO);
		switch (result)
		{
		case IDNO:
			QApplication::quit();
		case IDYES:
			// RESET GAME WIRD AUSGEFÜHRT
			alreadyStarted = true;
			resetGame();
			break;
		}
	}
	updatePhysics();
	update();
}

void GLScene::initializeGL()
{
	// Set up the rendering context, define display lists etc.:
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	// Flags f�r genauere Berechnungen setzen
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	// Schattierungsmodell setzen
	glShadeModel(GL_SMOOTH);

	resetGame();
}

void GLScene::resizeGL(int w, int h)
{

	float scaleW = static_cast<float>(w) / _w;
	float scaleH = static_cast<float>(h) / _h;


	_w = static_cast<float>(w);
	_h = static_cast<float>(h);
	_ballSize = _h / 40.0f;
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, _w, 0.0f, _h, 1.0f, -1.0f);
}

void GLScene::paintGL()
{
	// Fensterinhalt l�schen
	glClearColor(0.0f, 0.4f, 0.0f, 1.0f);               // L�schfarbe setzen auf Billiard Pool Farbe
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Farb und Tiefenpuffer l�schen

	// zur ModelView-Matrix wechseln
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // Identit�tsmatrix laden
	for (int i = 0; i < _balls.size(); i++) {
		renderBall(_balls[i]);
	}
	
}

void GLScene::updateBallVelocity(Ball& ball)
{	

	const float friction = 0.99f;
	const float rfriction = 0.001f;
	rotate(rfriction * ball.omega, ball.vx, ball.vy);
	ball.x = ball.x + ball.vx;
	ball.y = ball.y + ball.vy;
	ball.angle = ball.angle + ball.omega;

	// introduce some friction so game stays playable
	ball.vx *= friction;
	ball.vy *= friction;
	ball.omega *= friction;
}
void GLScene::CollisionWithHole(Ball& ball)
{
	for (int i = 0; i <= 2; i++)
	{ // Von links nach rechts alle Löcher durch zählen
		for (int j = 0; j <= 1; j++)
		{ //Von Oben nach unten die Löcher prüfen
			float dX = ball.x - i * _w / 2;
			float dY = ball.y - j * _h;
			if (sqrt(dX * dX + dY * dY) < _ballSize)
			{
				// TODO Dann ist im loch also löschen!!
			}
		}
	}
}
void GLScene::updateBallCollision(Ball& ball, int index)
{

	//TODO Collision mit Löchern
	//CollisionWithHole(ball);
	if (ball.color == Color::White) {
		ball.vx =20.0f;
		ball.vy = 0.0f;
	}
	if (ball.exists)
	{
		
		if (ball.y + _ballSize > _h) // unterer Rand
		{
			ball.vy *= -1.0f;
			ball.y -= 2.0 * (ball.y + _ballSize - _h);
		}
		if (ball.x + _ballSize > _w) //Rechter Rand
		{
			ball.vx *= -1.0f;
			ball.x -= 2.0 * (ball.x + _ballSize - _w);
		}
		if (ball.y - _ballSize < 0.0f) //oberer Rand
		{
			ball.vy *= -1.0f;
			ball.y -= 2.0 * (ball.y - _ballSize);
		}
		if (ball.x - _ballSize < 0.0f) // Linker Rand
		{
			ball.vx *= -1.0f;
			ball.x -= 2.0 * (ball.x - _ballSize);
		}
		//Maus erkennung
		/*float dist = d(currentPos.x(), currentPos.y(), ball.x, ball.y);
		//std::cout << dist << std::endl;
		if (dist >0 && dist < _ballSize*2) {
			const float slip = 0.6;
			float nx, ny, tx, ty;

			// normal
			nx = ball.x - lastPos.x();
			ny = ball.y - lastPos.y();
			normalize(nx, ny);

			// tangent pointing to the left of normal
			tx = -ny;
			ty = nx;

			float mvx = (currentPos.x() - lastPos.x());
			float mvy = (currentPos.y() - lastPos.y());
			// relative velocity
			float vsumx = mvx- ball.vx;
			float vsumy = mvy - ball.vy;

			// coordinates in radial tangential coordinate frame
			float vn = nx * vsumx + ny * vsumy;
			float vt = tx * vsumx + ty * vsumy;

			ball.x += nx * (vn + 0.1);
			ball.y += ny * (vn + 0.1);

			ball.vx += vn * nx;
			ball.vy += vn * ny;

			// Q_ASSERT(d(ball.x, ball.y, i.x, i.y) >= _ballsize + _ballsize); was ist das?
			//ball.omega = slip * -vt + ball.omega - _ballSize / _ballSize * currentBall.omega;
		}
		*/
		for (int i = 0; i < 16; i++)
		{
			Ball& currentBall = _balls[i];
			if ( index != i && currentBall.exists)
			{
				float dist = d(currentBall.x, currentBall.y, ball.x, ball.y);
				if (dist < _ballSize * 2) {
					//TODO Geschwindigkeit von beiden Kugeln ändern
					const float slip = 0.6;
					float nx, ny, tx, ty;

					// normal
					nx = ball.x - currentBall.x;
					ny = ball.y - currentBall.y;
					normalize(nx, ny);

					// tangent pointing to the left of normal
					tx = -ny;
					ty = nx;

					// relative velocity
					float vsumx = currentBall.vx - ball.vx;
					float vsumy = currentBall.vy - ball.vy;

					// coordinates in radial tangential coordinate frame
					float vn = nx * vsumx + ny * vsumy;
					float vt = tx * vsumx + ty * vsumy;

					ball.x += nx * (vn + 1.0f);
					ball.y += ny * (vn + 1.0f);

					ball.vx += vn * nx;
					ball.vy += vn * ny;

					// Q_ASSERT(d(ball.x, ball.y, i.x, i.y) >= _ballsize + _ballsize); was ist das?
					//ball.omega = slip * -vt + ball.omega - _ballSize / _ballSize * currentBall.omega;
				}
				
			}
			
		}
	}
}
void GLScene::updatePhysics()
{
	const float friction = 0.975;
	const float rfriction = 0.001;

	// update the rackets
	//Alle Kugeln Kollidieren lassen
	for (int i = 0; i < _balls.size(); i++)
	{
		updateBallCollision(_balls[i], i);
	}
	for (int i = 0; i < _balls.size(); i++)
	{
		updateBallVelocity(_balls[i]);
	}


}
void GLScene::initStandardBalls()
{
	_redBallHalf = loadTexture("C:\\Users\\fp16\\Documents\\Visual Studio 2013\\Projects\\AR_Billiard\\TouchScreen\\Debug\\textures\\BallRedFull.bmp");
	std::cout << "Starte init" << std::endl;
	for (int i = Yellow; i != Length; i++)
	{
		Color foo = static_cast<Color>(i);
		std::cout << "Current color: " << foo << std::endl;
		if (foo == White || foo == Black)
		{
			Ball currentBallHalf;
			currentBallHalf.color = foo;
			currentBallHalf.full = false;
		_balls.push_back(currentBallHalf);

		}
		else
		{
			Ball currentBallHalf;
			currentBallHalf.color = foo;
			currentBallHalf.full = false;
			_balls.push_back(currentBallHalf);
			Ball currentBallFull;
			currentBallFull.color = foo;
			currentBallFull.full = true;
			_balls.push_back(currentBallFull);
		}

	}
}
void GLScene::resetGame()
{
	//_balls.clear();
	if (!alreadyStarted) {
		initStandardBalls();
	}
	
	_balls[_balls.size() - 1].x = _w / 4;
	_balls[_balls.size() - 1].y = _h / 2 + (_ballSize);
//	for (int i = 0; i < 16; i++) {

	//}
	int currentPosition = 0;
	for (int i = 0; i <= 5; i++)
	{
		for (int j = 1; j <= i; j++)
		{
			float yOffset = -i + (_h / 2.0f - i / 2.0f * (_ballSize*2)); //Hälfte der Höhe, - hälfte der Anzahl der Kugeln mal die Größe der Kugeln
			
			_balls[currentPosition].x = _w / 3.0f * 2.0f + i * _ballSize*2; //Verschieben nach rechts von 3/4 der Width aus
			_balls[currentPosition].y = yOffset + j * (_ballSize+1.0f) *2;         //Verschieben nach unten/oben
			std::cout << "x: " << _balls[currentPosition].x << " y: " << _balls[currentPosition].y << std::endl;
			currentPosition++;
		}
	}
	
	/*
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
	*/
}

//Render eine Kugel mit ihren Parametern vor allem ihrer Farbe

GLuint GLScene::loadTexture(const char * filename) {
	GLuint texture;
	int width, height;

	unsigned char * data;

	FILE * file;

	file = fopen(filename, "rb"); 
	if (file == NULL) return 0;
	width = 128;
	height = 128;
	data = (unsigned char *)malloc(width * height * 3);
	//int size = fseek(file,);
	fread(data, width * height * 3, 1, file);
	fclose(file);
	for(int i = Color::Yellow; i < =)
	for (int i = 0; i < width * height; ++i)
	{
		int index = i * 3;
		unsigned char R,G,B;
		R = data[index];
		G = data[index + 1];
		B = data[index + 2];

		data[index] = G;
		data[index + 1] = R;
		data[index + 2] = B;

	}


	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	free(data);

	return texture;
}
void GLScene::renderBall(Ball const &ball)
{	
	GLfloat vertex[4];
	GLfloat texcoord[2];
	const int k = 128;
	const GLfloat delta_angle = 2.0*M_PI / static_cast<float>(k);
	
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(ball.x, ball.y, 0.0f);
	glRotatef( ball.angle, 0.0, 0.0, 1.0 );
	//glScalef(_ballSize, _ballSize, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _redBallHalf);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBegin(GL_TRIANGLE_FAN);
	texcoord[0] = -0.18;
	texcoord[1] = 0.5;
	glTexCoord2fv(texcoord);

	//glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i <= k; ++i)
	{
		float x = cos((delta_angle * static_cast<float>(i)))*_ballSize;
		float y = sin((delta_angle * static_cast<float>(i)))*_ballSize;
		texcoord[0] = (cos(delta_angle * static_cast<float>(i)) -0.36) *0.5;
		texcoord[1] = (sin(delta_angle * static_cast<float>(i)) +1.0) *0.5;
		glTexCoord2fv(texcoord);
		glVertex3f(x, y, 0.0f);

		/*
		switch (ball.color)
		{
		case (White) :

			glColor3f(1.0f, 1.0f, 1.0f);
			break;
		case (Black) :
			glColor3f(0.0f, 0.0f, 0.0f);
			break;
		case (Green) :
			glColor3f(0.0f, 1.0f, 0.0f);
			break;
		case (Red) :
			glColor3f(1.0f, 0.0f, 0.0f);
			break;
		case (Yellow) :
			glColor3f(1.0f, 1.0f, 0.0f);
			break;
		case (Brown) :
			glColor3f(0.7f, 0.0f, 0.0f);
			break;
		case (LightBlue) :
			glColor3f(0.2f, 0.8f, 1.0f);
			break;
		case (Orange) :
			glColor3f(1.0f, 0.6f, 0.0f);
			break;
		case (Blue) :
			glColor3f(0.0f, 0.0f, 1.0f);
		}
		*/
		//glColor3f(ball.color_r, ball.color_g, ball.color_b);
		
		//glColor3f(1.0f, 1.0f, 1.0f);
	}
	
	/*texcoord[0] = 1;
	texcoord[1] = 0.5;
	glTexCoord2fv(texcoord);

	vertex[0] = _ballSize;
	vertex[1] = 0.0;
	vertex[2] = 0.0;
	vertex[3] = 1.0;
	glVertex4fv(vertex);*/
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
	/*
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();*/
	glPopMatrix();
}
/*
void GLScene::renderPuck()
{

	glPushMatrix();
	glLoadIdentity();
	const int k = 32;
	glTranslatef(_puck.x, _puck.y, 0.0f);
	glRotatef(_puck.angle, 0.0, 0.0, 1.0);
	glScalef(_puckSize, _puckSize, 1.0f);

	glBegin(GL_TRIANGLE_FAN);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);

	for (int i = 0; i <= k; ++i)
	{
		float x = cos(2.0 * M_PI * static_cast<float>(i) / k);
		float y = sin(2.0 * M_PI * static_cast<float>(i) / k);
		glColor3f(1, 1, 1);
		glVertex3f(x, y, 0.0f);
	}
	glEnd();

	glPopMatrix();
}
*/
/*
void GLScene::renderRacket(Racket const &racket)
{
	glPushMatrix();
	glLoadIdentity();
	const int k = 32;
	glTranslatef(racket.x, racket.y, 0.0f);
	glRotatef(360 * racket.angle / (2 * M_PI), 0.0, 0.0, 1.0);
	glScalef(_racketSize, _racketSize, 1.0f);

	glBegin(GL_TRIANGLE_FAN);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);

	for (int i = 0; i <= k; ++i)
	{
		float x = cos(2.0f * M_PI * static_cast<float>(i) / k);
		float y = sin(2.0f * M_PI * static_cast<float>(i) / k);
		glColor3f(abs(x), 0.2f, abs(y));
		glVertex3f(x, y, 0.0f);
	}
	glEnd();

	glPopMatrix();
}
*/
