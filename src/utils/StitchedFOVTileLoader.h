//
// Created by gerardin on 2/27/19.
//

#ifndef PYRAMIDBUILDING_STITCHEDFOVTILELOADER_H
#define PYRAMIDBUILDING_STITCHEDFOVTILELOADER_H

#include <FastImage/api/ATileLoader.h>

template<class T>
class StitchedFOVTileLoader : public fi::ATileLoader<T> {

public:
    fi::ATileLoader<T> *copyTileLoader() override {
        return nullptr;
    }

    uint32_t getImageHeight(uint32_t level) const override {
        return 0;
    }

    uint32_t getImageWidth(uint32_t level) const override {
        return 0;
    }

    uint32_t getTileWidth(uint32_t level) const override {
        return 0;
    }

    uint32_t getTileHeight(uint32_t level) const override {
        return 0;
    }

    short getBitsPerSample() const override {
        return 0;
    }

    uint32_t getNbPyramidLevels() const override {
        return 0;
    }

    double loadTileFromFile(T *tile, uint32_t indexRowGlobalTile, uint32_t indexColGlobalTile) override {
        return 0;
    }

    ITask<fi::TileRequestData<T>, fi::TileRequestData<T>> *copy() override {
        return nullptr;
    }
};

#endif //PYRAMIDBUILDING_STITCHEDFOVTILELOADER_H
