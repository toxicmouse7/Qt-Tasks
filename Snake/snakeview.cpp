#include "snakeview.h"
#include "ui_snakeview.h"

void SnakeView::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();

    emit KeyPressed(key);
}

void SnakeView::GameOver()
{
    QMessageBox msg;
    msg.setText("Game Over!");
    msg.exec();
}

SnakeView::SnakeView(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SnakeView)
{
    ui->setupUi(this);

    field_size = 10;
    model.reset(new Snake(field_size));
    QPixmap backg(50,50);
    backg.fill();

    visible_field = new QLabel**[field_size];
    for (int i = 0; i < field_size; ++i)
    {
        visible_field[i] = new QLabel*[field_size];
    }

    for (int i = 0; i < field_size; ++i)
    {
        QHBoxLayout* hlayout = new QHBoxLayout();
        hlayout->setSpacing(0);

        for (int j = 0; j < field_size; ++j)
        {
            QVBoxLayout* vlayout = new QVBoxLayout();
            vlayout->setSpacing(0);
            QLabel* label = new QLabel;
            label->setPixmap(backg);
            visible_field[i][j] = label;
            vlayout->addWidget(label);
            hlayout->addLayout(vlayout);
        }

        ui->Field->addLayout(hlayout);
    }

    connect(model.data(), SIGNAL(RemoveTail(QPair<int, int>)), this, SLOT(RemoveObject(QPair<int, int>)));
    connect(model.data(), SIGNAL(RemoveApple(QPair<int, int>)), this, SLOT(RemoveApple(QPair<int, int>)));
    connect(model.data(), SIGNAL(UpdateBoard()), this, SLOT(UpdateBoard()));
    connect(model.data(), SIGNAL(GameOver()), this, SLOT(GameOver()));
    connect(this, SIGNAL(KeyPressed(int)), model.data(), SLOT(KeyPressed(int)));


    UpdateBoard();
    model->StartTimer();
}

SnakeView::~SnakeView()
{
    delete ui;
}

void SnakeView::UpdateBoard()
{
    QPixmap body_part(50, 50), _apple(50, 50);
    body_part.fill(Qt::red);
    _apple.fill(Qt::yellow);

    auto snake = model->GetSnake();

    auto apple = model->GetApple();

    visible_field[apple.second][apple.first]->setPixmap(_apple);

    for (auto& block : snake)
    {
        visible_field[block.second][block.first]->setPixmap(body_part);
    }
}

void SnakeView::RemoveObject(QPair<int, int> tail)
{
    QPixmap backgr(50,50);
    backgr.fill();

    visible_field[tail.second][tail.first]->setPixmap(backgr);
}

void SnakeView::RemoveApple(QPair<int, int> apple)
{
    QPixmap backgr(50,50);
    backgr.fill(Qt::red);

    visible_field[apple.second][apple.first]->setPixmap(backgr);
}



