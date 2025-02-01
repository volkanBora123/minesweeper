#ifndef GAMEHANDLER_H
#define GAMEHANDLER_H
#include "minebutton.h"
#include <vector>
#include <QObject>
class GameHandler : public QObject
{
    Q_OBJECT
public:
    GameHandler(int rows,int cols,int bombs);
    int rows;
    int cols;
    int bombs;
    int opened;
    std::vector<std::vector<MineButton*>> grid;
    std::vector<std::vector<bool>> map;
    bool canPlay;
    bool hintMode;
    int hintCoords;
private:
    void click(MineButton* button);
    int getCount(int y,int x);
    void revealMines();
    void die();
    void win();
    void reduceRowEchelon(std::vector<std::vector<int>> & system);
    int goDown(std::vector<std::vector<int>> & system);
    int goUp(std::vector<std::vector<int>> & system);
public slots:
    void buttonClick();
    void rightClick();
    void initialize_map();
    void print_map();
    void restart();
    void hint(bool checkBombs);
signals:
    void updateScore();
};

#endif // GAMEHANDLER_H
