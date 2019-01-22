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
        uint32_t *tileData = nullptr;

        htgs::m_data_t<fi::View<uint32_t>> t ;

        uint32_t pyramidTileSize = 32;

        switch (block.size()) {
            //bottom right single block
            case 1: {
                //downsample and send new tile

                tileData = new uint32_t[pyramidTileSize /
                                              2];  //the pyramid tile we will be filling from partial FOVs.
                memset(tileData, 0, pyramidTileSize * sizeof(uint32_t) / 2);

                break;
            }
            //bottom horizontal block
            case 2: {
                //blend resize and downsample and send new tile
                level = block[0]->getLevel();

                tileData = new uint32_t[3 * pyramidTileSize /
                                               2];  //the pyramid tile we will be filling from partial FOVs.
                memset(tileData, 0, 3 * pyramidTileSize * sizeof(uint32_t) / 2);

                generateDownsampledTile(tileData, 0, 0, block[0]->getData(), pyramidTileSize);
                generateDownsampledTile(tileData, 0, 1, block[1]->getData(), pyramidTileSize);

                break;
            }
            //right vertical block
            case 3: {
                //blend resize and downsample and send new tile
                level = block[0]->getLevel();


                tileData = new uint32_t[3 * pyramidTileSize /
                                               2];  //the pyramid tile we will be filling from partial FOVs.
                memset(tileData, 0, 3 * pyramidTileSize * sizeof(uint32_t) / 2);

                generateDownsampledTile(tileData, 0, 0, block[0]->getData(), pyramidTileSize);
                generateDownsampledTile(tileData, 1, 0, block[1]->getData(), pyramidTileSize);

                break;
            }
            //regular block
            case 4: {
                //blend resize and downsample and send new tile
                level = block[0]->getLevel();

                tileData = new uint32_t[2 *
                                               pyramidTileSize];  //the pyramid tile we will be filling from partial FOVs.
                memset(tileData, 0, 2 * pyramidTileSize * sizeof(uint32_t));

                generateDownsampledTile(tileData, 0, 0, block[0]->getData(), pyramidTileSize);
                generateDownsampledTile(tileData, 0, 1, block[1]->getData(), pyramidTileSize);
                generateDownsampledTile(tileData, 1, 0, block[2]->getData(), pyramidTileSize);
                generateDownsampledTile(tileData, 1, 1, block[3]->getData(), pyramidTileSize);

                break;
            }
        }

        tile = new Tile<uint32_t>(row, col, level + 1, tileData);

        this->addResult(tile);
    }

    std::string getName() override {
        return "Pyramid Tile Task";
    }

    htgs::ITask<BlockRequest<uint32_t>, Tile<uint32_t> > *copy() override {
        return new CreateTileTask();
    }


private:
    void generateDownsampledTile(uint32_t* data, uint32_t row, uint32_t col, uint32_t* d, uint32_t pyramidTileSize) {
        for (int j = 0; j < pyramidTileSize; j += 2) {
            for (int i = 0; i < pyramidTileSize; i += 2) {
                uint32_t index = ((row +j) * pyramidTileSize + (col * pyramidTileSize + i)) / 2;

                std::cout <<  "t : " << std::to_string(index) << std::endl;

                data[index] = (d[j * pyramidTileSize + i] + d[j * pyramidTileSize + i + 1] +
                                                d[(j + 1) * pyramidTileSize + i] + d[(j + 1) * pyramidTileSize + i + 1]) / 4;
            }
        }
    }

};


#endif //PYRAMIDBUILDING_CREATETILETASK_H
