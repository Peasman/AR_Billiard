/********************************************************************************
** Form generated from reading UI file 'TouchScreen.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TOUCHSCREEN_H
#define UI_TOUCHSCREEN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TouchScreenClass
{
public:
    QWidget *centralWidget;

    void setupUi(QMainWindow *TouchScreenClass)
    {
        if (TouchScreenClass->objectName().isEmpty())
            TouchScreenClass->setObjectName(QStringLiteral("TouchScreenClass"));
        TouchScreenClass->resize(1400, 700);
        centralWidget = new QWidget(TouchScreenClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        TouchScreenClass->setCentralWidget(centralWidget);

        retranslateUi(TouchScreenClass);

        QMetaObject::connectSlotsByName(TouchScreenClass);
    } // setupUi

    void retranslateUi(QMainWindow *TouchScreenClass)
    {
        TouchScreenClass->setWindowTitle(QApplication::translate("TouchScreenClass", "TouchScreen", 0));
    } // retranslateUi

};

namespace Ui {
    class TouchScreenClass: public Ui_TouchScreenClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TOUCHSCREEN_H
