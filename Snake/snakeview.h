#ifndef SNAKEVIEW_H
#define SNAKEVIEW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QMessageBox>

#include "snake.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SnakeView; }
QT_END_NAMESPACE

class SnakeView : public QMainWindow
{
    Q_OBJECT

public:
    SnakeView(QWidget *parent = nullptr);
    ~SnakeView();

private:
    Ui::SnakeView *ui;
    QScopedPointer<Snake> model;
    QLabel*** visible_field;
    int field_size;

    virtual void keyPressEvent(QKeyEvent* event);

public slots:
    void UpdateBoard();
    void RemoveObject(QPair<int, int> tail);
    void RemoveApple(QPair<int, int> apple);
    void GameOver();

signals:
    void KeyPressed(int);

};
#endif // SNAKEVIEW_H
