#include "modelview.h"
#include "ui_modelview.h"

ModelView::ModelView(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ModelView)
{
    ui->setupUi(this);

    QObject::connect(ui->startButton, SIGNAL(released()), this, SLOT(StartGame()));
}

ModelView::~ModelView()
{
    delete ui;
}

void ModelView::StartGame()
{
    QString string_size = ui->lineEdit->text();

    int field_size;

    if (!(field_size = ui->lineEdit->text().toInt()) || field_size < 3 || field_size > 10)
    {
        QMessageBox msg;
        msg.setText("Incorrect size");
        msg.exec();
        return;
    }

    model.reset(new Model(field_size, ui->checkBoxBot->checkState()));

    QObject::connect(model.data(), SIGNAL(UpdateBoard(std::pair<int, int>)), this, SLOT(UpdateBoard(std::pair<int, int>)));
    QObject::connect(model.data(), SIGNAL(EndGame(player)), this, SLOT(EndGame(player)));

    view_board = new ClickableLabel**[field_size];
    for (int i = 0; i < field_size; ++i)
    {
        view_board[i] = new ClickableLabel*[field_size];
    }

    for (int i = 0; i < field_size; ++i)
    {
        QHBoxLayout* hlayout = new QHBoxLayout();

        for (int j = 0; j < field_size; ++j)
        {
            ClickableLabel* label = new ClickableLabel;
            label->setObjectName("position_" + QString::number(i) + '_' + QString::number(j));
            label->setText("position_" + QString::number(i) + '_' + QString::number(j));

            label->coords.first = i;
            label->coords.second = j;

            QObject::connect(label, SIGNAL(clicked(std::pair<int, int>)), model.data(), SLOT(MakeMove(std::pair<int, int>)));

            hlayout->addWidget(label);
            view_board[i][j] = label;
        }

        ui->Field->addLayout(hlayout);
    }
    ui->startButton->setEnabled(false);
    ui->checkBoxBot->setEnabled(false);
}

void ModelView::UpdateBoard(std::pair<int, int> coords)
{
    const auto board = model->GetBoard();

    switch(board[coords.first][coords.second])
    {
        case X:
        {
            QPixmap image("x.png");
            view_board[coords.first][coords.second]->setPixmap(image);
            break;
        }
        case O:
        {
            QPixmap image("0.png");
            view_board[coords.first][coords.second]->setPixmap(image);
            break;
        }
    }
}

void ModelView::EndGame(player winner)
{
    QMessageBox msg;
    switch (winner)
    {
        case X:
        {
            msg.setText("X wins!");
            break;
        }
        case O:
        {
            msg.setText("0 wins!");
            break;
        }
        case DRAW:
        {
            msg.setText("Draw!");
            break;
        }
    }
    QObject::connect(&msg, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(close()));
    msg.exec();
}

