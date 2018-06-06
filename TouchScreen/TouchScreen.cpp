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
  _gridLayout->addWidget( _scene,       0, 0, 1, 1 );

  createActions();
  _fileMenu->addAction(_start);

}

void TouchScreen::createActions()
{
	_start = new QAction(tr("&Start Game"), this);
	connect(_start, SIGNAL(pressed()), this, SLOT(startGame()));

	_shrtFullScreen = new QShortcut(QKeySequence("Ctrl+F"), this);
	connect(_shrtFullScreen, SIGNAL(activated()), this, SLOT(toggleFullScreen()));

	_shrtQuit = new QShortcut(QKeySequence("Ctrl+Q"), this);
	connect(_shrtQuit, SIGNAL(activated()), QApplication::instance(), SLOT(quit()));

}

void TouchScreen::contextMenuEvent(QContextMenuEvent *event)
{
	
}

void TouchScreen::startGame()
{

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

