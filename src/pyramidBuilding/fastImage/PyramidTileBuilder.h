//
// Created by gerardin on 4/19/19.
//

#ifndef PYRAMIDBUILDING_PYRAMIDTILELOADER_H
#define PYRAMIDBUILDING_PYRAMIDTILELOADER_H

#include <FastImage/api/FastImage.h>
#include <FastImage/api/ATileLoader.h>
#include <pyramidBuilding/fastImage/utils/TileRequestBuilder.h/StitchingVectorParserOld.h>
#include <pyramidBuilding/utils/TiffImageLoader.h>

namespace pb {

    template<class T>
    class PyramidClassLoader : public fi::ATileLoader<T> {

    public:

        PyramidClassLoader(size_t numThreads, StitchingVectorParserOld parser, TiffImageLoader<T> imageLoader) : fi::ATileLoader<T>("", numThreads), _parser(parser), _imageLoader(imageLoader) {

        }

        double loadTileFromFile(T *tile, uint32_t row, uint32_t col) override {

            auto fovs = _parser.getGrid().at(row,col);
            for(PartialFov fov : fovs){
                _imageLoader->loadPartialImage(fov.getFilename(),fov.getFovCoordOverlap().x,fov.getFovCoordOverlap().y, fov.getFovCoordOverlap().width, fov.getFovCoordOverlap().height);
            }

        }

    private:

        StitchingVectorParserOld _parser;
        TiffImageLoader<T>* _imageLoader;

    };

}

#endif //PYRAMIDBUILDING_PYRAMIDTILELOADER_H
