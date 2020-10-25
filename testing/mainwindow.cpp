#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void MainWindow::AddText(const QString& text)
{
    QLabel* label = new QLabel(ui->verticalLayoutWidget);

    label->setText(text);
}

MainWindow::~MainWindow()
{
    delete ui;
}

