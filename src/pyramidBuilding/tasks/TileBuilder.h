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
#include <pyramidBuilding/utils/TileCache.h>


namespace pb {

    template<class T>
    class TileBuilder : public htgs::ITask<FOVWithData<T>, Tile<T> > {

    public:

        TileBuilder(
                size_t numThreads,
                FOVMetadata* metadata,
                uint32_t tileSize,
                TileCache<T>* tileCache)
                : htgs::ITask<FOVWithData<T>, Tile<T> >(numThreads),
        metadata(metadata), tileSize(tileSize), tileCache(tileCache) {}

        void executeTask(std::shared_ptr<FOVWithData<T>> data) override {

            auto fov = data->getFov();
            T* image = data->getData()->get();

            VLOG(3) << " number of tiled cached in TileBuilder when receiving FOV (" << fov->getRow() << "," << fov->getCol() << ") : " << tileCache->size() << std::endl;

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

                    Tile<T>* t = tileCache->getTile(row,col);

                    VLOG(3) << " filling tile ( "<< row << "," << col << ") with FOV (" << fov->getRow() << "," << fov->getCol() << ")"  << std::endl;

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
                    uint32_t destOffset = (startY - tileTopLeftY) * currentTileWidth + (startX - tileTopLeftX);


                    assert(currentTileWidth <= tileSize);
                    assert(currentTileHeight <= tileSize);
                    assert(rangeY <= currentTileHeight);
                    assert(rangeX <= currentTileWidth);

    //              printArray<T>("fov", image, fovWidth, fovHeight);
                    for(auto j = 0; j < rangeY; j++){
    //                  printArray<T>("tile", tile, t->getWidth(), t->getHeight());
                        std::copy_n(image + srcOffset + j * fovWidth, rangeX, tile + destOffset + j * currentTileWidth);
                    }

                    tileCache->doneCopyingFOVintoTile(row,col);

                    if(tileCache->tileReady(row,col)){
                        VLOG(3) << "tile (" << row << "," << col << ") ready. " << std::endl;
                        this->addResult(t);
                        tileCache->deleteTile(row,col);
                    }


                 //   auto tt = tileCache[{row,col}];
                //    printArray<T>("partial", tt->getData(), tt->getWidth(), tt->getHeight());
                }
            }

            data->getData()->releaseMemory();




            //if tile is complete, send to bookeeper.
        }

        htgs::ITask <FOVWithData<T>, Tile<T>> *copy() override {
            return new TileBuilder(this->getNumThreads(), metadata, tileSize, tileCache);
        }



    std::string getName() override { return "Empty Tile Rule"; }

    private:

        FOVMetadata* metadata;
        TileCache<T>* tileCache;
        uint32_t tileSize;

    };

}

#endif //PYRAMIDBUILDING_TILEBUILDER_H
