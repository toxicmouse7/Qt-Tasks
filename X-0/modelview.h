#ifndef MODELVIEW_H
#define MODELVIEW_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QPixmap>
#include <QCheckBox>

#include "Model.h"
#include "clickablelabel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ModelView; }
QT_END_NAMESPACE

class ModelView : public QMainWindow
{
    Q_OBJECT
private:
    Ui::ModelView *ui;
    QScopedPointer<Model> model;
    ClickableLabel*** view_board = nullptr;

public:
    ModelView(QWidget *parent = nullptr);
    ~ModelView();

private slots:
    void StartGame();
    void UpdateBoard(std::pair<int, int> coords);
    void EndGame(player winner);


};
#endif // MODELVIEW_H
