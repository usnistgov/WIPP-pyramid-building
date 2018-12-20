//
// Created by Gerardin, Antoine D. (Assoc) on 12/20/18.
//

#ifndef PYRAMIDBUILDING_PYRAMIDRULE_H
#define PYRAMIDBUILDING_PYRAMIDRULE_H

#include <FastImage/api/FastImage.h>
#include <math.h>

class PyramidRule : public htgs::IRule<htgs::MemoryData<fi::View<uint32_t>>,htgs::MemoryData<fi::View<uint32_t>>> {

public:
    PyramidRule(uint32_t numTileCol, uint32_t numTileRow) :  numTileCol(numTileCol), numTileRow(numTileRow) {
        auto maxDim = std::max(numTileCol,numTileRow);
        uint32_t numLevel = ceil(log2(maxDim));

        std::vector<std::vector<fi::View<uint32_t>>> levels ;

        for(uint32_t l=0; l<numLevel; l++){
            std::cout << "level " << l << std::endl;
        }
    }

    void applyRule(std::shared_ptr<MemoryData<fi::View<uint32_t>>> data, size_t pipelineId) override {

        

    }


private:
    uint32_t numTileCol;
    uint32_t numTileRow;
    uint32_t numLevel;



};

#endif //PYRAMIDBUILDING_PYRAMIDRULE_H
