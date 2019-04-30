//
// Created by Gerardin, Antoine D. (Assoc) on 2019-04-30.
//

#include <iostream>

int maxRow = 6;
int maxCol = 8;

bool isValid(int row,int col){
    return(row < maxRow && col < maxCol);
}

void blockTraversal(int row, int col, int width) {

    if(!isValid(row,col)){
        return;
    }

    int newWidth = width / 2;

    if(width == 1){
        std::cout << row << "," << col << std::endl;
        return;
    }

    blockTraversal(row, col, newWidth);
    blockTraversal(row, col + newWidth, newWidth);
    blockTraversal(row + newWidth, col, newWidth);
    blockTraversal(row + newWidth, col + newWidth, newWidth);


}

int main(){
    blockTraversal(0, 0,8);


}

