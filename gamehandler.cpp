#include "gamehandler.h"
#include "minebutton.h"
#include <vector>
#include <QObject>
#include <iostream>
#include <QMessageBox>
#include <time.h>
#include <unordered_set>
#include <string>
std::vector<std::pair<int,int>> directions = {{-1,-1},
                                               {-1,0},
                                               {-1,1},
                                               {1,-1},
                                               {1,0},
                                               {1,1},
                                               {0,1},
                                               {0,-1}};

GameHandler::GameHandler(int rows,int cols,int bombs) {
    this->opened = 0;
    this->rows = rows;
    this->cols = cols;
    this->bombs = bombs;
    this->grid = std::vector<std::vector<MineButton*>>();
    this->map = std::vector<std::vector<bool>>(rows,std::vector<bool>(cols,false));
    this->canPlay = true;
    this->hintMode = false;
    this->hintCoords = -1;
    // For randomization
    srand(time(NULL));
}

void GameHandler::buttonClick(){
    if(!canPlay)
        return;
    MineButton *caller = qobject_cast<MineButton *>(sender());
    click(caller);
}

void GameHandler::click(MineButton* caller){
    if(caller->opened)
        return;
    int x = caller->x;
    int y = caller->y;
    if(map[y][x]){
    // Stepped on a mine
        die();
        return;
    }
    // Score update
    opened++;
    emit updateScore();
    int bombCount = getCount(y,x);
    caller->opened = true;
    caller->setImage(QString::number(bombCount));
    if(hintMode && y * cols + x == hintCoords){
    // If this was hinted, reset hint.
        hintCoords = -1;
        hintMode = false;
    }
    if(opened >= rows * cols - bombs){
        win();
        return;
    }
    if(bombCount == 0){
    // If there are not any mines, do DFS.
        for(auto pair : directions){
            int dy = pair.first;
            int dx = pair.second;
            if(0 <= y + dy && y + dy < rows && 0 <= x + dx && x + dx < cols){
                if(!grid[y + dy][x + dx]->opened && !map[y + dy][x + dx] && getCount(y,x) == 0){
                    click(grid[y + dy][x + dx]);
                }
            }
        }
    }
}

void GameHandler::rightClick(){
    if(!canPlay)
        return;
    MineButton *caller = qobject_cast<MineButton *>(sender());
    if(caller->opened)
        return;
    if(caller->flagged){
        caller->setImage("empty");
        caller->flagged = false;
    }else{
        caller->setImage("flag");
        caller->flagged = true;
        if(hintMode && caller->x == hintCoords % cols && caller->y == hintCoords / cols){
            hintMode = false;
            hintCoords = -1;
        }
    }
}

int GameHandler::getCount(int y,int x){
    // Returns how many mines are around the coordinate.
    int bombCount = 0;
    for(auto pair : directions){
        int dy = pair.first;
        int dx = pair.second;
        if(0 <= y + dy && y + dy < rows && 0 <= x + dx && x + dx < cols){
            if(map[y + dy][x + dx])bombCount++;
        }
    }
    return bombCount;
}

void GameHandler::initialize_map(){
    int bombCount = 0;
    while(bombCount < bombs){
        int x = rand() % cols;
        int y = rand() % rows;
        std::cout << y << " " << x << std::endl;
        if(!map[y][x]){
            map[y][x] = true;
            bombCount++;
        }
    }
    print_map();
}

