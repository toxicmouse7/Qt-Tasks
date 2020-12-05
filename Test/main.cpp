#include "mainwindow.h"

#ifdef _WIN32
#include "wclasstest.h"
#endif

#include <QApplication>

int main(int argc, char *argv[])
{
#ifndef WIN32
    ClassTestLin test;
#else
    ClassTestWin test;
#endif
    test.func();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
