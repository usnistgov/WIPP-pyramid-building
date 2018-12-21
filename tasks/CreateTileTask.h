//
// Created by Gerardin, Antoine D. (Assoc) on 12/20/18.
//

#ifndef PYRAMIDBUILDING_CREATETILETASK_H
#define PYRAMIDBUILDING_CREATETILETASK_H

#include <htgs/api/ITask.hpp>
#include "FastImage/api/FastImage.h"

class CreateTileTask : public htgs::ITask<htgs::MemoryData<fi::View<uint32_t>>,htgs::VoidData> {



public:
    CreateTileTask() {}

    void executeTask(std::shared_ptr<MemoryData<fi::View<uint32_t>>> data) override {
        ++count;
        std::cout << "create tile : count : " << count << std::endl;
    }

    ITask<MemoryData<fi::View<uint32_t>>, VoidData> *copy() override {
        return new CreateTileTask();
    }


private :
    std::atomic<int> count = {0} ;
};


#endif //PYRAMIDBUILDING_CREATETILETASK_H
