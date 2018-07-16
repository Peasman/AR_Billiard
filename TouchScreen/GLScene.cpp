#include "GLScene.h"

#include <QMetaEnum>
#include <iostream>
#include <vector>
#include <QtWidgets/QApplication>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL\glut.h>
#include <time.h>

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
}
//TODO ?
void initGame()
{

}
bool alreadyStarted = false;
bool isMouseEnable = false;

//Die Funktion der Mouse wird aktiviert
void GLScene::enableMouse(bool isEnable) {
	isMouseEnable = isEnable;
}
//========================================================================================
// Event für das Drücken einer Maustaste
//========================================================================================
void GLScene::mousePressEvent(QMouseEvent *event)
{
	if (isMouseEnable) {
		currentPos = event->pos();
	}
}
//========================================================================================
// Event für Mausbewegungen
//========================================================================================
void GLScene::mouseMoveEvent(QMouseEvent *event)
{
	if (isMouseEnable) {
		lastPos = currentPos;
		float currentx = event->pos().x();
		float currenty = _h - event->pos().y() - 30;
		currentPos = QPoint(currentx, currenty);
		//std::cout << "x: " << currentx << ", y: " << currenty << std::endl;
	}
}
//========================================================================================
// Event für das Loslassen einer Maustaste
//========================================================================================
void GLScene::mouseReleaseEvent(QMouseEvent *event)
{
	if (isMouseEnable) {

	}
}

void GLScene::initLabel(){
	QPalette sample_palet;
	sample_palet.setColor(QPalette::Window, QColor(1.0f, 0.4f, 0.0f, 1.0f));
	sample_palet.setColor(QPalette::WindowText, Qt::white);
	sample_palet.setColor(QPalette::Background, QColor::fromRgba(qRgba(0, 102, 0, 255)));


	QString inputCurrentPlayer = "CurrentPlayer: "+QString::number(getCurrentPlayer());
	_currentPlayerLabel = new QLabel(inputCurrentPlayer, this);
	_currentPlayerLabel->setGeometry(QRect(200, 0, 100, 25));

	QString inputFull = "BallType of Player: ";
	_playerFull = new QLabel(inputFull, this);
	_playerFull->setGeometry(QRect(300, 0, 150, 25));

	QString inputHalf = "BallType of Player: ";
	_playerHalf = new QLabel(inputHalf, this);
	_playerHalf->setGeometry(QRect(445, 0, 150, 25));

	_currentPlayerLabel->setAutoFillBackground(true);
	_playerFull->setAutoFillBackground(true);
	_playerHalf->setAutoFillBackground(true);

	_currentPlayerLabel->setPalette(sample_palet);
	_playerFull->setPalette(sample_palet);
	_playerHalf->setPalette(sample_palet);
}

void GLScene::setBallTypeLabel(){
	if (getPlayers(getCurrentPlayer()).colorSet){
		if (getPlayers(getCurrentPlayer()).ballType){
			if (getCurrentPlayer() == 0){
				QString inputPlayer0 = "BallType of Player 0 : FULL";
				_playerFull->setText(inputPlayer0);

				QString inputPlayer1 = "BallType of Player 1: HALF";
				_playerHalf->setText(inputPlayer1);
			}
			else{
				QString inputPlayer0 = "BallType of Player 0 : HALF";
				_playerFull->setText(inputPlayer0);

				QString inputPlayer1 = "BallType of Player 1: FULL";
				_playerHalf->setText(inputPlayer1);
			}
		}
		else{
			if (getCurrentPlayer() == 1){
				QString inputPlayer0 = "BallType of Player 0 : HALF";
				_playerFull->setText(inputPlayer0);

				QString inputPlayer1 = "BallType of Player 1: FULL";
				_playerHalf->setText(inputPlayer1);
			}
			else{
				QString inputPlayer0 = "BallType of Player 0 : FULL";
				_playerFull->setText(inputPlayer0);

				QString inputPlayer1 = "BallType of Player 1: HALF";
				_playerHalf->setText(inputPlayer1);
			}
		}
	}
}

