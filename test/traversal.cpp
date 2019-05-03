//
// Created by Gerardin, Antoine D. (Assoc) on 2019-04-30.
//

#include <iostream>
#include <cmath>
#include <glog/logging.h>
#include <pyramidBuilding/pyramid/RecursiveBlockTraversal.h>
#include <assert.h>

int main(){


    size_t width = 111009;
    size_t height = 108972;
    size_t tileSize =1024;

    auto count = 0;

    auto pyramid = pb::Pyramid(width, height, tileSize);

    auto traversal = new pb::RecursiveBlockTraversal(pyramid);

    for(auto step : traversal->getTraversal()){
        count++;
        auto row = step.first;
        auto col = step.second;
        VLOG(1) << row << "," << col;

    }

    VLOG(1) << pyramid.getPyramidWidth(0);
    VLOG(1) << pyramid.getPyramidHeight(0);
    VLOG(1) << count;
    assert(count == pyramid.getPyramidWidth(0) * pyramid.getPyramidHeight(0));




}

