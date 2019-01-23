//
// Created by Gerardin, Antoine D. (Assoc) on 1/21/19.
//

#ifndef PYRAMIDBUILDING_OUTPUTTASK_H
#define PYRAMIDBUILDING_OUTPUTTASK_H

#include <htgs/api/ITask.hpp>
#include "FastImage/api/FastImage.h"
#include "../data/Tile.h"
#include <tiffio.h>

class OutputTask : public htgs::ITask< Tile<uint32_t>, Tile<uint32_t> > {

public:


    void executeTask(std::shared_ptr<Tile<uint32_t>> data) override {
        addResult(data);
    }

    std::string getName() override { return "OutputTask"; }

    OutputTask *copy() override {
        return new OutputTask();
    }
};


#endif //PYRAMIDBUILDING_OUTPUTTASK_H
