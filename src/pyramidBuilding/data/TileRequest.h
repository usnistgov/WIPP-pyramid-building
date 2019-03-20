//
// Created by Gerardin, Antoine D. (Assoc) on 1/21/19.
//

#ifndef PYRAMIDBUILDING_TILEREQUEST_H
#define PYRAMIDBUILDING_TILEREQUEST_H

#include <htgs/api/IData.hpp>

namespace pb {

    class TileRequest : htgs::IData {


    public:

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
