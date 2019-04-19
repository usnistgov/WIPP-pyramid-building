//
// Created by gerardin on 4/19/19.
//

#ifndef PYRAMIDBUILDING_FITILEREQUEST_H
#define PYRAMIDBUILDING_FITILEREQUEST_H

#include <htgs/api/IData.hpp>

namespace pb {

    /***
     * @class TileRequest TileRequest.h <pyramidBuilding/data/TileRequest.h>
     * @brief request to read a tile from disk.
     */
    class FITileRequest : htgs::IData {


    public:

        FITileRequest(size_t row, size_t col, size_t width, size_t height, const std::vector<PartialFOV> &fovs) : row(
                row), col(col), width(width), height(height), fovs(fovs) {}


        size_t getRow() const {
            return row;
        }

        size_t getCol() const {
            return col;
        }

        size_t getWidth() const {
            return width;
        }

        size_t getHeight() const {
            return height;
        }

        const std::vector<PartialFOV> &getFovs() const {
            return fovs;
        }


    private:

        size_t row;
        size_t col;
        size_t width;
        size_t height;
        std::vector<PartialFOV> fovs;


    };

}

#endif //PYRAMIDBUILDING_FITILEREQUEST_H
