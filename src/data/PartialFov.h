//
// Created by Gerardin, Antoine D. (Assoc) on 1/2/19.
//

#ifndef PYRAMIDBUILDING_PARTIALFOV_H
#define PYRAMIDBUILDING_PARTIALFOV_H

#include <string>
#include <opencv/cv.h>

class PartialFov {

public:
    PartialFov() {}


    PartialFov(const std::string &path, size_t x, size_t y) : path(path), x(x), y(y) {}

    PartialFov(const std::string &path, size_t x, size_t y, const cv::Rect &tileOverlap) : path(path), x(x), y(y),
                                                                                        tileOverlap(tileOverlap) {}

    PartialFov(const std::string &path, size_t x, size_t y, const cv::Rect &tileOverlap,
        const cv::Rect &globalCoordinatesTileOverlap) : path(path), x(x), y(y), tileOverlap(tileOverlap),
                                                        globalCoordinatesTileOverlap(globalCoordinatesTileOverlap) {}

    PartialFov(const std::string &path, size_t x, size_t y, const cv::Rect &tileOverlap,
        const cv::Rect &globalCoordinatesTileOverlap, const cv::Rect &FovCoordOverlap) : path(path), x(x), y(y),
                                                                                         tileOverlap(tileOverlap),
                                                                                         globalCoordinatesTileOverlap(
                                                                                                 globalCoordinatesTileOverlap),
                                                                                         FovCoordOverlap(
                                                                                                 FovCoordOverlap) {}


    const std::string &getPath() const {
        return path;
    }

    size_t getX() const {
        return x;
    }

    size_t getY() const {
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
    size_t x;
    size_t y;
    cv::Rect tileOverlap;
    cv::Rect globalCoordinatesTileOverlap;
    cv::Rect FovCoordOverlap;

};

#endif //PYRAMIDBUILDING_PARTIALFOV_H
