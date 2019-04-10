//
// Created by Gerardin, Antoine D. (Assoc) on 4/9/19.
//

#ifndef PYRAMIDBUILDING_TILEBUILDER_H
#define PYRAMIDBUILDING_TILEBUILDER_H

#include <htgs/api/ITask.hpp>
#include <pyramidBuilding/rules/ReleaseFOVMemoryRule.h>
#include <pyramidBuilding/utils/TiffImageLoader.h>
#include <pyramidBuilding/utils/StitchingVectorParser.h>
#include <pyramidBuilding/data/FOVWithData.h>
#include <pyramidBuilding/data/Tile.h>
#include <algorithm>
#include <pyramidBuilding/utils/Helper.h>


namespace pb {

    template<class T>
    class TileBuilder : public htgs::ITask<FOVWithData<T>, Tile<T> > {

    public:

        TileBuilder(FOVMetadata* metadata, uint32_t tileSize, std::map<std::pair<size_t,size_t>, u_int8_t> &fovUsageCount) :
        metadata(metadata), tileSize(tileSize), fovUsageCount(fovUsageCount) {
            fullWidth = metadata->getFullFovWidth();
            fullHeight = metadata->getFullFovHeight();
        }

        void executeTask(std::shared_ptr<FOVWithData<T>> data) override {

            auto fov = data->getFov();
            T* image = data->getData()->get();

            uint32_t fovWidth = fov->getMetadata()->getWidth();
            uint32_t fovHeight = fov->getMetadata()->getHeight();

            uint32_t fovTopLeftX = fov->getGlobalX();
            uint32_t fovTopLeftY = fov->getGlobalY();
            uint32_t fovBottomRightX = fovTopLeftX + fovWidth;
            uint32_t fovBottomRightY = fovTopLeftY + fovHeight;

            uint32_t colMin = fovTopLeftX / tileSize;
            uint32_t rowMin = fovTopLeftY / tileSize;
            uint32_t colMax = (fovBottomRightX - 1) / tileSize;
            uint32_t rowMax = (fovBottomRightY - 1) / tileSize;

            for (auto col = colMin; col <= colMax; col++) {
                for (auto row = rowMin; row <= rowMax; row++) {

                    Tile<T>* t = getTile(row,col);

                    uint32_t tileTopLeftX = col * tileSize;
                    uint32_t tileTopLeftY = row * tileSize;
                    uint32_t currentTileWidth = t->getWidth();
                    uint32_t currentTileHeight = t->getHeight();

                    uint32_t tileBottomRightX = tileTopLeftX + currentTileWidth;
                    uint32_t tileBottomRightY = tileTopLeftY + currentTileHeight;

                    uint32_t startX = std::max(fovTopLeftX, tileTopLeftX);
                    uint32_t startY = std::max(fovTopLeftY, tileTopLeftY);
                    uint32_t endX = std::min(fovBottomRightX, tileBottomRightX);
                    uint32_t endY = std::min(fovBottomRightY, tileBottomRightY);

                    uint32_t rangeY = endY - startY;
                    uint32_t rangeX = endX - startX;

                    T* tile = t->getData();

                    uint32_t srcOffset = (startY - fovTopLeftY) * fovWidth + (startX - fovTopLeftX);
                    uint32_t destOffset = (startY - tileTopLeftY) * tileSize + (startX - tileTopLeftX);


    //              printArray<T>("fov", image, fovWidth, fovHeight);
                    for(auto j = 0; j < rangeY; j++){
    //                  printArray<T>("tile", tile, t->getWidth(), t->getHeight());
                        std::copy_n(image + srcOffset + j * fovWidth, rangeX, tile + destOffset + j * currentTileWidth);
                    }

                    fovUsageCount[{row,col}]--;

                    if(fovUsageCount[{row,col}] == 0){
                        this->addResult(t);
                    }


                 //   auto tt = tileCache[{row,col}];
                //    printArray<T>("partial", tt->getData(), tt->getWidth(), tt->getHeight());
                }
            }

            data->getData()->releaseMemory();




            //if tile is complete, send to bookeeper.
        }

        htgs::ITask <FOVWithData<T>, Tile<T>> *copy() override {
            return nullptr;
        }

        Tile<T>* getTile(uint32_t row, uint32_t col){

            std::lock_guard<std::mutex> guard(lock);

            VLOG(3) << " building tile :  (" << row << "," << col << ")" << std::endl;

            Tile<T>* t = nullptr;
            std::pair<size_t,size_t> index= std::make_pair(row,col);
            auto it = tileCache.find(index);
            if(it == tileCache.end()){
                    auto width = std::min(tileSize, fullWidth - col * tileSize);
                    auto height = std::min(tileSize, fullHeight - row * tileSize);
                    t = new Tile<T>(0,row,col,width,height, new T[width * height]());
                    tileCache.insert({index,t});
            }
            else{
                t = it->second;
            }
            return t;
        }

    private:

        FOVMetadata* metadata;
        uint32_t tileSize;
        std::map<std::pair<size_t,size_t>, Tile<T>*> tileCache;
        std::map<std::pair<size_t,size_t>, u_int8_t> fovUsageCount;
        std::mutex lock;
        uint32_t fullWidth;
        uint32_t fullHeight;

    };

}

#endif //PYRAMIDBUILDING_TILEBUILDER_H
