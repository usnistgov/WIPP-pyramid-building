//
// Created by Gerardin, Antoine D. (Assoc) on 12/19/18.
//

#ifndef PYRAMIDBUILDING_TILEREQUEST_H
#define PYRAMIDBUILDING_TILEREQUEST_H

#include <htgs/api/IData.hpp>
#include <FastImage/api/FastImage.h>
#include "./Tile.h"
#include "BlockType.h"

template <class Type>
class TileRequest : public htgs::IData {

public:
    TileRequest(const std::vector<std::shared_ptr<Tile<Type>>> &block) : block(block) {
        if(block.size() <= 0){
            //CHECK HOW TO DO IN C++
            //assert error
        }
        //More test could happen : tiles are adjacent at the same level...
        //For now we will expect correctness.
        auto t = block[0];
        row = t->getRow() / 2;
        col = t->getCol() / 2;
        level = t->getLevel();
        blockType = initBlockType();
    }

    const std::vector<std::shared_ptr<Tile<Type>>> &getBlock() const {
        return block;
    }

    /// \brief Output operator stream to print a TileRequest
    /// \param os Stream to put the tileRequest information
    /// \param data The TileRequest to print
    /// \return the output stream with the information
    /// \note Used for debugging only
    friend std::ostream &operator<<(std::ostream &os, const TileRequest &block) {
        // auto info = printf("Block Info - Level: %d. Index: (%d , %d). Type: %s" , block.level, block.row, block.col, blockTypeToString(block.blockType));
//        auto info = std::string("Block - Level: ") + block.level + " Index: (" + block.row + "," + block.col + ")" + "Type: " +  blockTypeToString(block.blockType);
//        os << info << std:endl;
        os << "Block - Level: " << block.level << " Index: (" << block.row << "," << block.col << ")" << "Type: " << blockTypeToString(block.blockType) << std::endl;
        return os;
    }

private:

    BlockType initBlockType() {
        switch (block.size()) {
            case 1:
                return BlockType::Single;
            case 2:
                return BlockType::Horizontal;
            case 3:
                return BlockType::Vertical;
            case 4:
                return BlockType::Full;
        }
    }

    std::vector<std::shared_ptr<Tile<Type>>> block;
    uint32_t row;
    uint32_t col;
    uint32_t level;
    BlockType blockType;



};

#endif //PYRAMIDBUILDING_TILEREQUEST_H
