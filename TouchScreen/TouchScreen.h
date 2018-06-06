#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QGridLayout>

#include "GLScene.h"

#include "ui_TouchScreen.h"

class TouchScreen : public QMainWindow
{
    Q_OBJECT

public:
    TouchScreen(QWidget *parent = 0);
    ~TouchScreen();

private slots:
  void toggleFullScreen();
  void createActions();
  void startGame();

protected:
	void contextMenuEvent(QContextMenuEvent *event);

private:
    Ui::TouchScreenClass ui;

	QMenu *_fileMenu;
	QMenu *_optionMenu;
	QMenu *_aboutMenu;

	QAction *_start;

    QShortcut   *_shrtFullScreen,
                *_shrtQuit;
    QGridLayout *_gridLayout; 
    GLScene     *_scene;
};

#endif // TOUCHSCREEN_H
