//
// Created by Gerardin, Antoine D. (Assoc) on 2019-04-30.
//

#include <iostream>
#include <cmath>
#include <glog/logging.h>
#include <pyramidBuilding/fastImage/RecursiveBlockTraversal.h>
#include <assert.h>

int main(){

//    auto numTileRow = 61;
//    auto numTileCol = 88;

    auto numTileRow = 12;
    auto numTileCol = 8;

    auto count = 0;

    auto traversal = new pb::RecursiveBlockTraversal(numTileRow,numTileCol);

    for(auto step : traversal->getTraversal()){
        count++;
        auto row = step.first;
        auto col = step.second;
        VLOG(1) << row << "," << col;

    }

    VLOG(1) << numTileRow * numTileCol;
    VLOG(1) << count;
    assert(count == numTileRow * numTileCol);




}

