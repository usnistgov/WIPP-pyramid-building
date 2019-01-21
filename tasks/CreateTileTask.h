//
// Created by Gerardin, Antoine D. (Assoc) on 12/20/18.
//

#ifndef PYRAMIDBUILDING_CREATETILETASK_H
#define PYRAMIDBUILDING_CREATETILETASK_H

#include <htgs/api/ITask.hpp>
#include <FastImage/api/FastImage.h>
#include "data/BlockRequest.h"
#include "../data/FakeTile.h"
#include "../rules/MatrixMemoryRule.h"

class CreateTileTask : public htgs::ITask<BlockRequest<uint32_t>, Tile<uint32_t> > {


public:
    CreateTileTask() {}

    void executeTask(std::shared_ptr<BlockRequest<uint32_t>> data) override {
        auto block = data->getBlock();

        std::cout << "Create Tile Task - " << *data.get() << std::endl;

        uint32_t level = block[0]->getLevel();
        uint32_t row = floor(block[0]->getRow() / 2);
        uint32_t col = floor(block[0]->getCol() /2);

        Tile<uint32_t> *tile = nullptr;
        htgs::m_data_t<fi::View<uint32_t>> t ;

        switch (block.size()){
            //bottom right single block
            case 1:
                //downsample and send new tile
                tile = new Tile<uint32_t>(row,col,level+1, nullptr);
                break;
            //bottom horizontal block
            case 2:
                //blend resize and downsample and send new tile
                level = block[0]->getLevel();
                tile = new Tile<uint32_t>(row,col,level+1, nullptr);
                break;
            //right vertical block
            case 3:
                //blend resize and downsample and send new tile
                level = block[0]->getLevel();
                tile = new Tile<uint32_t>(row,col,level+1, nullptr);
                break;
            //regular block
            case 4:
                //blend resize and downsample and send new tile
                level = block[0]->getLevel();
                tile = new Tile<uint32_t>(row,col,level+1, nullptr);
                break;
        }

        this->addResult(tile);
    }

    std::string getName() override {
        return "Pyramid Tile Task";
    }

    htgs::ITask<BlockRequest<uint32_t>, Tile<uint32_t> > *copy() override {
        return new CreateTileTask();
    }


};


#endif //PYRAMIDBUILDING_CREATETILETASK_H
