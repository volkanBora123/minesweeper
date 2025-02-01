#ifndef MINEBUTTON_H
#define MINEBUTTON_H
#include <QPushButton>
class MineButton : public QPushButton
{
    Q_OBJECT
public:
    MineButton(int x, int y);
    void setImage(QString name);
    bool opened;
    bool flagged;
    int x;
    int y;
    void reset();
signals:
    void rightClicked();
private slots:
    void mousePressEvent(QMouseEvent *e);
};

#endif // MINEBUTTON_H
