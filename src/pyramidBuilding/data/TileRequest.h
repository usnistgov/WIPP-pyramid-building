//
// Created by Gerardin, Antoine D. (Assoc) on 1/21/19.
//

#ifndef PYRAMIDBUILDING_TILEREQUEST_H
#define PYRAMIDBUILDING_TILEREQUEST_H

#include <htgs/api/IData.hpp>

namespace pb {

    /***
     * @class TileRequest TileRequest.h <pyramidBuilding/data/TileRequest.h>
     * @brief request to read a tile from disk.
     */
    class TileRequest : htgs::IData {


    public:
        /**
         *
         * @param row of the tile we which to get.
         * @param col of the tile we which to get.
         */
        TileRequest(size_t row, size_t col) : row(row), col(col) {}

        size_t getRow() const {
            return row;
        }

        size_t getCol() const {
            return col;
        }


    private:

        size_t row;
        size_t col;


    };

}

#endif //PYRAMIDBUILDING_TILEREQUEST_H
