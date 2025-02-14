#include <QApplication>
#include "MainWindow.h"
#include "AngleRoundedWidget.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    MainWindow mainwindow;
    mainwindow.show();

    AngleRoundedWidget widget;
    widget.show();

    return a.exec();
}

