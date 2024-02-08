/* Your code here! */
#include "maze.h"
#include "dsets.h"
#include <vector>
#include <queue>
#include <utility>
#include "cs225/PNG.h"
#include "cs225/HSLAPixel.h"

SquareMaze::SquareMaze(){
    width_ = 0;
    height_ = 0;
    dsets_ = new DisjointSets();
}

void SquareMaze::makeMaze(int width, int height){
    width_ = width;
    height_ = height;

    dsets_ -> addelements(width_ * height_);

    maze_.clear();

    for(int i = 0; i < (width_ * height_); i++){
        maze_.push_back(std::make_pair(true, true));
    }

    for(int i = 0; i < (width_ * height_) - 1; i++){
        std::vector<int> randBreak;
        //int x = i % width_;
        //int y = i % height_;

        //right, cannot be rightmost to destroy right wall
        if((i + 1) % width_ != 0 && !isCycle(i, i + 1)){
            randBreak.push_back(0);
        }
        //down, cannot be on bottom row
        if( i + width_ < width_ * height_ && !isCycle(i, i + width_)){
            randBreak.push_back(1);
        }
        //left, cannot be leftmost, no wall to break
        if( i % width_ != 0 && !isCycle(i, i - 1)){
            randBreak.push_back(2);
        }
        //up
        if( i - width_ >= 0 && !isCycle(i, i - width_)){
            randBreak.push_back(3);
        }

        if(randBreak.empty()){
            continue;
        }
        else{
            int cases = randBreak[std::rand() % randBreak.size()];

            //no x and y calc so no setwall needed
            switch(cases){
                case 0:
                    dsets_ -> setunion(i, i + 1);
                    maze_[i].second = false;
                    break;
                case 1:
                    dsets_ -> setunion(i, i + width_);
                    maze_[i].first = false;
                    break;
                case 2:
                    dsets_ -> setunion(i, i - 1);
                    maze_[i - 1].second = false;
                    break;
                case 3:
                    dsets_ -> setunion(i, i - width_);
                    maze_[i - width_].first = false;
                    break;
            }
        }
    }
}

void SquareMaze::setWall(int x, int y, int dir, bool exists){
    if(dir){
        maze_[y * width_ + x].first = exists;
    }
    else{
        maze_[y * width_ + x].second = exists;
    }
}

bool SquareMaze::isCycle(int pos1, int pos2){
    //if in same set, contains cycle
    return (dsets_ -> find(pos1) == dsets_ -> find(pos2));
}

bool SquareMaze::canTravel(int x, int y, int dir) const{
    int loc = y * width_ + x;

    switch(dir){
        case 0:
            if(x + 1 >= width_){
                return false;
            }
            return !maze_[loc].second;
        case 1:
            if(y + 1 >= height_){
                return false;
            }
            return !maze_[loc].first;
        case 2:
            if(x - 1 < 0){
                return false;
            }
            return !maze_[loc - 1].second;
        case 3:
            if(y - 1 < 0){
                return false;
            }
            return !maze_[loc - width_].first;
        default:
            return false;
    }
}

