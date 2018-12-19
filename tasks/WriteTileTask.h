//
// Created by Gerardin, Antoine D. (Assoc) on 12/19/18.
//

#ifndef PYRAMIDBUILDING_WRITETILETASK_H
#define PYRAMIDBUILDING_WRITETILETASK_H

#include <htgs/api/ITask.hpp>
#include "FastImage/api/FastImage.h"

class WriteTileTask : public htgs::ITask<fi::View<uint32_t>,htgs::VoidData> {
public:
    void executeTask(std::shared_ptr<fi::View<uint32_t>> data) override {

        std::cout << "write tile : " << data->getRow() << "," << data->getCol() << std::endl;

    }

    ITask<fi::View<uint32_t>, VoidData> *copy() override {
        return new WriteTileTask();
    }


};

#endif //PYRAMIDBUILDING_WRITETILETASK_H
