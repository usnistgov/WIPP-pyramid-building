//
// Created by Gerardin, Antoine D. (Assoc) on 12/19/18.
//

#ifndef PYRAMIDBUILDING_WRITETILETASK_H
#define PYRAMIDBUILDING_WRITETILETASK_H

#include <htgs/api/ITask.hpp>
#include "FastImage/api/FastImage.h"
#include "../data/Tile.h"

class WriteTileTask : public htgs::ITask<Tile<uint32_t>,htgs::VoidData> {
public:

    void executeTask(std::shared_ptr<Tile<uint32_t>> data) override {


        //   std::cout << "write tile : " << data->get()->getRow() << "," << data->get()->getCol() << std::endl;
        // find out how to call the destructr    ~data();
        printf( "ref count for tile : %d %d %d :: %d \n", data->getRow() , data->getCol() , data->getLevel() , data.use_count());

        if(data->getLevel() == 0){
            data->getOrigin()->releaseMemory();
        }
    }

    ITask<Tile<uint32_t>, VoidData> *copy() override {
        return new WriteTileTask();
    }

};

#endif //PYRAMIDBUILDING_WRITETILETASK_H
