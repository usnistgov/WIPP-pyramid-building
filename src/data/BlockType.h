//
// Created by Gerardin, Antoine D. (Assoc) on 12/27/18.
//

#ifndef PYRAMIDBUILDING_BLOCKTYPE_H
#define PYRAMIDBUILDING_BLOCKTYPE_H

#include <string>

enum class BlockType {
  Single, //single block (bottom right or top-level)
  Vertical, //right vertical block
  Horizontal, //bottom horizontal block
  Full //regular block
};

std::string blockTypeToString(BlockType type);


#endif //PYRAMIDBUILDING_BLOCKTYPE_H