void GLScene::updateLabel(){
	QString inputCurrentPlayer = "CurrentPlayer: " + QString::number(getCurrentPlayer());
	_currentPlayerLabel->setText(inputCurrentPlayer);
}

int GLScene::getCurrentPlayer(){
	return currentPlayer;
}
GLScene::Player GLScene::getPlayers(int curPlayer){
	return players[curPlayer];
}

void GLScene::startGame(bool gameStarted) {
	if (!alreadyStarted) {

		alreadyStarted = gameStarted;
		resetGame();
	}
	else {
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

void GLScene::testPauseByInput() {
	std::cout << "Press any key to continue .." << std::endl;
	std::cin.get();
}

void GLScene::wait(int seconds)
{
	clock_t endwait;
	endwait = clock() + seconds * CLOCKS_PER_SEC;
	while (clock() < endwait) {}
}

void GLScene::updateFrame()
{
	//Falls _calibrateQuestion==true -> zeige beim naechsten Frame MsgBox an
	if (_calibrateQuestion) {
		_calibrateQuestion = false;
		int result = MessageBox(nullptr, TEXT("Want to calibrate?"), TEXT("Message"), MB_YESNO);
		switch (result)
		{
		case IDNO:
			return;
		case IDYES:
			hideLabels();
			std::cout << std::endl;
			std::cout << "GLScene: Calibration start" << std::endl;
			cam.startCalibration();
			return;
		}
	}
	if (!cam.getCalibStatus()){//Falls Kalibrierung nicht läuft
		if (!alreadyStarted) {//und Spiel noch nicht gestartet ist
			// -> MsgBox
			int result = MessageBox(nullptr, TEXT("Start a new game?"), TEXT("Message"), MB_YESNO);
			switch (result)
			{
			case IDNO: //Ende
				QApplication::quit();
				break;
			case IDYES: //Neues Spiel starten
				alreadyStarted = true;
				showLabel();
				resetGame();
				return;
			}
		}

		cv::Mat img = cam.capture();
		cv::Mat flipped;
		cv::flip(img, flipped, 1);
		det.detectCue(img);

		if (det._valid){
			cv::Point2f curr1 = cam.camera2world(det._curr_x_1, det._curr_y_1);
			cv::Point2f curr2 = cam.camera2world(det._curr_x_2, det._curr_y_2);
			cv::Point2f last1 = cam.camera2world(det._last_x_1, det._last_x_1);
			cv::Point2f last2 = cam.camera2world(det._last_x_2, det._last_x_2);

			invalidFrames = 0;
			racket.x = curr1.x;
			racket.y = curr1.y;
			racket.x2 = curr2.x;
			racket.y2 = curr2.y;
			racket.xLast = last1.x;
			racket.yLast = last1.y;
			racket.x2Last = last2.x;
			racket.y2Last = last2.y;
		}
		else
		{
			invalidFrames++;
		}
		if (invalidFrames > maxInvalidFrames)
		{
			racket.vx = 0;
			racket.vy = 0;
			racket.x = 0;
			racket.y = 0;
			racket.x2 = 0;
			racket.y2 = 0;
			racket.xLast = 0;
			racket.yLast = 0;
			racket.x2Last = 0;
			racket.y2Last = 0;
		}
		updatePhysics();
		update();
	}
}
void GLScene::hideLabels(){
	_currentPlayerLabel->hide();
	_playerFull->hide();
	_playerHalf->hide();
}
void GLScene::showLabel(){
	_currentPlayerLabel->show();
	_playerFull->show();
	_playerHalf->show();
}
void GLScene::initializeGL()
{
	initLabel();
	hideLabels();
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
	_h = 0.5f * w; //static_cast<float>(h);
	_ballSize = _h / 38.5f;
	_holeSize = _ballSize * 2;
	for (int i = 0; i < 16; i++) {
		Ball &ball = _balls[i];
		ball.x = ball.x * scaleW;
		ball.y = ball.y * scaleW *0.5f;
	}
	for (int i = 0; i < 6; i++) {
		Hole &hole = _holes[i];
		hole.x = hole.x * scaleW;
		hole.y = hole.y * scaleW * 0.5f;
	}
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, _w, 0.0f, _h, 1.0f, -1.0f);
}

void GLScene::changeCalibrateQuestionBool(bool value) {
	_calibrateQuestion = value;
}

void GLScene::createChessboard() {
	bool color = true;
	int w = _w;
	int h = _h;
	// Anzahl kacheln berechnen für Breite und Höhe
	int hor, vert;
	hor = 16;
	vert = 8;
	// Seitenlänge pro Kachel
	float slh = h / vert;
	float slw = w / hor;
	//for each width and height draw a rectangle with a specific color
	for (int i = 0; i < hor; ++i) {
		for (int j = 0; j < vert; ++j) {
			//Farbe pro Kachel auf dem Schachbrett
			if (color)
				glColor3f(1, 1, 1);
			else
				glColor3f(0, 0, 0);
			color = !color;
			//draw a rectangle in the i-th row and j-th column
			glRecti(i*slw, j*slh, (i + 1)*slw, (j + 1)*slh);
		}
		if (vert % 2 == 0) color = !color; //switch color order at end of row if necessary
	}
}

void GLScene::paintGL()
{
	if (cam.getCalibStatus()){ // Schachbrett anzeigen/rendern
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		createChessboard();
		return;
	}
	if (alreadyStarted && !cam.getCalibStatus())
	{ // Farbe Spielfeld GREEN
		// Fensterinhalt loeschen
		glClearColor(0.0f, 0.4f, 0.0f, 1.0f);               // Loeschfarbe setzen auf Billiard Pool Farbe
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Farb und Tiefenpuffer loeschen

		// zur ModelView-Matrix wechseln
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity(); // Identitaetsmatrix laden

		if (det._valid){
			renderRacket(racket.x, racket.y, true);
			renderRacket(racket.x2, racket.y2, false);
		}
		for (int i = 0; i < _balls.size(); i++)
		{
			renderBall(_balls[i]);
		}
		for (int i = 0; i < _holes.size(); i++)
		{
			renderHole(_holes[i]);
		}
		return;
	}
	if (!alreadyStarted && !cam.getCalibStatus()){
		// Start des Programms Farbe setzen BLACK
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Farb und Tiefenpuffer l�schen
		// zur ModelView-Matrix wechseln
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity(); // Identit�tsmatrix laden
		return;
	}

}

void GLScene::initHoles() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			Hole hole;
			hole.x = i * (_w / 2.0f);
			hole.y = j * _h;
			std::cout << "Hole x: " << hole.x << " Hole y: " << hole.y << std::endl;
			std::cout << "w: " << _w << "_h " << _h << std::endl;
			_holes.push_back(hole);
		}
	}
}
void GLScene::updateBallVelocity(Ball& ball)
{

	const float friction = 0.98f;
	const float rfriction = 0.99f;
	rotate(ball.omega, ball.vx, ball.vy);
	ball.x = ball.x + ball.vx;
	ball.y = ball.y + ball.vy;
	ball.angle = ball.angle + ball.omega;

	// introduce some friction so game stays playable
	ball.vx *= friction;
	ball.vy *= friction;
	ball.omega *= rfriction;
}
void GLScene::CollisionWithHole(Ball& ball)
{
	for (int i = 0; i < 6; i++) {
		float dX = ball.x - _holes[i].x;
		float dY = ball.y - _holes[i].y;

		if (sqrt(dX * dX + dY * dY) < _holeSize && ball.exists)
		{
			std::cout << "Getroffen! " << std::endl;
			// Kugel verschwinden lassen
			ball.exists = false;
			//Wenn die Schwarze Kugel im Loch landet
			if (ball.color == Color::Black)
			{
				if (VerifyWin())
				{
					std::cout << "Schwarze Kugel von Spieler " << currentPlayer << " und dadurch gewonnen! " << std::endl;

					//TODO Win currentPlayer
					std::string curPlayer = std::to_string(currentPlayer);
					std::string player = std::string("Player: ");
					std::string hasWon = std::string(" has WON!");
					std::string mixed = std::string(player + curPlayer + hasWon);
					MessageBoxA(NULL, mixed.c_str(), "WON!", MB_OK);

					int result = MessageBox(nullptr, TEXT("Start a new game?"), TEXT("Message"), MB_YESNO);
					switch (result)
					{
					case IDNO:
						QApplication::quit();
						break;
					case IDYES:

						alreadyStarted = true;
						resetGame();
						break;
					}

				}
				else
				{
					std::cout << "Schwarze Kugel von Spieler " << currentPlayer << " und dadurch verloren... " << std::endl;

					//TODO Lose currentPlayer
					std::string curPlayer = std::to_string(currentPlayer);
					std::string player = std::string("Player: ");
					std::string hasWon = std::string(" has LOST!");
					std::string mixed = std::string(player + curPlayer + hasWon);
					MessageBoxA(NULL, mixed.c_str(), "LOST!", MB_OK);

					//TODO Lose currentPlayer
					int result = MessageBox(nullptr, TEXT("Start a new game?"), TEXT("Message"), MB_YESNO);
					switch (result)
					{
					case IDNO:
						QApplication::quit();
						break;
					case IDYES:

						alreadyStarted = true;
						resetGame();
						break;
					}
				}
			}
			if (ball.color == Color::White) {
				definitlyNotAgain = true;
				ball.x = _w / 4;
				ball.y = _h / 2;
				ball.vx = 0;
				ball.vy = 0;
				ball.exists = true;
				nextPlayer();
				return;
			}
			//Hat der Spieler die Richtige Farbe rein gemacht?
			if (!players[currentPlayer].colorSet) {
				players[currentPlayer].colorSet = true;
				players[currentPlayer].ballType = ball.full;
				int otherPlayerPosition = (currentPlayer + 1) % 2;
				players[otherPlayerPosition].colorSet = true;
				players[otherPlayerPosition].ballType = !ball.full;
				setBallTypeLabel();
				std::cout << "Farbe angepasst auf " << players[currentPlayer].ballType << std::endl;
			}
			if (players[currentPlayer].ballType == ball.full)
			{
				//TODO Ja Dann Spieler weiterhin dran
				std::cout << "Getroffen von Spieler " << currentPlayer << std::endl;
				again = true;
			}
			else
			{
				std::cout << "Getroffen! Aber falsche Art von Spieler " << currentPlayer << std::endl;
				definitlyNotAgain = true;
				//Nein dann nächster Spieler 
				//TODO Listener für PlayerWechsel DIKO
				//currentPlayer = (currentPlayer + 1) % 2;
				nextPlayer();
			}
		}
	}
}
//Check wenn die Schwarze Kugel eingelocht wurde ob gewonnen oder verloren wurde.
bool GLScene::VerifyWin()
{
	return BallTypeStillExists(players[currentPlayer].ballType);
}
void GLScene::nextPlayer() {
	if (turnRunning){
		if (!StillMoving()) {
			if (again && !definitlyNotAgain)
			{
				currentPlayer = (currentPlayer + 1) % 2;
				std::cout << "Spieler Wechsel" << std::endl;
				updateLabel();
			}
			else {
				std::cout << "Nochmal dran" << std::endl;
			}
			again = false;
			definitlyNotAgain = false;
			turnRunning = false;
		}
	}
}
bool GLScene::BallTypeStillExists(bool ballType)
{
	//Alle Kuglen außer der Weißen und Schwarzen
	for (int i = 0; i < 14; i++)
	{
		//Wenn die Art noch übrig ist dann true 
		if (_balls[i].full == ballType)
		{
			return true;
		}
	}
	return false;
}

