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
        uint32_t numLevel = ceil(log2(maxDim));

        std::vector<std::vector<std::vector<fi::View<uint32_t>>>> levels ;

        double levelCol, levelRow;
        levelCol = numTileCol;
        levelRow = numTileRow;

        for(uint32_t l=0; l<numLevel; l++){
            std::cout << "level " << l << std::endl;
            std::array<uint32_t,2> gridSize = { (uint32_t)ceil(levelCol/2), (uint32_t)ceil(levelRow /2 ) };
            levelGridSizes.push_back(gridSize);
            levelCol = (uint32_t)ceil(levelCol/2);
            levelRow = (uint32_t)ceil(levelRow /2);
        }

    }

    void applyRule(std::shared_ptr<MemoryData<fi::View<uint32_t>>> data, size_t pipelineId) override {

        auto col = data->get()->getCol();
        auto row = data->get()->getRow();
        auto level = data->get()->getPyramidLevel();

        auto blockCol = floor(col / 2);
        auto blockRow = floor(row / 2);

        if(col % 2 == 0){
            
        }

    }


private:
    uint32_t numTileCol;
    uint32_t numTileRow;
    uint32_t numLevel;
    std::vector<std::array<uint32_t,2>> levelGridSizes;



};

#endif //PYRAMIDBUILDING_PYRAMIDRULE_H
