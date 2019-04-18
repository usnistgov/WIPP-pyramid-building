//
// Created by gerardin on 4/12/19.
//

#ifndef PYRAMIDBUILDING_TILEMANAGER_H
#define PYRAMIDBUILDING_TILEMANAGER_H


#include <glob.h>
#include <pyramidBuilding/data/TileRequest.h>
#include <pyramidBuilding/data/FOV.h>
#include <htgs/api/IRule.hpp>
#include <map>
#include <pyramidBuilding/data/Tile.h>
#include <pyramidBuilding/utils/StitchingVectorParser.h>


namespace pb {

    class FOVTileRule : public htgs::IRule<TileRequest, FOV>
    {

    public:
        FOVTileRule(std::shared_ptr<StitchingVectorParser> info) : info(info) {}


    public:
        void applyRule(std::shared_ptr<TileRequest> data, size_t pipelineId) override {
            auto row = data->getRow();
            auto col = data->getCol();
            auto entry = info->getFovUsage().find({row,col});

            if(entry != info->getFovUsage().end()){
                auto fovs = entry->second;
                for(auto fov : fovs){
                    auto fovRow = fov.first, fovCol = fov.second;
                    if(usedFOVs.find({fovRow,fovCol}) == usedFOVs.end()){
                        this->addResult(info->getGrid().find({fovRow, fovCol})->second);
                        usedFOVs[{fovRow, fovCol}] = true;
                    }
                }
            }
        }


    private:
        std::shared_ptr<StitchingVectorParser> info;
        std::map<std::pair<size_t,size_t>, bool> usedFOVs = {}; //could be obviously encoded as a bitarray


    };
}


#endif //PYRAMIDBUILDING_TILEMANAGER_H
