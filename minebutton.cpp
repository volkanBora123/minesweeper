#include "minebutton.h"
#include <QPushButton>
#include <QMouseEvent>
MineButton::MineButton(int x,int y) : QPushButton() {
    const QSize imageSize = QSize(25,25);
    this->setFixedSize(imageSize);
    this->setImage("empty");
    this->opened = false;
    this->flagged = false;
    this->x = x;
    this->y = y;
}
void MineButton::setImage(QString name){
    QPixmap pixmap(":/assets/" + name + ".png");
    // We think 25x25 buttons look the best. So we images
    QIcon ButtonIcon(pixmap.scaled(25,25,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    this->setIcon(ButtonIcon);
    this->setIconSize(QSize(25,25));
    // Do not add borders. It looks blue when touched and we don't want it.
    this->setStyleSheet("QPushButton {"
                         "border: none;"
                         "padding: 0px;"
                         "}");
}

void MineButton::mousePressEvent(QMouseEvent *e)
{
    // https://stackoverflow.com/questions/15658464/qt-rightclick-qpushbutton
    if(e->button()==Qt::RightButton)
        emit rightClicked();
    else
        emit clicked();
}

void MineButton::reset(){
    this->setImage("empty");
    this->opened = false;
    this->flagged = false;
}
