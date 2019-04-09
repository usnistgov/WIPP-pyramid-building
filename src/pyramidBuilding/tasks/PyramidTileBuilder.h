//
// Created by Gerardin, Antoine D. (Assoc) on 4/8/19.
//

#ifndef PYRAMIDBUILDING_PYRAMIDTILEBUILDER_H
#define PYRAMIDBUILDING_PYRAMIDTILEBUILDER_H


#include <htgs/api/MemoryData.hpp>
#include <pyramidBuilding/data/Tile.h>
#include <htgs/api/ITask.hpp>

namespace pb {

template<class T>
class PyramidTileBuilder : public htgs::ITask<htgs::MemoryData<T>, Tile<T>>
{

public:
    void executeTask(std::shared_ptr<htgs::MemoryData<T>> data) override {
        data->releaseMemory();
    }

    htgs::ITask <htgs::MemoryData<T>, Tile<T>> *copy() override {
        return nullptr;
    }


private:
    TileCache tileCache;

};


}

#endif //PYRAMIDBUILDING_PYRAMIDTILEBUILDER_H
