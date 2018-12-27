//
// Created by Gerardin, Antoine D. (Assoc) on 12/20/18.
//

#ifndef PYRAMIDBUILDING_CREATETILETASK_H
#define PYRAMIDBUILDING_CREATETILETASK_H

#include <htgs/api/ITask.hpp>
#include <FastImage/api/FastImage.h>
#include "../data/TileRequest.h"
#include "../data/FakeTile.h"
#include "../rules/MatrixMemoryRule.h"

class CreateTileTask : public htgs::ITask<TileRequest<uint32_t>, htgs::MemoryData<fi::View<uint32_t>> > {


public:
    CreateTileTask() {}

    void executeTask(std::shared_ptr<TileRequest<uint32_t>> data) override {
        auto block = data->getBlock();

        uint32_t level = 0;
        auto tile = new FakeTile<uint32_t>(0,0,0);
        htgs::m_data_t<fi::View<uint32_t>> t ;

        switch (block.size()){
            //bottom right single block
            case 1:
                //downsample and send new tile
                level = block[0]->get()->getPyramidLevel();
                tile = new FakeTile<uint32_t>(1024, 1024,level+1);
                t = this->getMemory<fi::View<uint32_t>>("PYRAMID_TILE", new MatrixMemoryRule(1));
                this->addResult(t.get());
                break;
            //bottom horizontal block
            case 2:
                //blend resize and downsample and send new tile
                level = block[0]->get()->getPyramidLevel();
                tile = new FakeTile<uint32_t>(1024, 1024,level+1);
                t = this->getMemory<fi::View<uint32_t>>("PYRAMID_TILE", new MatrixMemoryRule(1));
                this->addResult(t);
                break;
            //right vertical block
            case 3:
                //blend resize and downsample and send new tile
                level = block[0]->get()->getPyramidLevel();
                tile = new FakeTile<uint32_t>(1024, 1024,level+1);
                t = this->getMemory<fi::View<uint32_t>>("PYRAMID_TILE", new MatrixMemoryRule(1));
                this->addResult(t);
                break;
            //regular block
            case 4:
                //blend resize and downsample and send new tile
                level = block[0]->get()->getPyramidLevel();
                tile = new FakeTile<uint32_t>(1024, 1024,level+1);
                t = this->getMemory<fi::View<uint32_t>>("PYRAMID_TILE", new MatrixMemoryRule(1));
                this->addResult(t);
                break;
        }
    }


    ITask<TileRequest<uint32_t>, htgs::MemoryData<fi::View<uint32_t>>> *copy() override {
        return new CreateTileTask();
    }


};


#endif //PYRAMIDBUILDING_CREATETILETASK_H
