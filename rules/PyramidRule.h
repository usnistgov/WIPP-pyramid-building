//
// Created by Gerardin, Antoine D. (Assoc) on 12/20/18.
//

#ifndef PYRAMIDBUILDING_PYRAMIDRULE_H
#define PYRAMIDBUILDING_PYRAMIDRULE_H

#include <FastImage/api/FastImage.h>
#include <math.h>
#include <array>

class PyramidRule : public htgs::IRule<htgs::MemoryData<fi::View<uint32_t>>,htgs::MemoryData<fi::View<uint32_t>>> {

public:
    PyramidRule(uint32_t numTileCol, uint32_t numTileRow) :  numTileCol(numTileCol), numTileRow(numTileRow) {
        auto maxDim = std::max(numTileCol,numTileRow);

        numLevel = ceil(log2(maxDim)) + 1;

        std::vector<std::vector<std::vector<fi::View<uint32_t>>>> levels ;

        levelGridSizes.push_back({numTileCol, numTileRow});

        double levelCol, levelRow;
        levelCol = numTileCol;
        levelRow = numTileRow;

        for(uint32_t l=1; l<numLevel; l++){
            levelCol = ceil(levelCol/2);
            levelRow = ceil(levelRow /2);
            std::array<uint32_t,2> gridSize = { (uint32_t)levelCol, (uint32_t)levelRow };
            levelGridSizes.push_back(gridSize);
        }
    }

    void applyRule(std::shared_ptr<MemoryData<fi::View<uint32_t>>> data, size_t pipelineId) override {

        auto col = data->get()->getCol();
        auto row = data->get()->getRow();
        auto level = data->get()->getPyramidLevel();

        auto blockCol = floor(col / 2);
        auto blockRow = floor(row / 2);

        std::cout << "apply pyramid rule" << std::endl;
        std::cout << "tile : (" << col << "," << row << ") ; block (" << blockCol << "," << blockRow << ")"  << std::endl;
        std::cout << "level: " << level << " ; grid size: (" << levelGridSizes[level][0] << "," <<  levelGridSizes[level][1] << ")" << std::endl;

        //add to block

        //if blockISfull, send block

        if(col % 2 != 0) {
            std::cout << "col is odd number " << std::endl;
            if(col == levelGridSizes[level][0] - 1){
                std::cout << "end of col" << std::endl;
            }
        }

    }


private:
    uint32_t numTileCol;
    uint32_t numTileRow;
    uint32_t numLevel;
    std::vector<std::array<uint32_t,2>> levelGridSizes;



};

#endif //PYRAMIDBUILDING_PYRAMIDRULE_H
