#include "scorelabel.h"
#include <QLabel>
ScoreLabel::ScoreLabel() : QLabel() {
    int score;
    setScore(0);
}

void ScoreLabel::setScore(int score){
    this->score = score;
    this->setText("Score : " + QString::number(score));
}

void ScoreLabel::addScore(){
    setScore(score + 1);
}

void ScoreLabel::resetScore(){
    setScore(0);
}
