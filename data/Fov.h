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

    Fov(const std::string &path, uint32_t x, uint32_t y, const cv::Rect &tileOverlap,
        const cv::Rect &globalCoordinatesTileOverlap) : path(path), x(x), y(y), tileOverlap(tileOverlap),
                                                        globalCoordinatesTileOverlap(globalCoordinatesTileOverlap) {}

    Fov(const std::string &path, uint32_t x, uint32_t y, const cv::Rect &tileOverlap,
        const cv::Rect &globalCoordinatesTileOverlap, const cv::Rect &FovCoordOverlap) : path(path), x(x), y(y),
                                                                                         tileOverlap(tileOverlap),
                                                                                         globalCoordinatesTileOverlap(
                                                                                                 globalCoordinatesTileOverlap),
                                                                                         FovCoordOverlap(
                                                                                                 FovCoordOverlap) {}


    const std::string &getPath() const {
        return path;
    }

    uint32_t getX() const {
        return x;
    }

    uint32_t getY() const {
        return y;
    }

    const cv::Rect &getTileOverlap() const {
        return tileOverlap;
    }

    const cv::Rect &getGlobalCoordinatesTileOverlap() const {
        return globalCoordinatesTileOverlap;
    }

    const cv::Rect &getFovCoordOverlap() const {
        return FovCoordOverlap;
    }

private:

    std::string path;
    uint32_t x;
    uint32_t y;
    cv::Rect tileOverlap;
    cv::Rect globalCoordinatesTileOverlap;
    cv::Rect FovCoordOverlap;

};

#endif //PYRAMIDBUILDING_FOV_H
