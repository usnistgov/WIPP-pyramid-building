//
// Created by Gerardin, Antoine D. (Assoc) on 12/27/18.
//

#ifndef PYRAMIDBUILDING_BLOCKTYPE_H
#define PYRAMIDBUILDING_BLOCKTYPE_H

#include <string>

namespace pb {

    enum class BlockType {
        Single, //single block (bottom right or top-level)
        Vertical, //right vertical block
        Horizontal, //bottom horizontal block
        Full //regular block
    };

    std::string blockTypeToString(BlockType type) {
        switch (type) {
            case BlockType::Single:
                return "Single";
            case BlockType::Vertical:
                return "Block of 2 - Vertical";
            case BlockType::Horizontal:
                return "Block of 2 - Horizontal";
            case BlockType::Full:
                return " Block of 4 - Full";
            default:
                return "block error";
        }
    }
}

#endif //PYRAMIDBUILDING_BLOCKTYPE_H
