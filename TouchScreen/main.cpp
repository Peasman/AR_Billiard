#include "TouchScreen.h"
#include <QtWidgets/QApplication>
#include "Detection.h"
int main(int argc, char *argv[])
{
	Detection d;
	d.runTest();


    QApplication a(argc, argv);
    TouchScreen w;
    w.show();
    return a.exec();
}
