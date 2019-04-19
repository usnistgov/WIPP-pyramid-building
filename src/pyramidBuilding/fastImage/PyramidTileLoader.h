//
// Created by gerardin on 4/19/19.
//

#ifndef PYRAMIDBUILDING_PYRAMIDTILELOADER_H
#define PYRAMIDBUILDING_PYRAMIDTILELOADER_H

#include <FastImage/api/FastImage.h>
#include <FastImage/api/ATileLoader.h>
#include <pyramidBuilding/utils/deprecated/StitchingVectorParserOld.h>

namespace pb {

    template<class T>
    class PyramidClassLoader : public fi::ATileLoader<T> {

    public:

        PyramidClassLoader(size_t numThreads, StitchingVectorParserOld parser) : fi::ATileLoader<T>("", numThreads), _parser(parser) {

        }

        double loadTileFromFile(T *tile, uint32_t row, uint32_t col) override {


        }

    private:

        StitchingVectorParserOld _parser;

    };

}

#endif //PYRAMIDBUILDING_PYRAMIDTILELOADER_H
