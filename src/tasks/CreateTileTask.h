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
        uint32_t *tileData = nullptr;

        htgs::m_data_t<fi::View<uint32_t>> t ;

        uint32_t pyramidTileSize = 32;

        switch (block.size()) {
            //bottom right single block
            case 1: {
                //downsample and send new tile

//                tileData = new uint32_t[pyramidTileSize /
//                                              2];  //the pyramid tile we will be filling from partial FOVs.
//                memset(tileData, 0, pyramidTileSize * sizeof(uint32_t) / 2);

                break;
            }
            //bottom horizontal block
            case 2: {
                //blend resize and downsample and send new tile
                level = block[0]->getLevel();

//                tileData = new uint32_t[3 * pyramidTileSize /
//                                               2];  //the pyramid tile we will be filling from partial FOVs.
//                memset(tileData, 0, 3 * pyramidTileSize * sizeof(uint32_t) / 2);
//
//                uint32_t r = block[0]->getRow();
//                uint32_t c = block[0]->getCol();
//
//                // coordinates are reversed -> Size(col,row)
//                cv::Mat small_image(c, r, CV_32SC1, &tileData);
//                //TODO CHECK 32 signed?
//                cv::Mat big_image(cv::Size(pyramidTileSize, pyramidTileSize/2), CV_32SC1);
//                small_image.copyTo(big_image(cv::Rect(0,0,small_image.cols, small_image.rows)));


           //     generateDownsampledTile(tileData, 0, 0, block[0]->getData(), pyramidTileSize);
//                generateDownsampledTile(tileData, 0, 1, block[1]->getData(), pyramidTileSize);

                break;
            }
            //right vertical block
            case 3: {
                //blend resize and downsample and send new tile
                level = block[0]->getLevel();


//                tileData = new uint32_t[3 * pyramidTileSize /
//                                               2];  //the pyramid tile we will be filling from partial FOVs.
//                memset(tileData, 0, 3 * pyramidTileSize * sizeof(uint32_t) / 2);
//
//                generateDownsampledTile(tileData, 0, 0, block[0]->getData(), pyramidTileSize);
//                generateDownsampledTile(tileData, 1, 0, block[1]->getData(), pyramidTileSize);

                break;
            }
            //regular block
            case 4: {
                //blend resize and downsample and send new tile
                level = block[0]->getLevel();

//                tileData = new uint32_t[2 *
//                                               pyramidTileSize];  //the pyramid tile we will be filling from partial FOVs.
//                memset(tileData, 0, 2 * pyramidTileSize * sizeof(uint32_t));

//                generateDownsampledTile(tileData, 0, 0, block[0]->getData(), pyramidTileSize);
//                generateDownsampledTile(tileData, 0, 1, block[1]->getData(), pyramidTileSize);
//                generateDownsampledTile(tileData, 1, 0, block[2]->getData(), pyramidTileSize);
//                generateDownsampledTile(tileData, 1, 1, block[3]->getData(), pyramidTileSize);

                uint32_t r = block[0]->getRow();
                uint32_t c = block[0]->getCol();

                cv::Mat small_image0_8(32, 32, CV_8UC1);
                cv::Mat small_image1_8(32, 32, CV_8UC1);
                cv::Mat small_image2_8(32, 32, CV_8UC1);
                cv::Mat small_image3_8(32, 32, CV_8UC1);


                cv::Mat small_image0(32, 32, CV_32SC1, block[0]->getData());
                cv::Mat small_image1(32, 32, CV_32SC1, block[1]->getData());
                cv::Mat small_image2(32, 32, CV_32SC1, block[2]->getData());
                cv::Mat small_image3(32, 32, CV_32SC1, block[3]->getData());

                small_image0.convertTo(small_image0_8, CV_8UC1);
                small_image1.convertTo(small_image0_8, CV_8UC1);
                small_image2.convertTo(small_image0_8, CV_8UC1);
                small_image3.convertTo(small_image0_8, CV_8UC1);

                cv::imwrite( "small_image0_8", small_image0_8 );


//                std::cout << "M = "<< std::endl << " "  << small_image0 << std::endl << std::endl;
//                std::cout << "M = "<< std::endl << " "  << small_image1 << std::endl << std::endl;
//                std::cout << "M = "<< std::endl << " "  << small_image2 << std::endl << std::endl;
//                std::cout << "M = "<< std::endl << " "  << small_image3 << std::endl << std::endl;


                //TODO CHECK 32 signed?
                cv::Mat big_image(128,128, CV_8UC1);
                small_image0_8.copyTo(big_image(cv::Rect(0,0,small_image0_8.cols, small_image0_8.rows)));
                small_image1_8.copyTo(big_image(cv::Rect(31,0,small_image1_8.cols, small_image1_8.rows)));
                small_image2_8.copyTo(big_image(cv::Rect(0,31,small_image2_8.cols, small_image2_8.rows)));
                small_image3_8.copyTo(big_image(cv::Rect(31,31,small_image3_8.cols, small_image3_8.rows)));

                cv::Mat tmp(32, 32, CV_8UC1);
                cv::pyrDown( tmp, big_image, cv::Size( big_image.cols/2, big_image.rows/2 ));

                cv::Mat tmp2(32, 32, CV_32SC1);
                tmp.convertTo(tmp2, CV_32SC1);

                std::cout << "final  = "<< std::endl << " "  << big_image << std::endl << std::endl;

                tile = new Tile<uint32_t>(row, col, level + 1, (uint32_t*)tmp2.data);
//                std::cout << "FINAL = "<< std::endl << " "  << big_image << std::endl << std::endl;
                break;
            }
        }



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
