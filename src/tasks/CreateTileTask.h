//
// Created by Gerardin, Antoine D. (Assoc) on 12/20/18.
//

#ifndef PYRAMIDBUILDING_CREATETILETASK_H
#define PYRAMIDBUILDING_CREATETILETASK_H

#include <htgs/api/ITask.hpp>
#include <FastImage/api/FastImage.h>
#include "../data/BlockRequest.h"
#include "../data/FakeTile.h"
#include "../rules/MatrixMemoryRule.h"

#include <opencv/cv.h>
#include <opencv2/imgproc.hpp>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

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

        uint32_t pyramidTileSize = 32;
        uint32_t* newTileData = new uint32_t[ 2 * pyramidTileSize * 2 * pyramidTileSize ];  //the pyramid tile we will be filling from partial FOVs.

        //TODO CHECK. MIGHT NOT BE NECESSARY. Set all values to 0
        memset( newTileData, 0, 4 * pyramidTileSize * pyramidTileSize*sizeof(uint32_t) );

        switch (block.size()){
            //regular block
            case 4:
                generateDownsampledTile(newTileData, 0,0, block[0]->getData(),pyramidTileSize);
                generateDownsampledTile(newTileData, 0,1, block[1]->getData(),pyramidTileSize);
                generateDownsampledTile(newTileData, 1,0, block[2]->getData(),pyramidTileSize);
                generateDownsampledTile(newTileData, 1,1, block[3]->getData(),pyramidTileSize);
                break;
            //bottom right single block
            case 3:
                generateDownsampledTile(newTileData, 0,0, block[0]->getData(),pyramidTileSize);
                generateDownsampledTile(newTileData, 1,0, block[2]->getData(),pyramidTileSize);
                break;
                //bottom horizontal block
            case 2:
                generateDownsampledTile(newTileData, 0,0, block[0]->getData(),pyramidTileSize);
                generateDownsampledTile(newTileData, 0,1, block[1]->getData(),pyramidTileSize);
                break;
                //right vertical block
            case 1:
                generateDownsampledTile(newTileData, 0,0, block[0]->getData(),pyramidTileSize);
                break;
        }

        cv::Mat image(2 * pyramidTileSize, 2 * pyramidTileSize, CV_32SC1, newTileData);
        cv::imwrite("createTileTask.png", image);

        uint32_t* downsampleData = new uint32_t[ pyramidTileSize * pyramidTileSize ];

        for(uint32_t j= 0 ; j < pyramidTileSize ; j++) {
            for(uint32_t i= 0 ; i < pyramidTileSize ; i++){
                auto index = j * pyramidTileSize + i;
                downsampleData[index] = (newTileData[2 * j * 2 * pyramidTileSize + 2 * i] + newTileData[2 * j * 2 * pyramidTileSize + 2 *i + 1] +
                        newTileData[2 * (j+1) * 2 * pyramidTileSize + 2 * i] + newTileData[2 * (j+1) * 2 * pyramidTileSize + 2 *i + 1] ) / 4;
            }
        }

        cv::Mat image2(pyramidTileSize, pyramidTileSize, CV_32SC1, downsampleData);
        cv::imwrite("downsampleCreateTileTask.png", image2);


        auto b = block[0];
        tile = new Tile<uint32_t>(row,col,b->getLevel()+1,downsampleData);
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

        for (int j = 0; j < pyramidTileSize; j ++) {
            for (int i = 0; i < pyramidTileSize; i ++) {
                uint32_t indexRowOffset = row * ( 2 * pyramidTileSize * pyramidTileSize);
                uint32_t indexColOffset = col * pyramidTileSize;

                uint32_t index = indexRowOffset + (j * 2 * pyramidTileSize)  + indexColOffset + i;

                std::cout <<  "t : " << std::to_string(index) << std::endl;

                data[index] = d[j * pyramidTileSize + i];

            }
        }
    }

};


#endif //PYRAMIDBUILDING_CREATETILETASK_H
