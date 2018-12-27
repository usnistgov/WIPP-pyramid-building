//
// Created by Gerardin, Antoine D. (Assoc) on 12/19/18.
//

#ifndef PYRAMIDBUILDING_TILEREQUEST_H
#define PYRAMIDBUILDING_TILEREQUEST_H

#include <htgs/api/IData.hpp>
#include <FastImage/api/FastImage.h>
#include "./Tile.h"

template <class Type>
class TileRequest : public htgs::IData {

public:
    TileRequest(const std::vector<std::shared_ptr<Tile<Type>>> &block) : block(block) {}

    const std::vector<std::shared_ptr<Tile<Type>>> &getBlock() const {
        return block;
    }

private:
    std::vector<std::shared_ptr<Tile<Type>>> block;


};

#endif //PYRAMIDBUILDING_TILEREQUEST_H
