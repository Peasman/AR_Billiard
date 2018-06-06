#include "TouchScreen.h"
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
  _optionMenu->addAction(_mouseControll);

}

void TouchScreen::createActions()
{
	_mouseControll = new QAction(tr("&Enable Mouse"),this);
	_mouseControll->setChecked(false);
	_mouseControll->setCheckable(true);
	connect(_mouseControll, SIGNAL(triggered()), this, SLOT(enableMouseControll()));

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

TouchScreen::~TouchScreen()
{

}


void TouchScreen::toggleFullScreen()
{
  if ( isFullScreen() )
    showNormal();
  else
    showFullScreen();
}

