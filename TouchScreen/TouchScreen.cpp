#include "TouchScreen.h"
#include "windows.h"
#include <iostream>
#include <QMenuBar>

TouchScreen::TouchScreen(QWidget *parent)
    : QMainWindow(parent)
{
  ui.setupUi(this);

  _fileMenu = menuBar()->addMenu(tr("&File"));
  _optionMenu = menuBar()->addMenu(tr("&Option"));
  _aboutMenu = menuBar()->addMenu(tr("&About"));
 

  _scene = new GLScene( this );

  _gridLayout = new QGridLayout( ui.centralWidget );
  _gridLayout->addWidget( _scene, 0, 0, 1, 1 );

  createActions();
  _fileMenu->addAction(_startGame);
  _fileMenu->addAction(_resetGame);
  _fileMenu->addAction(_quitGame);
  _optionMenu->addAction(_mouseControll);
  
}
TouchScreen::~TouchScreen()
{}

void TouchScreen::createActions()
{
	_mouseControll = new QAction(tr("&Enable Mouse"),this);
	_mouseControll->setChecked(false);
	_mouseControll->setCheckable(true);
	connect(_mouseControll, SIGNAL(triggered()), this, SLOT(enableMouseControll()));

	_quitGame = new QAction(tr("&Close Game"), this);
	connect(_quitGame, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));

	_resetGame = new QAction(tr("&Reset Game"), this);
	connect(_resetGame, SIGNAL(triggered()), this, SLOT(resetGame()));

	_startGame = new QAction(tr("&Start Game"), this);
	connect(_startGame, SIGNAL(triggered()), this, SLOT(startGame()));

	_shrtFullScreen = new QShortcut(QKeySequence("Ctrl+F"), this);
	connect(_shrtFullScreen, SIGNAL(activated()), this, SLOT(toggleFullScreen()));

	_shrtQuit = new QShortcut(QKeySequence("Ctrl+Q"), this);
	connect(_shrtQuit, SIGNAL(activated()), QApplication::instance(), SLOT(quit()));

}


//CONTEXTMENU EVENTUELL NICHT BENÖTIGT
void TouchScreen::contextMenuEvent(QContextMenuEvent *event)
{}

void TouchScreen::enableMouseControll()
{
	if (_mouseControll->isChecked())
	{
		_mouseControll->setText(tr("&Dissable Mouse"));
		_mouseControll->setChecked(true);

		//funcion für die GLSCENE
		_mouseFunction = true;

	}
	if (!_mouseControll->isChecked()){
		_mouseControll->setText(tr("&Enable Mouse"));
		_mouseControll->setChecked(false);

		//function für die GLSCENE
		_mouseFunction = false;
	}
}

void TouchScreen::startGame()
{
	//BOOL von GLScene ändern in True, weil das Spiel gestartet wird.
	
}

void TouchScreen::resetGame()
{
	const int result = MessageBox(nullptr, TEXT("Reset Game?"), TEXT("Message"), MB_YESNO);
	switch (result)
	{
		case IDYES:
			// RESET GAME WIRD AUSGEFÜHRT
			break;
	}
	
}

void TouchScreen::toggleFullScreen()
{
  if ( isFullScreen() )
    showNormal();
  else
    showFullScreen();
}

