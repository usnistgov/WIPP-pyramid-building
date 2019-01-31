//
// Created by Gerardin, Antoine D. (Assoc) on 12/20/18.
//

#ifndef PYRAMIDBUILDING_CREATETILETASK_H
#define PYRAMIDBUILDING_CREATETILETASK_H

#include <htgs/api/ITask.hpp>
#include <FastImage/api/FastImage.h>
#include "../data/BlockRequest.h"
#include <opencv/cv.h>
#include <opencv2/imgproc.hpp>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>


template <class T>
class CreateTileTask : public htgs::ITask<BlockRequest<T>, Tile<T> > {

public:

    explicit CreateTileTask(size_t numThreads) : ITask<BlockRequest<T>, Tile<T>>(numThreads) {}

    void print(std::string title, T* data, size_t w, size_t h){
        std::cout << title << std::endl;
        for (size_t i = 0; i < h; ++i){
            for(size_t j = 0; j < w; ++j){
                std::cout << std::setw(3) << (int)(data[i * w + j]) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    void executeTask(std::shared_ptr<BlockRequest<T>> data) override {
        auto block = data->getBlock();

        std::cout << "Create Tile Task - " << *data.get() << std::endl;

        size_t level = block[0]->getLevel() + 1;
        size_t row = floor(block[0]->getRow() / 2);
        size_t col = floor(block[0]->getCol() /2);

        Tile<T> *tile = nullptr;
        T* newTileData = nullptr;
        T* downsampleData = nullptr;

        size_t width = 0, height = 0;

        switch (block.size()){
            //regular block
            case 4:
                width = block[0]->get_width() + block[1]->get_width();
                height = block[0]->get_height() + block[2]->get_height();

                newTileData = new T[ width * height ]();
                copyTileBlock(newTileData, block[0].get(), width, height, 0, 0);
                print("NW", newTileData, width, height);
                copyTileBlock(newTileData, block[1].get(), width, height, block[0]->get_width(), 0);
                print("NE", newTileData, width, height);
                copyTileBlock(newTileData, block[2].get(), width, height, 0, block[0]->get_height());
                print("SW", newTileData, width, height);
                copyTileBlock(newTileData, block[3].get(), width, height, block[0]->get_width(), block[0]->get_height());
                print("SE", newTileData, width, height);
                downsampleData = generateDownsampleData(newTileData, width, height);
                print("DS", downsampleData, width/2, height/2);
                break;
            //right vertical block
            case 3:
                width = block[0]->get_width();
                height = block[0]->get_height() + block[2]->get_height();

                newTileData = new T[ width * height ]();
                copyTileBlock(newTileData, block[0].get(), width, height, 0, 0);
                copyTileBlock(newTileData, block[2].get(), width, height, 0, block[0]->get_height());
                downsampleData = generateDownsampleData(newTileData, width, height);
                break;
            //bottom horizontal block
            case 2:
                width = block[0]->get_width() + block[1]->get_width();
                height = block[0]->get_height();

                newTileData = new T[ width * height ]();
                copyTileBlock(newTileData, block[0].get(), width, height, 0, 0);
                copyTileBlock(newTileData, block[1].get(), width, height, block[0]->get_width(), 0);
                downsampleData = generateDownsampleData(newTileData, width, height);
                break;
            //bottom right single block
            case 1:

                width = block[0]->get_width();
                height = block[0]->get_height();

                newTileData = new T[ width * height ]();
                copyTileBlock(newTileData, block[0].get(), width, height, 0, 0);
                downsampleData = generateDownsampleData(newTileData, width, height);
                break;
            default:
                std::cerr << "block was malformed. Size : " << block.size() << std::endl;
                exit(1);
        }

        //TODO check this kind of conversion throughout
        auto downsampleWidth = ceil( (size_t)width / 2);
        auto downsampleHeight = ceil( (size_t)height / 2);

        std::vector<std::shared_ptr<Tile<T>>> & origin = data->getBlock();
        tile = new Tile<T>(level, row, col, downsampleWidth, downsampleHeight, downsampleData, origin);


//        DEBUG
//        cv::Mat image(height, width, CV_32SC1, newTileData);
//        cv::imwrite("createTileTaskLargeFOV" + std::to_string(counter) + "orig.png", image);
//        cv::Mat tmp(height, width, CV_16U);
//        image.convertTo(tmp, CV_16U);
//        cv::imwrite("createTileTaskLargeFOV" + std::to_string(counter) + ".png", tmp);

        delete[] newTileData;


        this->addResult(tile);

     //   counter++;
    }

    std::string getName() override {
        return "Pyramid Tile Task";
    }

    htgs::ITask<BlockRequest<T>, Tile<T> > *copy() override {
        return new CreateTileTask(this->getNumThreads());
    }

private:
    void copyTileBlock(T *data, Tile<T>* block, size_t fullWidth, size_t fullHeight, size_t colOffset, size_t rowOffset) {

        for (size_t j = 0; j < block->get_height(); j ++) {
            for (size_t i = 0; i < block->get_width(); i ++) {
//                uint32_t indexRowOffset = row * ( nbCols * width * height);
//                uint32_t indexColOffset = (nbCols == 2) ? col * width : 0;
//
//                uint32_t index = indexRowOffset + (j * nbCols * width)  + indexColOffset + i;

                size_t index = fullWidth * ( j + rowOffset) + colOffset + i;

    //            std::cout <<  "t : " << std::to_string(index) << std::endl;

                data[index] = block->getData()[j * block->get_width() + i];

            }
        }
    }

    T* generateDownsampleData(T* newTileData, size_t width, size_t height) {

        //TODO check this kind of conversion throughout
        //in particular : when we have size_t as inputs, how do we ensure there is no overflow
        auto downsampleWidth = static_cast<size_t>(ceil( (size_t)width / 2));
        auto downsampleHeight = static_cast<size_t>(ceil( (size_t)height / 2));

        T* downsampleData = new T[ downsampleWidth * downsampleHeight ]();

        for(size_t j= 0 ; j < downsampleHeight - 1; j++) {
            for(size_t i= 0 ; i < downsampleWidth - 1; i++){
                size_t index = j * downsampleWidth + i;
                downsampleData[index] = (newTileData[2 * j * width + 2 * i] + newTileData[2 * j * width + 2 *i + 1] +
                                         newTileData[2 * (j+1) * width + 2 * i] + newTileData[2 * (j+1) * width + 2 *i + 1] ) / 4;
            }
        }

        for(size_t i= 0 ; i < downsampleWidth - 1; i++) {
            size_t index = (downsampleHeight - 1) * downsampleWidth + i;
            downsampleData[index] = (newTileData[(height - 1) * width + 2 * i] + newTileData[(height -1) * width + 2 * i + 1]) / 2;
        }



        //TODO REMOVE FOR DEBUG
//        cv::Mat  mat = cv::Mat(downsampleHeight, downsampleWidth, CV_32SC1 , downsampleData);
//        cv::imwrite("createTileTaskDownsample" + std::to_string(counter) + "orig.png", mat);
//        cv::Mat tmp(downsampleHeight, downsampleWidth, CV_16U);
//        mat.convertTo(tmp, CV_16U, 1,0);
//        cv::imwrite("createTileTaskDownsample" + std::to_string(counter) + ".png", tmp);


        return downsampleData;

    }


//    uint32_t counter = 0;

};


#endif //PYRAMIDBUILDING_CREATETILETASK_H
