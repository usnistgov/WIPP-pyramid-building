//
// Created by Gerardin, Antoine D. (Assoc) on 1/2/19.
//

#ifndef PYRAMIDBUILDING_PARTIALFOV_H
#define PYRAMIDBUILDING_PARTIALFOV_H

#include <string>
#include <opencv/cv.h>

namespace pb {

    /**
     * @class PartialFov PartialFov.h <pyramidBuilding/data/PartialFov.h>
     * @brief Represents the overlap of a pyramid tile and a Field of View (FOV).
     * @details The overlap is represented in several coordinates system to enable building blended tiles.
     */
    class PartialFov {

    public:
        /***
         * @param filename to the FOV on disk.
         * @param tileOverlap overlap of the FOV in the coordinates of a pyramid tile.
         * @param globalCoordinatesTileOverlap overlap of the FOV in the global coordinates of the full FOV.
         * @param FovCoordOverlap overlap of the tile in the FOV coordinates.
         */
        PartialFov(const std::string &filename, const cv::Rect &tileOverlap,
                   const cv::Rect &globalCoordinatesTileOverlap, const cv::Rect &FovCoordOverlap) : filename(filename),
                                                                                                    tileOverlap(
                                                                                                            tileOverlap),
                                                                                                    globalCoordinatesTileOverlap(
                                                                                                            globalCoordinatesTileOverlap),
                                                                                                    FovCoordOverlap(
                                                                                                            FovCoordOverlap) {}


        const std::string &getFilename() const {
            return filename;
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
        std::string filename;
        cv::Rect tileOverlap;
        cv::Rect globalCoordinatesTileOverlap;
        cv::Rect FovCoordOverlap;

    };

}

#endif //PYRAMIDBUILDING_PARTIALFOV_H
