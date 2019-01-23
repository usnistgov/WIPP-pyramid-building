//
// Created by Gerardin, Antoine D. (Assoc) on 12/27/18.
//

#include "BlockType.h"
#include <string>

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
    }
}