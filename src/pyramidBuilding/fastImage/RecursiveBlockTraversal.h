//
// Created by gerardin on 4/30/19.
//

#ifndef PYRAMIDBUILDING_RECURSIVEBLOCKTRAVERSAL_H
#define PYRAMIDBUILDING_RECURSIVEBLOCKTRAVERSAL_H

#include <cstdint>
#include <glog/logging.h>
#include <cmath>
#include <assert.h>

namespace pb {

    class RecursiveBlockTraversal {

    public:
        RecursiveBlockTraversal(uint32_t maxRow, uint32_t maxCol) : maxRow(maxRow), maxCol(maxCol) {
            auto maxDim = std::max(maxRow, maxCol);
            blockTraversal(0,0, maxCol, maxRow);
        }


        bool isValid(uint32_t row,uint32_t col){
            return(row < maxRow && col < maxCol);
        }

        void blockTraversal(uint32_t row, uint32_t col, uint32_t width, uint32_t height) {

            //deal with matrices not square.
            if(!isValid(row,col)){
                assert(false);
                return;
            }

            if(width == 1 && height == 1){
                VLOG(4) << row << "," << col;
                traversal.emplace_back(row,col);
                return;
            }

            //deal with odd dimensions.
            uint32_t newWidth = (uint32_t)ceil((double)width / 2);
            uint32_t newHeight = (uint32_t)ceil((double)height / 2);

            auto widthMod = newWidth % 4;
            auto heightMod = newHeight % 4;


            std::cout << "start: " << row << "," << col << std::endl;

            blockTraversal(row, col, newWidth + widthMod, newHeight + heightMod);
            blockTraversal(row, col + newWidth + widthMod, newWidth - widthMod, newHeight + heightMod);
            if(width % 2 != 0) {
                blockTraversal(row, col + 2 * newWidth, 1, newHeight + heightMod);
            }

            blockTraversal(row + newHeight, col, newWidth + widthMod, newHeight -heightMod);
            blockTraversal(row + newHeight, col + newWidth + widthMod, newWidth - widthMod, newHeight - heightMod);
            if(width % 2 != 0) {
                blockTraversal(row + newHeight, col + 2 * newWidth, 1, newHeight - heightMod);
            }

            if(height % 2 != 0){
                blockTraversal(row + 2 * newHeight, col, newWidth + widthMod, 1);
                blockTraversal(row + 2 * newHeight, col + newWidth + widthMod, newWidth - widthMod, 1);
            }

            if(height % 2 != 0 && width % 2 != 0){
                blockTraversal(row + 2 * newHeight, col + 2 * newWidth, 1, 1);
            }


        }


        const std::vector<std::pair<uint32_t, int32_t>> &getTraversal() const {
            return traversal;
        }

    private:

        uint32_t maxRow = 0;
        uint32_t maxCol = 0;

        std::vector<std::pair<uint32_t ,int32_t>> traversal;


    };


}



#endif //PYRAMIDBUILDING_RECURSIVEBLOCKTRAVERSAL_H
