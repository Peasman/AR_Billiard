#ifndef GLSCENE_H
#define GLSCENE_H

#include <QtOpenGL/QGLWidget>
#include <QtWidgets/QShortcut>
#include <QEvent>
#include <QTouchEvent>
#include <QTimer>
#include <Camera.h>
#include <Detection.h>
#include <qlabel.h>
class GLScene : public QGLWidget
{

	Q_OBJECT

public:

	GLScene(QWidget *parent);
	struct Player
	{
		int num;
		bool ballType;
		bool colorSet = false;
	};

	public slots:
	void resetLabels();
	void hideLabels();
	void showLabel();
	int getCurrentPlayer();
	void testPauseByInput();
	void wait(int);
	Player getPlayers(int);
	void updateFrame();
	void resetGame();
	void enableMouse(bool);
	void startGame(bool);
	void changeCalibrateQuestionBool(bool);
	void initLabel();
	void setBallTypeLabel();
	void updateLabel();


private:
	QLabel *_currentPlayerLabel;
	QLabel *_playerHalf;
	QLabel *_playerFull;

protected:
	Detection det;
	Camera cam;
	bool _calibrateQuestion = true;
	void createChessboard();


	QPoint currentPos; // erste Mausposition
	QPoint lastPos; // letzte Mausposition

	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void updatePhysics();


	// Maus-Event-Methoden
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);


	QTimer *_timer;
	QShortcut *_shrtReset;
	GLuint  _texture;
	const bool FULL = true;
	const bool HALF = false;

	float _w, _h, _xpos, _ypos;

	enum Color{
		Yellow = 0, Blue, Brown, Purple, Orange, Green, Red, Black, White, Length
	};
	struct Pool
	{
		float leftBound, rightBound,              //Linkes Ende des Spielfelds/Pools immer 0, rechtes ist Fenstergröße bzw 1920 im Normalfall
			topBound, bottomBound,              //Oben immer 0 bzw (1080 - 960)/2 = 60 (für das 2:1 Verhältnis) und unten 1020 
			midX, midY;                         //Fenstergröße/2, wird benötigt für Start und Löcher

	};
	struct Hole
	{
		float x, y;
	};
	struct Ball
	{
		//glColor3f color;
		Color color;                        //Farbe
		bool full,                          //Halb oder ganz?
			exists = true;                        // Noch im spiel?
		int number;                         //Noetig? Nummer der Kugel 
		float x = 0, y = 0,                          //Position x und y der Kugel
			xLast = 0, yLast = 0,                  //Letzte Position
			vx = 0, vy = 0,                        //Momentane Geschwindigkeit
			omega = 0,                        //Momentane Winkelgeschwindigkeit
			angle = 0, angleLast = 0;              //Momentaner Winkel und letzter Winkel
	};
	struct Racket
	{
		float x, y,             // aktuelle position
			x2, y2,
			xLast, yLast,     // letzte position
			x2Last, y2Last,
			vx, vy,           // geschwindigkeit
			omega,            // winkelgeschwindigket der rotation in 2 * pi / sek
			// positive werte -> mathematisch positive rotation ( gegen den uhrzeiger )
			// negative werte -> mathematisch negative rotation ( mit dem uhrzeiger )
			angle, angleLast; // aktueller und letzter Winkel des Rackets
	};

	inline float d(float x1, float y1, float x2, float y2)
	{
		float dx = x1 - x2;
		float dy = y1 - y2;
		return sqrt(dx * dx + dy * dy);
	}
	inline void normalize(float &x, float &y)
	{
		float n = norm(x, y);
		x /= n;
		y /= n;
	}
	inline float dot(float x1, float y1, float x2, float y2)
	{
		return x1 * x2 + y1 * y2;
	}
	inline float norm(float x, float y)
	{
		return sqrt(x * x + y * y);
	}
	inline void scalarMult(float scalar, float& x, float& y)
	{
		x *= scalar;
		y *= scalar;
	}
	inline void rotate(float angle, float& x, float &y)
	{
		float xnew, ynew, cosa = cos(angle), sina = sin(angle);
		xnew = cosa * x - sina * y;
		ynew = sina * x + cosa * y;
		x = xnew;
		y = ynew;
	}
	Racket racket;
	Player players[2];
	bool again = false, definitlyNotAgain = false;
	int currentPlayer = 0;
	float _puckSize, _racketSize, _ballSize, _holeSize;
	bool turnRunning = false;
	int invalidFrames = 0;
	int maxInvalidFrames = 30;
	void renderBall(Ball const& ball);
	void renderRacket(float  x, float y, bool other);
	void updateBallVelocity(Ball& ball);
	void updateBallCollision(Ball& ball, int index);
	void CollisionWithHole(Ball& ball);
	void initStandardBalls();
	void loadTexture();
	void initHoles();
	void renderHole(Hole const &hole);
	void CollisionWithWall(Ball& ball);
	void CollisionWithMouse(Ball& ball);
	void nextPlayer();
	void CollisionWithRacket(Ball& ball, bool other);
	bool StillMoving();
	bool VerifyWin();
	bool BallTypeStillExists(bool ballType);
	const int _timerPeriod;
	std::vector<Hole> _holes;
	std::vector<Ball> _balls; //Liste der Kugeln die Momentan aktiv sind
	//Generell um Berechnungen während des Spielens zu verringern
	int _gameInit;
	static const float eps;
};

#endif