void GLScene::CollisionWithWall(Ball& ball) {


	if (ball.y + _ballSize > _h) // unterer Rand
	{
		if ((ball.x >= _holeSize * 1.5f && ball.x <= _w / 2 - _holeSize * 1.5f)
			|| ball.x >= _w / 2 + _holeSize * 1.5f && ball.x <= _w - _holeSize * 1.5f) {
			ball.vy *= -1.0f;
			ball.y -= 2.0 * (ball.y + _ballSize - _h);
		}
	}
	if (ball.x + _ballSize > _w) //Rechter Rand
	{
		if (ball.y <= _h - _holeSize * 1.5f && ball.y >= _holeSize * 1.5f) {
			ball.vx *= -1.0f;
			ball.x -= 2.0 * (ball.x + _ballSize - _w);
		}

	}
	if (ball.y - _ballSize < 0.0f) //oberer Rand
	{
		if ((ball.x >= _holeSize * 1.5f && ball.x <= _w / 2 - _holeSize * 1.5f)
			|| ball.x >= _w / 2 + _holeSize * 1.5f && ball.x <= _w - _holeSize * 1.5f) {
			ball.vy *= -1.0f;
			ball.y -= 2.0 * (ball.y - _ballSize);
		}
	}
	if (ball.x - _ballSize < 0.0f) // Linker Rand
	{
		if (ball.y <= _h - _holeSize * 1.5f && ball.y >= _holeSize * 1.5f) {
			ball.vx *= -1.0f;
			ball.x -= 2.0 * (ball.x - _ballSize);
		}
	}

}
//Kollisionserkennung für die Maus
void GLScene::CollisionWithMouse(Ball& ball)
{	
	//float dist = d(currentPos.x(), currentPos.y(), ball.x, ball.y);
	
	float lastx = static_cast<float>(lastPos.x()), lasty = static_cast<float>(lastPos.y());
	float mousex = static_cast<float>(currentPos.x()), mousey = static_cast<float>(currentPos.y());
	
	float x = mousex;
	float y = mousey;
	float xLast = lastx;
	float yLast = lasty;

	if (false){

		// d = xy - xyLast # direction vector from xyLast to xy
		// f = xyLast - center # direction vector from center to xyLast
		float dx = x - xLast, dy = y - yLast;
		float fx = xLast - ball.x, fy = yLast - ball.y;

		// a = np.dot(d, d)
		// b = 2 * np.dot(d, f)
		// c = np.dot(f, f) - radius**2
		// dis = b**2 - 4 * a*c


		float a = dot(dx, dy, dx, dy);
		float b = 2 * dot(dx, dy, fx, fy);
		float c = dot(fx, fy, fx, fy) - _ballSize*_ballSize;

		float dis = b*b - 4 * a * c;

		if (dis < 0){
			// no collision
			return;
		}
		dis = sqrt(dis);
		float t1 = (-b - dis) / (2.0f * a); // always the first point
		float t2 = (-b + dis) / (2.0f * a);
	
		if (t1 >= 0 && t1 <= 1){
			// calculate collision point
			x = xLast + t1 * dx;
			y = yLast + t1 * dy;

			const float slip = 0.1;
			float nx, ny, tx, ty;

			// normal
			nx = ball.x - x;
			ny = ball.y - y;
			normalize(nx, ny);

			// tangent pointing to the left of normal
			tx = -ny;
			ty = nx;

			//TODO eher einfach vx/vy direkt im RacketUpdate berechnen?
			float mvx = (x - xLast);
			float mvy = (y - yLast);
			// relative velocity
			float vsumx = mvx - ball.vx;
			float vsumy = mvy - ball.vy;

			// coordinates in radial tangential coordinate frame
			float vn = nx * vsumx + ny * vsumy;
			float vt = tx * vsumx + ty * vsumy;

			ball.x += nx * (vn + 0.1);
			ball.y += ny * (vn + 0.1);

			ball.vx += vn * nx;
			ball.vy += vn * ny;
			turnRunning = true;
			if (t2 > 1){
				// Poke
			}
			else{
				// Impale
			}
		}
		else if (t2 >= 0 && t2 <= 1){
			// Exit Wound
		}
		else{
			// FallShort, Past, CompletelyInside"
		}

	}
	else{
		float dist = d(mousex, mousey, ball.x, ball.y);
		if (dist < _ballSize) {
			//TODO Geschwindigkeit von beiden Kugeln ändern
			const float slip = 0.1;
			float nx, ny, tx, ty;

			// normal
			nx = ball.x - x;
			ny = ball.y - y;
			normalize(nx, ny);

			// tangent pointing to the left of normal
			tx = -ny;
			ty = nx;

			//TODO eher einfach vx/vy direkt im RacketUpdate berechnen?
			float mvx = (x - xLast);
			float mvy = (y - yLast);
			// relative velocity
			float vsumx = mvx - ball.vx;
			float vsumy = mvy - ball.vy;

			// coordinates in radial tangential coordinate frame
			float vn = nx * vsumx + ny * vsumy;
			float vt = tx * vsumx + ty * vsumy;

			ball.x += nx * (vn + 0.1);
			ball.y += ny * (vn + 0.1);

			ball.vx += vn * nx;
			ball.vy += vn * ny;
		}
	}

}
//Kollisionserkennung für den Kö
//TODO immer erkennen oder nur wenn kein Ball sich bewegt?
void GLScene::CollisionWithRacket(Ball& ball, bool other)
{
	float x = (other) ? racket.x2 : racket.x;
	float y = (other) ? racket.y2 : racket.y;
	float xLast = (other) ? racket.x2Last : racket.xLast;
	float yLast = (other) ? racket.y2Last : racket.yLast;


	// d = xy - xyLast # direction vector from xyLast to xy
	// f = xyLast - center # direction vector from center to xyLast
	float dx = x - xLast, dy = y - yLast;
	float fx = xLast - ball.x, fy = yLast - ball.y;

	// a = np.dot(d, d)
	// b = 2 * np.dot(d, f)
	// c = np.dot(f, f) - radius**2
	// dis = b**2 - 4 * a*c

	float a = dot(dx, dy, dx, dy);
	float b = 2 * dot(dx, dy, fx, fy);
	float c = dot(fx, fy, fx, fy) - _ballSize*_ballSize;

	float dis = b*b - 4 * a * c;

	if (dis < 0){
		// no collision
		return;
	}
	dis = sqrt(dis);
	float t1 = (-b - dis) / (2.0f * a); // always the first point
	float t2 = (-b + dis) / (2.0f * a);


	if (t1 > 0 && t1 <= 1){
		// calculate collision point
		x = xLast + t1 * dx;
		y = yLast + t1 * dy;

		const float slip = 0.1;
		float nx, ny, tx, ty;

		// normal
		nx = ball.x - x;
		ny = ball.y - y;
		normalize(nx, ny);

		// tangent pointing to the left of normal
		tx = -ny;
		ty = nx;

		//TODO eher einfach vx/vy direkt im RacketUpdate berechnen?
		float mvx = (x - xLast);
		float mvy = (y - yLast);
		// relative velocity
		float vsumx = mvx - ball.vx;
		float vsumy = mvy - ball.vy;

		// coordinates in radial tangential coordinate frame
		float vn = nx * vsumx + ny * vsumy;
		float vt = tx * vsumx + ty * vsumy;

		ball.x += nx * (vn + 0.1);
		ball.y += ny * (vn + 0.1);

		ball.vx += vn * nx;
		ball.vy += vn * ny;
		turnRunning = true;
		if (t2 > 1){
			// Poke
		}
		else{
			// Impale
		}
	}
	else if (t2 >= 0 && t2 <= 1){
		// Exit Wound
	}
	else{
		// FallShort, Past, CompletelyInside"
	}


	// Q_ASSERT(d(ball.x, ball.y, i.x, i.y) >= _ballsize + _ballsize); was ist das?
	//TODO Rotation fixen/ausprobieren bei kö
	//ball.omega = slip * -vt + ball.omega - _ballSize / _ballSize * ball.omega;
}

