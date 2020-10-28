#include "snakeview.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SnakeView w;
    w.show();
    return a.exec();
}