std::vector<int> SquareMaze::solveMaze(){
    int size_ = width_ * height_;
    std::vector<std::pair<int, int>> pathCalc;
    std::vector<bool> hasSeen;

    std::queue<int> bfsQueue;
    bfsQueue.push(0);

    for(int i = 0; i < size_; i++){
        pathCalc.push_back(std::make_pair(i, 0));
        hasSeen.push_back(false);
    }

    std::pair<int, int> longestPath(-1, -1);
    while(!bfsQueue.empty()){
        int curPos = bfsQueue.front();
        int curX = curPos % width_;
        int curY = curPos / width_;
        hasSeen[curPos] = true;
        bfsQueue.pop();
        
        if(canTravel(curX, curY, 0) && !hasSeen[curPos + 1]){
            bfsQueue.push(curPos + 1);
            pathCalc[curPos + 1].first = curPos;
            pathCalc[curPos + 1].second = pathCalc[curPos].second + 1;
        }

        if(canTravel(curX, curY, 1) && !hasSeen[curPos + width_]){
            bfsQueue.push(curPos + width_);
            pathCalc[curPos + width_].first = curPos;
            pathCalc[curPos + width_].second = pathCalc[curPos].second + 1;
        }

        if(canTravel(curX, curY, 2) && !hasSeen[curPos - 1]){
            bfsQueue.push(curPos - 1);
            pathCalc[curPos - 1].first = curPos;
            pathCalc[curPos - 1].second = pathCalc[curPos].second + 1;
        }
    
        if(canTravel(curX, curY, 3) && !hasSeen[curPos - width_]){
            bfsQueue.push(curPos - width_);
            pathCalc[curPos - width_].first = curPos;
            pathCalc[curPos - width_].second = pathCalc[curPos].second + 1;
        }
    }

    for(int i = size_ - width_; i < size_; i++){
        if(pathCalc[i].second > longestPath.second){
            longestPath.first = i;
            longestPath.second = pathCalc[i].second;
        }
    }
    //std::cout << longestPath.first << " " << longestPath.second;

    std::vector<int> ret(longestPath.second);
    int backTracker = longestPath.first;
    int distance_ = longestPath.second;
    while(backTracker != 0){
        distance_--;
        if(pathCalc[backTracker].first == backTracker + 1){
            ret[distance_] = 2;
        }
        else if(pathCalc[backTracker].first == backTracker + width_){
            ret[distance_] = 3;
        }
        else if(pathCalc[backTracker].first == backTracker - 1){
            ret[distance_] = 0;
        }
        else if(pathCalc[backTracker].first == backTracker - width_){
            ret[distance_] = 1;
        }
        backTracker = pathCalc[backTracker].first;
    }
    return ret;
}

cs225::PNG* SquareMaze::drawMaze() const{
    int xMax = width_ * 10 + 1;
    int yMax = height_ * 10 + 1;
    cs225::PNG *mazePicture = new cs225::PNG(xMax, yMax);

    for(int x = 10; x < xMax; x++){
        cs225::HSLAPixel &p_ = mazePicture -> getPixel(x, 0);
        p_.l = 0;
    }

    for(int y = 0; y < yMax; y++){
        cs225::HSLAPixel &p_ = mazePicture -> getPixel(0, y);
        p_.l = 0;
    }

    for(int x = 0; x < width_; x++){
        for(int y = 0; y < height_; y++){
            if(maze_[x + y * width_].second){
                for(int k = 0; k <= 10; k++){
                    cs225::HSLAPixel &p_ = mazePicture -> getPixel((x + 1) * 10, y * 10 + k);
                    p_.l = 0;
                }
            }

            if(maze_[x + y * width_].first){
                for(int k = 0; k <= 10; k++){
                    cs225::HSLAPixel &p_ = mazePicture -> getPixel(x * 10 + k, (y + 1) * 10);
                    p_.l = 0;
                }
            }
        }
    }

    return mazePicture;
}

cs225::PNG* SquareMaze::drawMazeWithSolution(){
    cs225::PNG *mazeSolution = drawMaze();
    std::vector<int> solutionPath = solveMaze();

    int curX = 5;
    int curY = 5;
    for(auto i : solutionPath){
        if(i == 0){
            for(int k = 0; k <= 10; k++){
                cs225::HSLAPixel &p_ = mazeSolution -> getPixel(curX, curY);
                p_.s = 1;
                p_.l = 0.5;
                curX++;
            }
            curX--;
        }
        else if(i == 1){
            for(int k = 0; k <= 10; k++){
                cs225::HSLAPixel &p_ = mazeSolution -> getPixel(curX, curY);
                p_.s = 1;
                p_.l = 0.5;
                curY++;
            }
            curY--;
        }
        else if(i == 2){
            for(int k = 0; k <= 10; k++){
                cs225::HSLAPixel &p_ = mazeSolution -> getPixel(curX, curY);
                p_.s = 1;
                p_.l = 0.5;
                curX--;
            }
            curX++;
        }
        else if(i == 3){
            for(int k = 0; k <= 10; k++){
                cs225::HSLAPixel &p_ = mazeSolution -> getPixel(curX, curY);
                p_.s = 1;
                p_.l = 0.5;
                curY--;
            }
            curY++;
        }
    }
    curX -= 5;
    
    for(int k = 1; k <= 9; k++){
        cs225::HSLAPixel &p_ = mazeSolution -> getPixel(curX + k, height_ * 10);
        p_.l = 1;
    }
    return mazeSolution;
}