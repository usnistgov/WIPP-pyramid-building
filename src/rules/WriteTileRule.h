//
// Created by Gerardin, Antoine D. (Assoc) on 12/19/18.
//

#ifndef PYRAMIDBUILDING_WRITETILERULE_H
#define PYRAMIDBUILDING_WRITETILERULE_H

#include <cstdint>
#include <FastImage/api/FastImage.h>
#include "../data/Tile.h"

class WriteTileRule : public htgs::IRule< Tile<uint32_t>, Tile<uint32_t>> {
public:
    WriteTileRule(uint32_t level = 0) : level(level) {};

    void applyRule(std::shared_ptr<Tile<uint32_t>> data, size_t pipelineId) override {

        uint32_t l = data->getLevel();
        if(l >= level) {
            this->addResult(data);
        }

     //   data.reset();
    }


    std::string getName() override {
        return "Write Rule";
    }

private:
    uint32_t level;

};

#endif //PYRAMIDBUILDING_WRITETILERULE_H
