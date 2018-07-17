#include "TouchScreen.h"
#include "windows.h"
#include <iostream>
#include <QMenuBar>
#include "GLScene.h"
#include <string>
#include <QString>

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
  _fileMenu->addSeparator();
  _fileMenu->addAction(_quitGame);

  _optionMenu->addAction(_mouseControll);
 
  _helpMenu->addAction(_about);
  _helpMenu->addAction(_showTips);
}
TouchScreen::~TouchScreen()
{}

void TouchScreen::createActions()
{
	_mouseControll = new QAction(tr("&Enable Mouse"),this);
	_mouseControll->setChecked(false);
	_mouseControll->setCheckable(true);
	connect(_mouseControll, SIGNAL(triggered()), this, SLOT(enableMouseControll()));

	_startNewGame = new QAction(tr("&Start New Game"), this);
	connect(_startNewGame, SIGNAL(triggered()), this, SLOT(startGame()));

	_about = new QAction(tr("&About AR_Biliard"), this);
	connect(_about, SIGNAL(triggered()), this, SLOT(showAbout()));

	_showTips = new QAction(tr("&Show Game Rules"), this);
	connect(_showTips, SIGNAL(triggered()), this, SLOT(showURL()));

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

void TouchScreen::showURL(){
	ShellExecute(0, 0, L"http://www.billiardino.ch/billiard-regeln.html", 0, 0, SW_SHOW);
}

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

/*
void TouchScreen::calibrate(){
	_scene->changeCalibrateQuestionBool(true);
}
*/

//LÖSCHEN NICHT ZU GEBRAUCHEN
/*
void TouchScreen::toggleFullScreen()
{
	if (isFullScreen()){
		_shrtFullScreen->setText("&Fullscreen");
		_scene->changeCalibrateQuestionBool(true);
		showNormal();
	}
	else{
		_shrtFullScreen->setText("&Normalscreen");
		_scene->changeCalibrateQuestionBool(true);
		showFullScreen();
	}
}
*/

