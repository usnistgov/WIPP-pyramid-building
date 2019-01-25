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

        uint32_t level = block[0]->getLevel() + 1;
        uint32_t row = floor(block[0]->getRow() / 2);
        uint32_t col = floor(block[0]->getCol() /2);

        Tile<uint32_t> *tile = nullptr;
        htgs::m_data_t<fi::View<uint32_t>> t ;

        uint32_t* newTileData = nullptr;
        uint32_t* downsampleData = nullptr;

        uint32_t width,height;


        switch (block.size()){
            //regular block
            case 4:

                width = block[0]->get_width() + block[1]->get_width();
                height = block[0]->get_height() + block[2]->get_height();

                newTileData = new uint32_t[ width * height ];
                memset( newTileData, 0, width * height * sizeof(uint32_t) );
                copyTileBlock(newTileData, 0, 0, block[0]->getData(), width, height, 2, 2);
                copyTileBlock(newTileData, 0, 1, block[1]->getData(), width, height, 2, 2);
                copyTileBlock(newTileData, 1, 0, block[2]->getData(), width, height, 2, 2);
                copyTileBlock(newTileData, 1, 1, block[3]->getData(), width, height, 2, 2);
                downsampleData = generateDownsampleData(newTileData, width, height);
                break;
            //right vertical block
            case 3:
                width = block[0]->get_width();
                height = block[0]->get_height() + block[2]->get_height();

                newTileData = new uint32_t[ width * height ];
                memset( newTileData, 0, width * height * sizeof(uint32_t) );
                copyTileBlock(newTileData, 0, 0, block[0]->getData(), width, height, 2, 1);
                copyTileBlock(newTileData, 1, 0, block[2]->getData(), width, height, 2, 1);
                downsampleData = generateDownsampleData(newTileData, width, height);
                break;
            //bottom horizontal block
            case 2:

                width = block[0]->get_width() + block[1]->get_width();
                height = block[0]->get_height();

                newTileData = new uint32_t[ width * height ];
                memset( newTileData, 0, width * height * sizeof(uint32_t) );
                copyTileBlock(newTileData, 0, 0, block[0]->getData(), width, height, 1, 2);
                copyTileBlock(newTileData, 0, 1, block[1]->getData(), width, height, 1, 2);
                downsampleData = generateDownsampleData(newTileData, width, height);
                break;
            //bottom right single block
            case 1:

                width = block[0]->get_width();
                height = block[0]->get_height();

                newTileData = new uint32_t[ width * height ];
                memset( newTileData, 0, width * height * sizeof(uint32_t) );
                copyTileBlock(newTileData, 0, 0, block[0]->getData(), width, height, 1, 1);
                downsampleData = generateDownsampleData(newTileData, width, height);
                break;
        }

        //TODO check this kind of conversion throughout
        auto downsampleWidth = ceil( (double)width / 2);
        auto downsampleHeight = ceil( (double)height / 2);

        tile = new Tile<uint32_t>(level, row, col, downsampleWidth, downsampleHeight, downsampleData);

        //TODO REMOVE FOR DEBUG
//        cv::Mat image(2 * pyramidTileSize, 2 * pyramidTileSize, CV_32SC1, newTileData);
//        cv::imwrite("createTileTask.png", image);


        this->addResult(tile);
    }

    std::string getName() override {
        return "Pyramid Tile Task";
    }

    htgs::ITask<BlockRequest<uint32_t>, Tile<uint32_t> > *copy() override {
        return new CreateTileTask();
    }

private:
    void copyTileBlock(uint32_t *data, uint32_t row, uint32_t col, uint32_t *d, uint32_t width, uint32_t height,
                       uint32_t nbRows, uint32_t nbCols) {

        for (int j = 0; j < height; j ++) {
            for (int i = 0; i < width; i ++) {
                uint32_t indexRowOffset = row * ( nbCols * width * height);
                uint32_t indexColOffset = (nbCols == 2) ? col * width : 0;

                uint32_t index = indexRowOffset + (j * nbCols * width)  + indexColOffset + i;

                std::cout <<  "t : " << std::to_string(index) << std::endl;

                data[index] = d[j * width + i];

            }
        }
    }

    uint32_t* generateDownsampleData(uint32_t *newTileData, uint32_t width, uint32_t height) {

        //TODO check this kind of conversion throughout
        auto downsampleWidth = ceil( (double)width / 2);
        auto downsampleHeight = ceil( (double)height / 2);

        uint32_t* downsampleData = new uint32_t[ width * height ];

        for(auto j= 0 ; j < downsampleHeight; j++) {
            for(auto i= 0 ; i < downsampleWidth; i++){
                auto index = j * width + i;
                downsampleData[index] = (newTileData[2 * j * 2 * width + 2 * i] + newTileData[2 * j * 2 * width + 2 *i + 1] +
                                         newTileData[2 * (j+1) * 2 * width + 2 * i] + newTileData[2 * (j+1) * 2 * width + 2 *i + 1] ) / 4;
            }
        }

//        cv::Mat image2(pyramidTileSize, pyramidTileSize, CV_32SC1, downsampleData);
//        cv::imwrite("downsampleCreateTileTask.png", image2);

        return downsampleData;

    }

};


#endif //PYRAMIDBUILDING_CREATETILETASK_H
