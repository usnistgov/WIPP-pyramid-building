//
// Created by gerardin on 4/19/19.
//

#ifndef PYRAMIDBUILDING_PYRAMIDTILELOADER_H
#define PYRAMIDBUILDING_PYRAMIDTILELOADER_H

#include <FastImage/api/FastImage.h>
#include <FastImage/api/ATileLoader.h>
#include <pyramidBuilding/utils/TiffImageLoader.h>
#include <pyramidBuilding/fastImage/utils/TileRequestBuilder.h>
#include <pyramidBuilding/fastImage/data/PartialFOV.h>
#include <pyramidBuilding/fastImage/data/FITileRequest.h>

namespace pb {

    template<class T>
    class PyramidTileLoader : public fi::ATileLoader<T> {

    public:

        PyramidTileLoader(size_t numThreads, std::shared_ptr<TileRequestBuilder> tileRequestBuilder, TiffImageLoader<T>* imageLoader) : fi::ATileLoader<T>("", numThreads), _tileRequestBuilder(tileRequestBuilder), _imageLoader(imageLoader) {

        }

        //TODO CHECK. Problem might occur when duplicating this loader.
        fi::ATileLoader<T> *copyTileLoader() override {
            return new PyramidTileLoader(this->getNumThreads(), this->_tileRequestBuilder, this->_imageLoader);
        }

        uint32_t getImageHeight(uint32_t level) const override {
            return _tileRequestBuilder->getFullFovHeight();
        }

        uint32_t getImageWidth(uint32_t level) const override {
            return _tileRequestBuilder->getFullFovWidth();
        }

        uint32_t getTileWidth(uint32_t level) const override {
            return 1024;
        }

        uint32_t getTileHeight(uint32_t level) const override {
            return 1024;
        }

        short getBitsPerSample() const override {
            return _tileRequestBuilder->getFovMetadata()->getBitsPerSample();
        }

        uint32_t getNbPyramidLevels() const override {
            size_t fullFovWidth = _tileRequestBuilder->getFullFovWidth();
            size_t fullFovHeight = _tileRequestBuilder->getFullFovHeight();
            uint32_t deepZoomLevel = 0;
            //calculate pyramid depth
            auto maxDim = std::max(fullFovWidth,fullFovHeight);
            deepZoomLevel = uint32_t(ceil(log2(maxDim)) + 1);
            return deepZoomLevel;
        }

        std::string getName() override {
            return "PyramidTileLoader";
        }

        double loadTileFromFile(T *tile, uint32_t row, uint32_t col) override {
            auto tileRequest = _tileRequestBuilder->getTileRequests().at({row,col});
            for(PartialFOV* fov : tileRequest->getFovs()){
                _imageLoader->loadPartialImage(fov->getFilename(),fov->getOverlap()->originX,fov->getOverlap()->originY, fov->getOverlap()->width, fov->getOverlap()->height);
            }
        }



    private:

        std::shared_ptr<TileRequestBuilder> _tileRequestBuilder;
        TiffImageLoader<T>* _imageLoader;

    };

}

#endif //PYRAMIDBUILDING_PYRAMIDTILELOADER_H
