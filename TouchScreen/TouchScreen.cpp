#include "TouchScreen.h"

TouchScreen::TouchScreen(QWidget *parent)
    : QMainWindow(parent)
{
  ui.setupUi(this);

  _shrtFullScreen = new QShortcut( QKeySequence( "Ctrl+F" ), this );
  _shrtQuit       = new QShortcut( QKeySequence( "Ctrl+Q" ), this );
  
  connect( _shrtFullScreen, SIGNAL( activated() ), this, SLOT( toggleFullScreen() ) );
  connect( _shrtQuit,       SIGNAL( activated() ), QApplication::instance(), SLOT( quit() ) );

  _scene = new GLScene( this );

  _gridLayout = new QGridLayout( ui.centralWidget );
  _gridLayout->addWidget( _scene,       0, 0, 1, 1 );

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
