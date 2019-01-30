//
// Created by Gerardin, Antoine D. (Assoc) on 1/21/19.
//

#ifndef PYRAMIDBUILDING_BASETILETASK_H
#define PYRAMIDBUILDING_BASETILETASK_H

#include <htgs/api/ITask.hpp>
#include "../data/BlockRequest.h"
#include "../data/TileRequest.h"
#include "../utils/BaseTileGenerator.h"

template <class T>
class BaseTileTask : public htgs::ITask<TileRequest , Tile<T>> {

public:

    BaseTileTask(size_t numThreads, BaseTileGenerator<T> *generator) : htgs::ITask<TileRequest , Tile<T>>(numThreads), generator(generator) {}

    void executeTask(std::shared_ptr<TileRequest> data) override {
        size_t i = data.get()->getRow();
        size_t j= data.get()->getCol();
        std::pair<size_t,size_t> index= std::make_pair(i,j);
        auto t = generator->generateTile(index);
        this->addResult(t);
    }

    ITask<TileRequest, Tile<T>> *copy() override {
        return new BaseTileTask(this->getNumThreads(), generator);
    }

    std::string getName() override {
        return "Base Tile Task";
    }

private:

    BaseTileGenerator<T>* generator;



};

#endif //PYRAMIDBUILDING_BASETILETASK_H
