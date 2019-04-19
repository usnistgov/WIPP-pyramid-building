//
// Created by Gerardin, Antoine D. (Assoc) on 12/20/18.
//

#ifndef PYRAMIDBUILDING_TILEDOWNSAMPLER_H
#define PYRAMIDBUILDING_TILEDOWNSAMPLER_H

#include <htgs/api/ITask.hpp>
#include <FastImage/api/FastImage.h>
#include "pyramidBuilding/data/TileBlock.h"
#include <opencv/cv.h>
#include <opencv2/imgproc.hpp>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include "../utils/Downsampler.h"
#include "pyramidBuilding/utils/Utils.h"

namespace pb {

    /**
     * Take a block of tiles and generate a downsampled tile with a factor of 2.
     * @details Current Implementation copy individual tiles in a bigger tile and then downsample this tile.
     * Performance could be improved by downsampling tiles in place. In this however more involved since we have to
     * identify each case and select source tiles depending if we are inside a tile or at the border.
     * The benefits is not expected to be in term of runtime but in term of memory since less data is kept around.
     * @tparam T Image pixel representation.
     */

template <class T>
class TileDownsampler : public htgs::ITask<TileBlock<T>, Tile<T> > {

private:
    Downsampler<T> *downsampler;

public:

    TileDownsampler(size_t numThreads, Downsampler<T> *downsampler) : ITask<TileBlock<T>, Tile<T>>(numThreads), downsampler(downsampler) {}

    void executeTask(std::shared_ptr<TileBlock<T>> data) override {

        auto block = data->getBlock();

        VLOG(2) << "Create Tile Task - " << *data.get() << std::endl;

        size_t level = block[0]->getLevel() + 1;
        size_t row = block[0]->getRow() / 2;
        size_t col = block[0]->getCol() / 2;

        Tile<T> *tile = nullptr;
        T* newTileData = nullptr;
        T* downsampleData = nullptr;

        size_t width = 0, height = 0;

        switch (data->getType()){
            //regular block
            case BlockType::Full:
                width = block[0]->getWidth() + block[1]->getWidth();
                height = block[0]->getHeight() + block[2]->getHeight();

                newTileData = new T[ width * height ]();
                copyTileBlock(newTileData, block[0].get(), width, height, 0, 0);
                copyTileBlock(newTileData, block[1].get(), width, height, block[0]->getWidth(), 0);
                copyTileBlock(newTileData, block[2].get(), width, height, 0, block[0]->getHeight());
                copyTileBlock(newTileData, block[3].get(), width, height, block[0]->getWidth(), block[0]->getHeight());

                downsampleData = this->downsampler->downsample(newTileData, width, height);
                break;
            //right vertical block
            case BlockType::Vertical:
                width = block[0]->getWidth();
                height = block[0]->getHeight() + block[2]->getHeight();

                newTileData = new T[ width * height ]();
                copyTileBlock(newTileData, block[0].get(), width, height, 0, 0);
                copyTileBlock(newTileData, block[2].get(), width, height, 0, block[0]->getHeight());

                downsampleData = this->downsampler->downsample(newTileData, width, height);
                break;
            //bottom horizontal block
            case BlockType::Horizontal:
                width = block[0]->getWidth() + block[1]->getWidth();
                height = block[0]->getHeight();

                newTileData = new T[ width * height ]();
                copyTileBlock(newTileData, block[0].get(), width, height, 0, 0);
                copyTileBlock(newTileData, block[1].get(), width, height, block[0]->getWidth(), 0);

                downsampleData = this->downsampler->downsample(newTileData, width, height);
                break;
            //bottom right single block
            case BlockType::Single:
                width = block[0]->getWidth();
                height = block[0]->getHeight();

                newTileData = new T[ width * height ]();
                copyTileBlock(newTileData, block[0].get(), width, height, 0, 0);

                downsampleData = this->downsampler->downsample(newTileData, width, height);
                break;
                default:
                    throw std::runtime_error("block was malformed. Size : " + std::to_string(block.size()) );
        }

        auto downsampleWidth = ceil( (double)width / 2);
        auto downsampleHeight = ceil( (double)height / 2);

        std::vector<std::shared_ptr<Tile<T>>> & origin = data->getBlock();
        tile = new Tile<T>(level, row, col, downsampleWidth, downsampleHeight, downsampleData, origin);

        delete[] newTileData;

        assert(level > 0);

        this->addResult(tile);
    }

    std::string getName() override {
        return "Create Tile Task";
    }

    htgs::ITask<TileBlock<T>, Tile<T> > *copy() override {
        return new TileDownsampler(this->getNumThreads(), this->downsampler);
    }

private:
    void copyTileBlock(T *data, Tile<T>* block, size_t fullWidth, size_t fullHeight, size_t colOffset, size_t rowOffset) {

        //Faster implementation of copy. But does not change computation time because of the overlap with slow IO.
        for(auto j = 0; j < block->getHeight(); j++){
            std::copy_n(block->getData() + j * block->getWidth(), block->getWidth(), data + colOffset + (j + rowOffset) * fullWidth);
        }
        //SLOWER IMPLEMENTATION
        //        for (size_t j = 0; j < block->getHeight(); j ++) {
        //            for (size_t i = 0; i < block->getWidth(); i ++) {
        //                size_t index = fullWidth * ( j + rowOffset) + colOffset + i;
        //                data[index] = block->getData()[j * block->getWidth() + i];
        //            }
        //        }
    }

};

}


#endif //PYRAMIDBUILDING_TILEDOWNSAMPLER_H