void GameHandler::print_map(){
    for(int i = 0;i < rows;i++){
        for(int j = 0;j < cols;j++){
            std::cout << map[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void GameHandler::restart(){
    this->opened = 0;
    for(int i = 0;i < rows;i++){
        for(int j = 0;j < cols;j++){
            this->map[i][j] = false;
        }
    }
    initialize_map();
    for(int i = 0;i < rows;i++){
        for(int j = 0;j < cols;j++){
            grid[i][j]->reset();
        }
    }
    canPlay = true;
}

void GameHandler::revealMines(){
    for(int i = 0;i < rows;i++){
        for(int j = 0;j < cols;j++){
            if(map[i][j])grid[i][j]->setImage("mine");
            else if(grid[i][j]->flagged)grid[i][j]->setImage("wrong-flag");
        }
    }
}

void GameHandler::die(){
    canPlay = false;
    revealMines();
    QMessageBox msgBox;
    msgBox.setText("You lose!");
    msgBox.setWindowTitle("minesweeper");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void GameHandler::win(){
    canPlay = false;
    revealMines();
    QMessageBox msgBox;
    msgBox.setText("You win!");
    msgBox.setWindowTitle("minesweeper");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

static bool compareByPivotPosition(const std::vector<int> & first,const std::vector<int> & second){
    // Used to sort matrix elements. Tries to create a upper triangular matrix.
    for(int i = 0;i < first.size() - 1;i++){
        if(first[i] != 0 && second[i] == 0)
            return true;
        else if(second[i] != 0 && first[i] == 0)
            return false;
    }
    return false;
}

int GameHandler::goDown(std::vector<std::vector<int>> & linearSystem){
    // Gaussian elimination from top to bottom
    // Returns an integer indicating next action
    // 0 -> no change, do not need to reduce it anymore
    // 1 -> some change, do row reduction again
    // 2 -> already found a empty cell, return it
    std::unordered_set<int> removeIndexes;
    std::sort(linearSystem.begin(),linearSystem.end(),compareByPivotPosition);
    // Matrix reduction
    for(int i = 0;i + 1< linearSystem.size();i++){
        int firstIndex = -1;
        // Find first nonzero element
        for(int x = 0;x < linearSystem[i].size() - 1;x++){
            if(linearSystem[i][x] != 0){
                firstIndex = x;
                break;
            }
        }
        if(firstIndex == -1){
        // If first index is -1, it is an empty equation, remove it.
            removeIndexes.insert(i);
            continue;
        }
        // Try not to do unneccessary gaussian elimination
        std::vector<int> nonzeroIndexes;
        for(int index = firstIndex;index < linearSystem[i].size() - 1;index ++){
            if(linearSystem[i][index] != 0)nonzeroIndexes.push_back(index);
        }
        for(int j = i + 1;j < linearSystem.size();j++){
            bool allImportant = true;
            // Check if this substraction is needed
            // Example : 1 + 2 + 3 + 4 = 3
            //           1 + 2         = 2
            // Do not subtract upper row from lower row because we are going to lose information.
            for(int index : nonzeroIndexes){
                if(linearSystem[j][index] == 0){
                    allImportant = false;
                    break;
                }
            }
            if(!allImportant)
                continue;
            // Subtract this row from other row.
            if(linearSystem[j][firstIndex] == linearSystem[i][firstIndex]){
                for(int x = 0;x < linearSystem[i].size();x++){
                    linearSystem[j][x] -= linearSystem[i][x];
                }
            }
            else if(linearSystem[j][firstIndex] == -linearSystem[i][firstIndex]){
                for(int x = 0;x < linearSystem[i].size();x++){
                    linearSystem[j][x] += linearSystem[i][x];
                }
            }
        }
    }
    // Equation simplifier
    bool changed = false;
    for(int index = 0; index < linearSystem.size();index++){
        std::vector<int> equation = linearSystem[index];
        int positiveCount = 0;
        int negativeCount = 0;
        for(int i = 0;i < equation.size() - 1;i++){
            if(equation[i] > 0)positiveCount++;
            if(equation[i] < 0)negativeCount++;
        }
        // a + b = 2 -> a = 1 b = 1
        if((positiveCount > 1 && negativeCount == 0 && equation.back() == positiveCount) ||
            (negativeCount > 1 && positiveCount == 0&& equation.back() == -negativeCount)){
            changed = true;
            // Remove this row, because we are replacing it with multiple equations
            removeIndexes.insert(index);
            for(int i = 0;i < equation.size() - 1;i++){
                std::vector<int> newEq = std::vector<int>(this->rows*cols + 1,0);
                if(equation[i] != 0){
                // These cells are guaranteed to be mines.
                    newEq[i] = 1;
                    newEq[rows*cols] = 1;
                    linearSystem.push_back(newEq);
                }
            }
        }
        // a + b = 0 -> a = 0 b = 0
        else if((positiveCount > 1 && negativeCount == 0 && equation.back() == 0) ||
            (negativeCount > 1 && positiveCount == 0 && equation.back() == 0)){
            changed = true;
            // Remove this row, because we are replacing it with multiple equations
            removeIndexes.insert(index);
            for(int i = 0;i < equation.size() - 1;i++){
                std::vector<int> newEq = std::vector<int>(this->rows*cols + 1,0);
                if(equation[i] != 0){
                // These cells are guaranteed not to be mines, return early to save computation.
                    newEq[i] = 1;
                    newEq[rows*cols] = 0;
                    linearSystem.push_back(newEq);
                    return 2;
                }
            }
        }
    }
    // Remove zero rows.
    std::vector<std::vector<int>> res = std::vector<std::vector<int>>(0);
    for(int i = 0;i < linearSystem.size();i++){
        if(removeIndexes.find(i) == removeIndexes.end()){
            res.push_back(linearSystem[i]);
        }
    }
    linearSystem = res;
    return changed;
}

int GameHandler::goUp(std::vector<std::vector<int> > & linearSystem){
    // Same gaussian elimination from bottom to top.
    // Returns an integer indicating next action
    // 0 -> no change, do not need to reduce it anymore
    // 1 -> some change, do row reduction again
    // 2 -> already found a empty cell, return it
    std::unordered_set<int> removeIndexes;
    std::sort(linearSystem.begin(),linearSystem.end(),compareByPivotPosition);
    for(int i = linearSystem.size() - 1;i - 1 >= 0;i--){
        int firstIndex = -1;
        // Find first nonzero element
        for(int x = 0;x < linearSystem[i].size() - 1;x++){
            if(linearSystem[i][x] != 0){
                firstIndex = x;
                break;
            }
        }
        if(firstIndex == -1){
            removeIndexes.insert(i);
            continue;
        }
        // Try to not do unneccessary gaussian elimination
        std::vector<int> nonzeroIndexes;
        for(int index = firstIndex;index < linearSystem[i].size() - 1;index ++){
            if(linearSystem[i][index] != 0)nonzeroIndexes.push_back(index);
        }
        for(int j = i - 1;j >= 0;j--){
            bool allImportant = true;
            // Check if this substraction is needed
            // Example : 1 + 2         = 3
            //           1 + 2 + 3 + 4 = 3
            // Do not subtract lower row from upper row because we are going to lose information.
            for(int index : nonzeroIndexes){
                if(linearSystem[j][index] == 0){
                    allImportant = false;
                    break;
                }
            }
            if(!allImportant)
                continue;
            if(linearSystem[j][firstIndex] == linearSystem[i][firstIndex]){
                for(int x = 0;x < linearSystem[i].size();x++){
                    linearSystem[j][x] -= linearSystem[i][x];
                }
            }
            else if(linearSystem[j][firstIndex] == -linearSystem[i][firstIndex]){
                for(int x = 0;x < linearSystem[i].size();x++){
                    linearSystem[j][x] += linearSystem[i][x];
                }
            }
        }
    }
    bool changed = false;
    // Simplifier
    for(int index = 0; index < linearSystem.size();index++){
        std::vector<int> equation = linearSystem[index];
        int positiveCount = 0;
        int negativeCount = 0;
        for(int i = 0;i < equation.size() - 1;i++){
            if(equation[i] > 0)positiveCount++;
            if(equation[i] < 0)negativeCount++;
        }
        // a + b = 2 -> a = 1 b = 1
        if(positiveCount > 1 && negativeCount == 0 && equation.back() == positiveCount ||
            negativeCount > 1 && positiveCount == 0 && equation.back() == -negativeCount){
            changed = true;
            // Remove this row, because we are replacing it with multiple equations
            removeIndexes.insert(index);
            for(int i = 0;i < equation.size() - 1;i++){
                std::vector<int> newEq = std::vector<int>(this->rows*cols + 1,0);
                if(equation[i] != 0){
                // These cells are guaranteed to be mines.
                    newEq[i] = 1;
                    newEq[rows*cols] = 1;
                    linearSystem.push_back(newEq);
                    return 2;
                }
            }
        }
        // a = 0
        else if((positiveCount > 1 && negativeCount == 0 && equation.back() == 0) ||
            (negativeCount > 1 && positiveCount == 0 && equation.back() == 0)){
            changed = true;
            // Remove this row, because we are replacing it with multiple equations
            removeIndexes.insert(index);
            for(int i = 0;i < equation.size() - 1;i++){
                std::vector<int> newEq = std::vector<int>(this->rows*cols + 1,0);
                if(equation[i] != 0){
                // These cells are guaranteed not to be mines, return early to save computation.
                    newEq[i] = 1;
                    newEq[rows*cols] = 0;
                    linearSystem.push_back(newEq);
                }
            }
        }
    }
    std::vector<std::vector<int>> res = std::vector<std::vector<int>>(0);
    for(int i = 0;i < linearSystem.size();i++){
        if(removeIndexes.find(i) == removeIndexes.end()){
            res.push_back(linearSystem[i]);
        }
    }
    linearSystem = res;
    return changed;
}

void GameHandler::reduceRowEchelon(std::vector<std::vector<int>> & linearSystem){
    // Row reduction to empty cells
    bool changed = false;
    int downCode = goDown(linearSystem);
    if(downCode == 2)
    // Found an empty cell
        return;
    changed |= downCode;
    int upCode = goUp(linearSystem);
    if(upCode == 2)
        return;
    changed |= upCode;
    if(changed){
    // If there is a change, do row reduction again.
        reduceRowEchelon(linearSystem);
    }
}

void GameHandler::hint(bool checkBombs = false){
    if(!canPlay)
        return;
    if(hintMode && hintCoords != -1){
    // Already found an empty cell, just click it
        click(grid[hintCoords / cols][hintCoords % cols]);
        hintMode = false;
        return;
    }
    std::vector<std::vector<int>> hintMap = std::vector<std::vector<int>>(rows,std::vector<int>(cols,-1));
    std::vector<std::vector<int>> linearSystem = std::vector<std::vector<int>>();
    std::vector<int> bombEquation = std::vector<int>(rows*cols + 1,0);
    for(int i = 0;i < rows;i++){
        for(int j = 0;j < cols;j++){
            if(grid[i][j]->opened){
                int unknown = 0;
                // Look for unopened blocks
                for(auto pair : directions){
                    int dy = pair.first;
                    int dx = pair.second;
                    if(0 <= i + dy && i + dy < rows && 0 <= j + dx && j + dx < cols){
                        if(!grid[i + dy][j + dx]->opened)unknown++;
                        else hintMap[i + dy][j + dx] = getCount(i + dy,j + dx);
                    }
                }
                if(unknown > 0){
                // Add all unopened block coordinates to equation
                    std::vector<int> coords = std::vector<int>(rows*cols + 1,0);
                    for(auto pair : directions){
                        int dy = pair.first;
                        int dx = pair.second;
                        if(0 <= i + dy && i + dy < rows && 0 <= j + dx && j + dx < cols){
                            if(!grid[i + dy][j + dx]->opened){
                                coords[(i + dy) * cols + j + dx] = 1;
                            }
                        }
                    }
                    // Last element of the vector is the solution to the equation
                    coords[rows * cols] = getCount(i,j);
                    linearSystem.push_back(coords);
                }
            }
            else{
            // Add all unopened cells to bomb equation
                bombEquation[i * cols + j] = 1;
            }
        }
    }
    // Unopened cells must be equal to remaining bomb count
    bombEquation[rows*cols] = this->bombs;
    linearSystem.push_back(bombEquation);
    this->reduceRowEchelon(linearSystem);
    // Find a equation with full positives or full negatives with an answer 0.
    for(auto vector : linearSystem){
        if(vector[rows * cols] == 0){
            int firstIndex = -1;
            int positiveCount = 0;
            int negativeCount = 0;
            for(int i = 0;i < rows*cols;i++){
                if(vector[i] != 0){
                    if(firstIndex == -1)firstIndex = i;
                    if(vector[i] > 0)positiveCount++;
                    else negativeCount++;
                }
            }
            if(firstIndex != -1 && (positiveCount == 0 || negativeCount == 0)){
                std::cout << "hint : " << firstIndex / cols << " " << firstIndex % cols << std::endl;
                hintMode = true;
                hintCoords = firstIndex;
                grid[firstIndex / cols][firstIndex % cols]->setImage("hint");
                grid[firstIndex / cols][firstIndex % cols]->flagged = false;
                return;
            }
        }
    }
}
