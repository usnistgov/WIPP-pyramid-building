//
// Created by gerardin on 4/30/19.
//

#ifndef PYRAMIDBUILDING_RECURSIVEBLOCKTRAVERSAL_H
#define PYRAMIDBUILDING_RECURSIVEBLOCKTRAVERSAL_H

#include <cstdint>
#include <glog/logging.h>
#include <cmath>

namespace pb {

    class RecursiveBlockTraversal {

    public:
        RecursiveBlockTraversal(uint32_t maxRow, uint32_t maxCol) : maxRow(maxRow), maxCol(maxCol) {
            auto maxDim = std::max(maxRow, maxCol);
            blockTraversal(0,0, maxDim);
        }


        bool isValid(int row,int col){
            return(row < maxRow && col < maxCol);
        }

        void blockTraversal(int row, int col, int width) {

            //deal with matrices not square.
            if(!isValid(row,col)){
                return;
            }

            if(width == 1){
                VLOG(4) << row << "," << col;
                traversal.emplace_back(row,col);
                return;
            }

            //deal with odd dimensions.
            int newWidth = (int)ceil((double)width / 2);
            if(newWidth !=1 && newWidth % 2 != 0){
                ++newWidth;
            }

//            std::cout << "start: " << row << "," << col << std::endl;

            blockTraversal(row, col, newWidth);
            blockTraversal(row, col + newWidth, newWidth);
            blockTraversal(row + newWidth, col, newWidth);
            blockTraversal(row + newWidth, col + newWidth, newWidth);


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
