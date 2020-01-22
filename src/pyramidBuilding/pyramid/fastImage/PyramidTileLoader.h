//
// Created by gerardin on 4/19/19.
//

#ifndef PYRAMIDBUILDING_PYRAMIDTILELOADER_H
#define PYRAMIDBUILDING_PYRAMIDTILELOADER_H

#include <FastImage/api/FastImage.h>
#include <FastImage/api/ATileLoader.h>
#include <pyramidBuilding/pyramid/TiffImageLoader.h>
#include <pyramidBuilding/pyramid/PyramidBuilder.h>
#include <pyramidBuilding/pyramid/data/PartialFOV.h>
#include <pyramidBuilding/pyramid/data/TileRequest.h>

namespace pb {

    template<class T>
    class PyramidTileLoader : public fi::ATileLoader<T> {

    public:

        PyramidTileLoader(size_t numThreads, std::shared_ptr<PyramidBuilder> &pyramidBuilder, TiffImageLoader<T>* imageLoader, size_t pyramidTileSize) : fi::ATileLoader<T>("", numThreads), _pyramidBuilder(pyramidBuilder), _imageLoader(imageLoader), _pyramidTileSize(pyramidTileSize) {

        }

        //TODO CHECK. Problem might occur when duplicating this loader.
        fi::ATileLoader<T> *copyTileLoader() override {
            return new PyramidTileLoader(this->getNumThreads(), this->_pyramidBuilder, this->_imageLoader, this->_pyramidTileSize);
        }

        uint32_t getImageHeight(uint32_t level) const override {
            return _pyramidBuilder->getFullFovHeight();
        }

        uint32_t getImageWidth(uint32_t level) const override {
            return _pyramidBuilder->getFullFovWidth();
        }

        uint32_t getTileWidth(uint32_t level) const override {
            return (uint32_t)_pyramidTileSize;
        }

        uint32_t getTileHeight(uint32_t level) const override {
            return (uint32_t)_pyramidTileSize;
        }

        short getBitsPerSample() const override {
            return _pyramidBuilder->getFovMetadata()->getBitsPerSample();
        }

        uint32_t getNbPyramidLevels() const override {
            return 1;
//            size_t fullFovWidth = _pyramidBuilder->getFullFovWidth();
//            size_t fullFovHeight = _pyramidBuilder->getFullFovHeight();
//            uint32_t deepZoomLevel = 0;
//            //calculate pyramid depth
//            auto maxDim = std::max(fullFovWidth,fullFovHeight);
//            deepZoomLevel = uint32_t(ceil(log2(maxDim)) + 1);
//            return deepZoomLevel;
        }

        std::string getName() override {
            return "PyramidTileLoader";
        }

        double loadTileFromFile(T *tile, uint32_t row, uint32_t col) override {

            //TODO is there a more efficient way to reinit the tile?
            for(uint32_t tileRow = 0; tileRow < getTileHeight(0); tileRow++){
                for(uint32_t tileCol = 0; tileCol < getTileWidth(0); tileCol++) {
                    tile[tileRow * getTileWidth(0) + tileCol ] = 0;
                }
            }

            auto builder = _pyramidBuilder->getTileRequests();
            auto tileRequest = builder[{row,col}];

            //if there is a hole without any info we generate an empty tile
            if(tileRequest == nullptr){
                return 0;
            }

//            auto tileRequest = _pyramidBuilder->getTileRequests().at({row,col});

            for(PartialFOV* fov : tileRequest->getFovs()){
                _imageLoader->loadPartialImageIntoTile(tile,row,col,fov);
//                cv::Mat image(this->_pyramidTileSize, this->_pyramidTileSize, CV_8U, tile);
//                auto path = "/home/gerardin/Documents/pyramidBuilding/outputs/DEBUG/"  + std::to_string(row) + "_" + std::to_string(col) + "---" + std::to_string(counter++) + ".png";
//                cv::imwrite(path, image);
//                image.release();
            }

            return 0;
        }


        size_t counter = 0;


    private:

        std::shared_ptr<PyramidBuilder> _pyramidBuilder;
        TiffImageLoader<T>* _imageLoader;
        size_t _pyramidTileSize;

    };

}

#endif //PYRAMIDBUILDING_PYRAMIDTILELOADER_H
