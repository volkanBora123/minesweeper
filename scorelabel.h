#ifndef SCORELABEL_H
#define SCORELABEL_H
#include <QLabel>
class ScoreLabel : public QLabel
{
    Q_OBJECT
public:
    ScoreLabel();
    int score;
public slots:
    void setScore(int);
    void addScore();
    void resetScore();
};

#endif // SCORELABEL_H
