#include "snake.h"

void Snake::MakeMove()
{
    emit RemoveTail(snake[snake.size() - 1]);

    if (AteApple())
    {
        GenerateApple();
        snake.push_back(qMakePair(-1, -1));
    }

    PullSnake();

    switch (cur_dir)
    {
    case w:
    {
        --snake[0].second;
        break;
    }
    case a:
    {
        --snake[0].first;
        break;
    }
    case s:
    {
        ++snake[0].second;
        break;
    }
    case d:
    {
        ++snake[0].first;
        break;
    }
    }

    if (CheckGameOver())
    {
        timer->stop();
        emit GameOver();
        return;
    }


    emit UpdateBoard();
}

void Snake::KeyPressed(int key)
{
    switch (key)
    {
    case Qt::Key_W:
    {
        cur_dir = w;
        break;
    }
    case Qt::Key_A:
    {
        cur_dir = a;
        break;
    }
    case Qt::Key_S:
    {
        cur_dir = s;
        break;
    }
    case Qt::Key_D:
    {
        cur_dir = d;
        break;
    }
    }
}

bool Snake::CheckGameOver()
{
    if (snake[0].first > field_size - 1 || snake[0].first < 0 || snake[0].second > field_size - 1 || snake[0].second < 0)
    {
        return true;
    }

    if (snake.size() > 1)
    {
        for (int i = 1; i < snake.size(); ++i)
        {
            if (snake[0] == snake[i])
            {
                return true;
            }
        }
    }

    return false;
}

void Snake::GenerateApple()
{
    srand(time(NULL));
    apple = qMakePair(rand() % 10, rand() % 10);

    for (auto& block : snake)
    {
        if (apple == block)
        {
            return GenerateApple();
        }
    }
}

bool Snake::AteApple()
{
    if (snake[0] == apple)
        return true;
    return false;
}

void Snake::PullSnake()
{
    if (snake.size() > 1)
    {
        auto temp = snake[0];
        QPair<int, int> temp2;
        for (int i = 1; i < snake.size(); ++i)
        {
            temp2 = snake[i];
            snake[i] = temp;
            temp = temp2;
        }
    }
}

Snake::Snake(int _field_size, QObject *parent) : QObject(parent), field_size(_field_size)
{
    snake.push_back(qMakePair(0,0));
    timer = new QTimer;
    timer->setInterval(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(MakeMove()));
    GenerateApple();
}

const QVector<QPair<int, int>> Snake::GetSnake()
{
    return snake;
}

const QPair<int, int> Snake::GetApple()
{
    return apple;
}

void Snake::StartTimer()
{
    timer->start();
}
