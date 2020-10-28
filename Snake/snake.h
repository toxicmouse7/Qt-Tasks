#ifndef SNAKE_H
#define SNAKE_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <random>

enum direction
{
    w,a,s,d
};

class Snake : public QObject
{
    Q_OBJECT
private:
    QVector<QPair<int, int>> snake;
    QPair<int, int> apple;
    direction cur_dir = d;
    int field_size;
    QTimer* timer;

    bool CheckGameOver();
    void GenerateApple();
    bool AteApple();
    void PullSnake();

public:
    explicit Snake(int _field_size, QObject *parent = nullptr);
    const QVector<QPair<int, int>> GetSnake();
    const QPair<int, int> GetApple();
    void StartTimer();

signals:
    void RemoveTail(QPair<int, int>);
    void RemoveApple(QPair<int, int>);
    void UpdateBoard();
    void GameOver();

private slots:
    void MakeMove();
    void KeyPressed(int key);

};

#endif // SNAKE_H
