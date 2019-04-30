//
// Created by Gerardin, Antoine D. (Assoc) on 2019-04-30.
//

#include <iostream>
#include <cmath>

int maxRow = 4;
int maxCol = 7;

bool isValid(int row,int col){
    return(row < maxRow && col < maxCol);
}

void blockTraversal(int row, int col, int width) {

    //deal with matrices not square.
    if(!isValid(row,col)){
        return;
    }

    if(width == 1){
        std::cout << row << "," << col << std::endl;
        return;
    }

    //deal with odd dimensions.
    int newWidth = (int)ceil((double)width / 2);

    if(newWidth !=1 && newWidth % 2 != 0){
        ++newWidth;
    }

    std::cout << "start: " << row << "," << col << std::endl;

    blockTraversal(row, col, newWidth);
    blockTraversal(row, col + newWidth, newWidth);
    blockTraversal(row + newWidth, col, newWidth);
    blockTraversal(row + newWidth, col + newWidth, newWidth);


}

int main(){
    auto maxDim = std::max(maxRow, maxCol);
    blockTraversal(0, 0,maxDim);


}

