#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    std::pair<int,int> coords;
   explicit ClickableLabel( const QString& text="", QWidget* parent = nullptr);
   ~ClickableLabel();

signals:

   void clicked(std::pair<int, int>);

protected:

   void mousePressEvent(QMouseEvent* event);
};
#endif // CLICKABLELABEL_H
