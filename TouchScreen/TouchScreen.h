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

private:
    Ui::TouchScreenClass ui;

    QShortcut   *_shrtFullScreen,
                *_shrtQuit;
    QGridLayout *_gridLayout; 
    GLScene     *_scene;
};

#endif // TOUCHSCREEN_H
