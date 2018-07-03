#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QGridLayout>
#include "GLScene.h"
#include "ui_TouchScreen.h"


class GLScene;

class TouchScreen : public QMainWindow
{
    Q_OBJECT

public:
    TouchScreen(QWidget *parent = 0);
    ~TouchScreen();

private slots:
 // void toggleFullScreen();
  void createActions();
  void startGame();
 // void resetGame();
  void enableMouseControll();
  void showAbout();
 // void calibrate();

protected:
	void contextMenuEvent(QContextMenuEvent *event);

private:
	bool _mouseFunction;
	bool _startGame;
	Ui::TouchScreenClass ui;
	QMenu *_fileMenu;
	QMenu *_optionMenu;
	QMenu *_helpMenu;
	QAction *_startNewGame;
//	QAction *_resetGame;
	QAction *_quitGame;
	QAction *_mouseControll;
	QAction *_about;
//	QAction *_shrtFullScreen;
//	QAction *_calibrate;
    QShortcut   *_shrtQuit;
    QGridLayout *_gridLayout; 
    GLScene     *_scene;
};

#endif // TOUCHSCREEN_H
