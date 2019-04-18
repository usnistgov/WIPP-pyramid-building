//
// Created by gerardin on 4/12/19.
//

#ifndef PYRAMIDBUILDING_EMPTYTILEMANAGER_H
#define PYRAMIDBUILDING_EMPTYTILEMANAGER_H

#include <glob.h>
#include <pyramidBuilding/data/TileRequest.h>
#include <pyramidBuilding/data/FOV.h>
#include <htgs/api/IRule.hpp>
#include <map>
#include <pyramidBuilding/data/Tile.h>
#include <pyramidBuilding/utils/StitchingVectorParser.h>


namespace pb {


    template <class T>
    class EmptyTileRule : public htgs::IRule<TileRequest, Tile<T>>
    {

    public:
        EmptyTileRule(std::shared_ptr<StitchingVectorParser> info) : info(info) {}


    public:
        void applyRule(std::shared_ptr<TileRequest> data, size_t pipelineId) override {
            uint32_t row = data->getRow();
            uint32_t col = data->getCol();
            auto entry = info->getFovUsage().find({row,col});

            if(entry == info->getFovUsage().end()){
                uint32_t tileSize = info->getPyramidTileSize();
                uint32_t fullWidth = info->getFullFovWidth();
                uint32_t fullHeight = info->getFullFovHeight();
                auto width = std::min(tileSize, fullWidth - col * tileSize);
                auto height = std::min(tileSize, fullHeight - row * tileSize);
                auto t = new Tile<T>(0,row, col, width, height, new T[width * height]());
                this->addResult(t);
            }
        }


    private:
        std::shared_ptr<StitchingVectorParser> info;

    };




}

#endif //PYRAMIDBUILDING_EMPTYTILEMANAGER_H
