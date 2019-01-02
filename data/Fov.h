//
// Created by Gerardin, Antoine D. (Assoc) on 1/2/19.
//

#ifndef PYRAMIDBUILDING_FOV_H
#define PYRAMIDBUILDING_FOV_H

#include <string>
#include <opencv/cv.h>

class Fov {

public:
    Fov() {}


    Fov(const std::string &path, uint32_t x, uint32_t y) : path(path), x(x), y(y) {}

    Fov(const std::string &path, uint32_t x, uint32_t y, const cv::Rect &tileOverlap) : path(path), x(x), y(y),
                                                                                        tileOverlap(tileOverlap) {}


private:

    std::string path;
    uint32_t x;
    uint32_t y;
    cv::Rect tileOverlap;

};

#endif //PYRAMIDBUILDING_FOV_H
