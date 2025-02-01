#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGridLayout>
#include "scorelabel.h"
#include "minebutton.h"
#include "gamehandler.h"

int main(int argc, char* argv[]){
    QApplication app(argc,argv);
    // Main Window
    QMainWindow *mw = new QMainWindow;
    // Central Widget
    QWidget *cw = new QWidget;
    QWidget *gridHolder = new QWidget;
    QGridLayout *gridLayout = new QGridLayout(gridHolder);
    QVBoxLayout *verticalLayout = new QVBoxLayout(cw);
    // Score, Reset and Hint buttons
    QWidget *horizontalHolder = new QWidget;
    QHBoxLayout *hl = new QHBoxLayout(horizontalHolder);
    ScoreLabel *scoreLabel = new ScoreLabel;
    QPushButton *resetButton = new QPushButton("Restart");
    QPushButton *hintButton = new QPushButton("Hint");
    verticalLayout->addWidget(horizontalHolder);
    hl->addWidget(scoreLabel);
    hl->addWidget(resetButton);
    hl->addWidget(hintButton);
    // Mines
    int M = 10; // Columns
    int N = 10; // Rows
    int K = 10; // Bombs
    // Main game controller
    GameHandler *gameHandler = new GameHandler(N,M,K);
    gameHandler->initialize_map();
    gameHandler->print_map();
    // Necessary connections
    QObject::connect(resetButton,SIGNAL(clicked(bool)),gameHandler,SLOT(restart()));
    QObject::connect(gameHandler,SIGNAL(updateScore()),scoreLabel,SLOT(addScore()));
    QObject::connect(resetButton,SIGNAL(clicked(bool)),scoreLabel,SLOT(resetScore()));
    QObject::connect(hintButton,SIGNAL(clicked(bool)),gameHandler,SLOT(hint(bool)));
    // Minebuttons and grid layout
    std::vector<std::vector<MineButton*>> grid;
    for(int i = 0;i < N;i++){
        std::vector<MineButton*> vectorRow;
        for(int j = 0;j < M;j++){
            MineButton *mineButton = new MineButton(j,i);
            vectorRow.push_back(mineButton);
            QObject::connect(mineButton,SIGNAL(clicked()),gameHandler,SLOT(buttonClick()),Qt::UniqueConnection);
            QObject::connect(mineButton,SIGNAL(rightClicked()),gameHandler,SLOT(rightClick()),Qt::UniqueConnection);
            gridLayout->addWidget(mineButton,i,j);
        }
        grid.push_back(vectorRow);
    }
    gameHandler->grid = grid;
    // Visual settings
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    verticalLayout->addWidget(gridHolder);
    mw->setCentralWidget(cw);
    mw->setWindowTitle("Minesweeper");
    mw->show();
    return app.exec();
}