//Überprüft ob sich noch Kugeln bewegen / ob schon der andere Spieler anfangen darf
/*
* Gibt False zurück wenn sich keine Kugel mehr bewegt.True wenn sich noch mindestens eine bewegt
* Kugeln die nicht mehr im Spiel sind werden ignoriert
*/
bool GLScene::StillMoving()
{
	for (int i = 0; i < 16; i++)
	{
		if (_balls[i].exists)
		{
			//TODO Threshold anpassen
			if (_balls[i].vx > 0.5f || _balls[i].vy > 0.5f)
			{
				return true;
			}
		}
	}
	return false;
}
void GLScene::updateBallCollision(Ball& ball, int index)
{

	//TODO Collision mit Löchern
	CollisionWithHole(ball);

	if (ball.exists)
	{
		CollisionWithWall(ball);
		//Maus erkennung
		for (int i = 0; i < 16; i++)
		{
			Ball& currentBall = _balls[i];
			if (index != i && currentBall.exists)
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
					float vt = tx * vsumx / 2 + ty * vsumy / 2;

					ball.x += nx * (vn + 1.0f);
					ball.y += ny * (vn + 1.0f);

					ball.vx += vn * nx;
					ball.vy += vn * ny;

					// Q_ASSERT(d(ball.x, ball.y, i.x, i.y) >= _ballsize + _ballsize); was ist das?
					//ball.omega = slip * -vt + ball.omega -  currentBall.omega;
					//ball.omega =  slip * -vt + ball.omega + currentBall.omega;
				}

			}

		}
		
		//TODO Checken ob das so funzt (Nur Kollision mit weißer kugel möglich/ keine Fouls in dem Sinne möglich
		if (!StillMoving() && ball.color == Color::White)
		{
			CollisionWithMouse(ball);
			if (invalidFrames < maxInvalidFrames){
				CollisionWithRacket(ball, false);
				CollisionWithRacket(ball, true);
			}
		}
	}
}
void GLScene::updatePhysics()
{
	const float friction = 0.9999;
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
	loadTexture();
	for (int i = Yellow; i != Length; i++)
	{
		Color foo = static_cast<Color>(i);

		if (foo == White)
		{
			Ball currentBallHalf;
			currentBallHalf.color = foo;
			currentBallHalf.full = false;
			_balls.push_back(currentBallHalf);

		}
		else {
			if (foo == Black) {
				Ball currentBallFull;
				currentBallFull.color = foo;
				currentBallFull.full = true;
				_balls.push_back(currentBallFull);
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
}

void GLScene::resetGame()
{
	//_balls.clear();
	if (!alreadyStarted) {
		initStandardBalls();
		initHoles();
	}

	int size = _balls.size();
	Ball &whiteball = _balls[size - 1];
	whiteball.x = _w / 4.0f;
	whiteball.y = _h / 2.0f + (_ballSize);

	int currentPosition = 0;
	for (int i = 0; i <= 5; i++)
	{
		for (int j = 0; j < i; j++)
		{
			float yOffset = -i + (_h / 2.0f - i / 2.0f * (_ballSize * 2)); //Hälfte der Höhe, - hälfte der Anzahl der Kugeln mal die Größe der Kugeln

			_balls[currentPosition].x = _w / 3.0f * 2.0f + i * _ballSize * 2; //Verschieben nach rechts von 3/4 der Width aus
			_balls[currentPosition].y = yOffset + (j + 1) * (_ballSize + 1.0f) * 2;         //Verschieben nach unten/oben

			std::cout << "x: " << _balls[currentPosition].x << " y: " << _balls[currentPosition].y << std::endl;
			currentPosition++;
			_balls[currentPosition].vx = 0;
			_balls[currentPosition].vy = 0;
			_balls[currentPosition].exists = true;
		}
	}
	//swap position so black ball is middle
	Ball &blackball = _balls[size - 2];
	Ball &otherball = _balls[4];
	float blackx = blackball.x;
	float blacky = blackball.y;
	float otherx = otherball.x;
	float othery = otherball.y;
	blackball.x = otherx;
	blackball.y = othery;
	otherball.x = blackx;
	otherball.y = blacky;
	racket.angle = 0;
	racket.angleLast = 0;
	racket.omega = 0;
	racket.vx = 0;
	racket.vy = 0;
	racket.x = 0;
	racket.y = 0;
	racket.x2 = 0;
	racket.y2 = 0;
	racket.xLast = 0;
	racket.yLast = 0;
	racket.x2Last = 0;
	racket.y2Last = 0;
}

//Render eine Kugel mit ihren Parametern vor allem ihrer Farbe

void GLScene::loadTexture() {
	//GLuint texture;
	int width, height, fullwidth, fullheight;
	unsigned char * data;

	//TODO Richtiger Filename
	const char * textureName = "C:\\Users\\fp19\\Documents\\Fachprojekt\\AR_Billiard\\TouchScreen\\textures\\Balls.bmp";
	FILE * fullFile;
	fullFile = fopen(textureName, "rb");
	//file = fopen(filename, "rb"); 
	if (fullFile == NULL) {
		std::cout << "Texture not found" << std::endl;
		return;
	}
	width = 800;
	height = 200;
	data = (unsigned char *)malloc(width * height * 3);
	//int size = fseek(file,);
	fread(data, width * height * 3, 1, fullFile);
	fclose(fullFile);
	//Full Balls:
	for (int j = 0; j < width * height; ++j)

	{


		int index = j * 3;
		unsigned char R, G, B;
		R = data[index];
		G = data[index + 1];
		B = data[index + 2];

		data[index] = G;
		data[index + 1] = R;
		data[index + 2] = B;

	}
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

	//Half Balls:
	free(data);

}
void GLScene::renderHole(Hole const &hole) {
	GLfloat vertex[4];
	GLfloat texcoord[2];
	const int k = 256;
	const GLfloat delta_angle = 2.0*M_PI / static_cast<float>(k);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(hole.x, hole.y, 0.0f);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	for (int i = 0; i <= k; ++i)
	{
		float x = cos((delta_angle * static_cast<float>(i)))*_holeSize;
		float y = sin((delta_angle * static_cast<float>(i)))*_holeSize;
		glVertex3f(x, y, 0.0f);
	}
	glEnd();
	glPopMatrix();
}

void GLScene::renderBall(Ball const &ball)
{

	if (!ball.exists) {
		return;
	}
	GLfloat vertex[4];
	GLfloat texcoord[2];
	const int k = 256;
	const GLfloat delta_angle = 2.0*M_PI / static_cast<float>(k);

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(ball.x, ball.y, 0.0f);
	glRotatef(ball.angle, 0.0, 0.0, 1.0);
	//glScalef(_ballSize, _ballSize, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBegin(GL_TRIANGLE_FAN);
	float currentx;
	float currenty;
	if (ball.color == Color::White) {
		currenty = 0.25;
		currentx = -0.0115f + (1.0f / 8.0f) * 8;
	}
	else {
		if (ball.color == Color::Black) {
			currenty = 0.75;
			currentx = -0.0115f + (1.0f / 8.0f) * 8;
		}
		else {
			currenty = 0.25f + (0.5f * (1 - ball.full));
			currentx = -0.0115f + (1.0f / 8.0f) * (ball.color + 1);
		}
	}
	texcoord[0] = currentx;
	texcoord[1] = currenty;
	glTexCoord2fv(texcoord);
	glVertex3f(0.0f, 0.0f, 0.0f);

	for (int i = 0; i <= k; ++i)
	{
		float x = cos((delta_angle * static_cast<float>(i)))*_ballSize;
		float y = sin((delta_angle * static_cast<float>(i)))*_ballSize;

		texcoord[0] = currentx + (cos(delta_angle * static_cast<float>(i))) *1.0f / 16.0f;
		texcoord[1] = currenty + (sin(delta_angle * static_cast<float>(i))) *1.0f / 4.0f;
		glTexCoord2fv(texcoord);
		glVertex3f(x, y, 0.0f);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void GLScene::renderRacket(float x, float y, bool other){
	GLfloat vertex[4];
	GLfloat texcoord[2];
	const int k = 256;
	const GLfloat delta_angle = 2.0*M_PI / static_cast<float>(k);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, 0.0f);
	glBegin(GL_TRIANGLE_FAN);
	if (other)
		glColor3f(1.0f, 0.0f, 0.0f);
	else
		glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	for (int i = 0; i <= k; ++i)
	{
		float x = cos((delta_angle * static_cast<float>(i)))*_ballSize / 4;
		float y = sin((delta_angle * static_cast<float>(i)))*_ballSize / 4;
		glVertex3f(x, y, 0.0f);
	}
	glEnd();
	glPopMatrix();

}