//
// Created by Gerardin, Antoine D. (Assoc) on 12/19/18.
//

#ifndef PYRAMIDBUILDING_TILEBLOCK_H
#define PYRAMIDBUILDING_TILEBLOCK_H

#include <htgs/api/IData.hpp>
#include <FastImage/api/FastImage.h>
#include "./Tile.h"
#include "BlockType.h"

namespace pb {

    /**
     * @class BlockRequest BlockRequest.h <pyramidBuilding/data/BlockRequest.h>
     * @brief Represents a request to generate a new tile from a block of tiles.
     * @details Regular blocks of tiles have size 4. If the full FOV is not wholly divisible by the tile size,
     * then we can also produce vertical or horizontal blocks of size 2.
     * The bottom right corner of the image can also produce a block of size 1.
    */
    template<class T>
    class TileBlock : public htgs::IData {

    public:

        TileBlock(const std::vector<std::shared_ptr<Tile<T>>> &block) :
        block(block) {

                assert(block.size() > 0);
                //More test could happen : tiles are adjacent and at the same level...

                auto t = block[0]; //top left tile gives us the block coordinates
                row = t->getRow() / 2;
                col = t->getCol() / 2;
                level = t->getLevel() + 1;

                switch (block.size()) {
                    case 1:
                        blockType = BlockType::Single;
                    break;
                    case 2:
                        blockType = BlockType::Horizontal;
                    break;
                    case 3:
                        blockType = BlockType::Vertical;
                    break;
                    case 4:
                        blockType = BlockType::Full;
                    break;
                    default:
                        std::runtime_error("fatal error. Unknown block Request Type.");
                }
        }

        std::vector<std::shared_ptr<Tile<T>>> &getBlock() {
            return block;
        }

        size_t getLevel() const {
            return level;
        }

        size_t getRow() const {
            return row;
        }

        size_t getCol() const {
            return col;
        }

        BlockType getType() const {
            return blockType;
        }

        /// \brief Output operator stream to print a BlockRequest
        /// \param os Stream BlockRequest information will be pushed to
        /// \param block The BlockRequest to print
        /// \return the output stream with the information
        /// \note Used for debugging only
        friend std::ostream &operator<<(std::ostream &os, const TileBlock &block) {
            os << "Tile - Level: " << block.level + 1 << " from: Block - Level: " << block.level << " Index: ("
               << block.row << "," << block.col << ")" << "Type: " << blockTypeToString(block.blockType) << std::endl;
            return os;
        }

    private:
        std::vector<std::shared_ptr<Tile<T>>> block;
        size_t row;
        size_t col;
        size_t level;
        BlockType blockType;


    };

}

#endif //PYRAMIDBUILDING_TILEBLOCK_H
