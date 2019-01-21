//
// Created by Gerardin, Antoine D. (Assoc) on 1/21/19.
//

#ifndef PYRAMIDBUILDING_TILEREQUEST_H
#define PYRAMIDBUILDING_TILEREQUEST_H

#include <htgs/api/IData.hpp>

class TileRequest : htgs::IData {



public:

    TileRequest(uint32_t row, uint32_t col) : row(row), col(col) {}

    uint32_t getRow() const {
        return row;
    }

    uint32_t getCol() const {
        return col;
    }


private:

    uint32_t row;
    uint32_t col;


};

#endif //PYRAMIDBUILDING_TILEREQUEST_H
