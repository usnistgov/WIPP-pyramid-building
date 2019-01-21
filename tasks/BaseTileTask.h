//
// Created by Gerardin, Antoine D. (Assoc) on 1/21/19.
//

#ifndef PYRAMIDBUILDING_BASETILETASK_H
#define PYRAMIDBUILDING_BASETILETASK_H

#include <htgs/api/ITask.hpp>
#include <data/BlockRequest.h>
#include <data/TileRequest.h>
#include <utils/BaseTileGenerator.h>

class BaseTileTask : public htgs::ITask<TileRequest , Tile<uint32_t>> {

public:

    BaseTileTask(BaseTileGenerator *generator) : generator(generator) {}

    void executeTask(std::shared_ptr<TileRequest> data) override {
        uint32_t i = data.get()->getRow();
        uint32_t j= data.get()->getCol();
        std::pair<uint32_t,uint32_t> index= std::make_pair(i,j);
        auto t = generator->generateTile(index);
        auto tile = new Tile<uint32_t>(i,j,0,t);
        this->addResult(tile);
    }

    ITask<TileRequest, Tile<uint32_t>> *copy() override {
        return new BaseTileTask(generator);
    }

private:

    BaseTileGenerator* generator;



};

#endif //PYRAMIDBUILDING_BASETILETASK_H
