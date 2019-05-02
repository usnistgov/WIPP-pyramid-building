//
// Created by gerardin on 5/2/19.
//

#ifndef PYRAMIDBUILDING_PYRAMID_H
#define PYRAMIDBUILDING_PYRAMID_H


#include <cstdio>
#include <algorithm>
#include <cmath>
#include <array>
#include <vector>

namespace pb {


    class Pyramid {


    public :

        Pyramid(size_t numTileRow, size_t numTileCol) : numTileRow(numTileRow), numTileCol(numTileCol) {

            //calculate pyramid depth
            auto maxDim = std::max(numTileCol, numTileRow);
            numLevel = static_cast<size_t>(ceil(log2(maxDim)) + 1);

            //calculate number of tiles for each level
            size_t
                    levelCol, levelRow;
            levelCol = numTileCol;
            levelRow = numTileRow;
            for (size_t l = 0; l < numLevel; l++) {
                std::array<size_t, 2> gridSize = {(size_t)levelRow, (size_t)levelCol};
                levelGridSizes.push_back(gridSize);
                levelCol = static_cast<size_t>(ceil((double) levelCol / 2));
                levelRow = static_cast<size_t>(ceil((double) levelRow / 2));
            }
        }


        size_t getNumTileCol(int level) {
            assert(level < numLevel);
            return levelGridSizes[level][1];
        }

        size_t getNumTileRow(int level) {
            assert(level < numLevel);
            return levelGridSizes[level][0];
        }

        size_t getNumLevel() const {
            return numLevel;
        }

    private :
            size_t numTileCol;
            size_t numTileRow;
            size_t numLevel;
            std::vector<std::array<size_t,2>> levelGridSizes;
    };
}


#endif //PYRAMIDBUILDING_PYRAMID_H
