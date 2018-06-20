#include "TouchScreen.h"
#include "windows.h"
#include <iostream>
#include <QMenuBar>
#include "GLScene.h"

TouchScreen::TouchScreen(QWidget *parent)
    : QMainWindow(parent)
{

  ui.setupUi(this);
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

  _fileMenu = menuBar()->addMenu(tr("&File"));
  _optionMenu = menuBar()->addMenu(tr("&Option"));
  _helpMenu = menuBar()->addMenu(tr("&Help"));
 

  _scene = new GLScene( this );

  _gridLayout = new QGridLayout( ui.centralWidget );
  _gridLayout->addWidget( _scene, 0, 0, 1, 1 );

  createActions();
  _fileMenu->addAction(_startNewGame);
  _fileMenu->addAction(_resetGame);
  _fileMenu->addAction(_calibrate);
  _fileMenu->addSeparator();
  _fileMenu->addAction(_quitGame);

  _optionMenu->addAction(_mouseControll);
  _optionMenu->addAction(_shrtFullScreen);
 
  _helpMenu->addAction(_about);
  
}
TouchScreen::~TouchScreen()
{}

void TouchScreen::createActions()
{
	_mouseControll = new QAction(tr("&Enable Mouse"),this);
	_mouseControll->setChecked(false);
	_mouseControll->setCheckable(true);
	connect(_mouseControll, SIGNAL(triggered()), this, SLOT(enableMouseControll()));

	_resetGame = new QAction(tr("&Reset Game"), this);
	connect(_resetGame, SIGNAL(triggered()), this, SLOT(resetGame()));

	_startNewGame = new QAction(tr("&Start New Game"), this);
	connect(_startNewGame, SIGNAL(triggered()), this, SLOT(startGame()));

	_calibrate = new QAction(tr("&Calibrate"), this);
	connect(_calibrate, SIGNAL(triggered()), this, SLOT(calibrate()));

	_about = new QAction(tr("&About AR_Biliard"), this);
	connect(_about, SIGNAL(triggered()), this, SLOT(showAbout()));

	_shrtFullScreen = new QAction(tr("&Fullscreen"), this);
	connect(_shrtFullScreen, SIGNAL(triggered()), this, SLOT(toggleFullScreen()));

	_shrtQuit = new QShortcut(QKeySequence("Ctrl+Q"), this);
	connect(_shrtQuit, SIGNAL(activated()), QApplication::instance(), SLOT(quit()));

	_quitGame = new QAction(tr("&Close Game"), this);
	connect(_quitGame, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));

}

void TouchScreen::showAbout(){
	const int result = MessageBox(nullptr, TEXT("Made by: \nMoritz Ludolf \nRobin Mertens \nFriedemann Runte\nDiyar Omar"), TEXT("About AR_Biliard"), MB_OK);
}

//CONTEXTMENU EVENTUELL NICHT BENÖTIGT
void TouchScreen::contextMenuEvent(QContextMenuEvent *event){}

void TouchScreen::enableMouseControll()
{
	if (_mouseControll->isChecked())
	{
		_mouseControll->setText(tr("&Dissable Mouse"));
		_mouseControll->setChecked(true);

		//funcion für die GLSCENE
		_mouseFunction = true;
		_scene->enableMouse(_mouseFunction);

	}
	if (!_mouseControll->isChecked()){
		_mouseControll->setText(tr("&Enable Mouse"));
		_mouseControll->setChecked(false);

		//function für die GLSCENE
		_mouseFunction = false;
		_scene->enableMouse(_mouseFunction);
	}
}

void TouchScreen::startGame()
{
	//BOOL von GLScene ändern in True, wenn das Spiel gestartet wird.
	_startGame = true;
	_scene->startGame(_startGame);
}

void TouchScreen::resetGame()
{
	const int result = MessageBox(nullptr, TEXT("Do you want to reset the game?"), TEXT("Reset Game"), MB_YESNO);
	switch (result)
	{
		case IDYES:
			// RESET GAME WIRD AUSGEFÜHRT
			_scene->resetGame();
			break;
	}
	
}
bool b1 = true;
void TouchScreen::calibrate(){
	std::cout << "GLScene: Calibrate start" << std::endl;
	_scene->changeCalibrateQuestionBool(b1);
	//_scene->startCalibrate();
}

void TouchScreen::toggleFullScreen()
{
	if (isFullScreen()){
		_shrtFullScreen->setText("&Fullscreen");
		_scene->changeCalibrateQuestionBool(b1);
		showNormal();
	}
	else{
		_shrtFullScreen->setText("&Normalscreen");
		_scene->changeCalibrateQuestionBool(b1);
		showFullScreen();
	}
}

