//
// Created by Gerardin, Antoine D. (Assoc) on 12/20/18.
//

#ifndef PYRAMIDBUILDING_PYRAMIDRULE_H
#define PYRAMIDBUILDING_PYRAMIDRULE_H

#include <FastImage/api/FastImage.h>
#include <math.h>
#include <array>
#include <assert.h>
#include "../data/Tile.h"
#include "../data/BlockRequest.h"


namespace pb {

template <class T>
class PyramidRule : public htgs::IRule<Tile<T>, BlockRequest<T>> {

public:
    PyramidRule(size_t numTileCol, size_t numTileRow) :  numTileCol(numTileCol), numTileRow(numTileRow) {

        //calculate pyramid depth
        auto maxDim = std::max(numTileCol,numTileRow);
        numLevel = static_cast<size_t>(ceil(log2(maxDim)) + 1);

        //calculate number of tiles for each level
        size_t levelCol, levelRow;
        levelCol = numTileCol;
        levelRow = numTileRow;
        for(size_t l=0; l<numLevel; l++){
            std::array<size_t,2> gridSize = { (size_t)levelCol, (size_t)levelRow };
            levelGridSizes.push_back(gridSize);
            levelCol = static_cast<size_t>(ceil((double)levelCol/2));
            levelRow = static_cast<size_t>(ceil((double)levelRow /2));
        }

        //dimension the tile cache for each level of the pyramid
        pyramidCache.resize(numLevel);
        levelCol = numTileCol;
        levelRow = numTileRow;
        for (auto it = pyramidCache.begin() ; it != pyramidCache.end(); ++it) {
            it->resize(levelCol * levelRow);
            levelCol = static_cast<size_t>(ceil((double)levelCol/2));
            levelRow = static_cast<size_t>(ceil((double)levelRow /2));
        }

    }

    std::string getName() override {
        return "Pyramid Rule - Cache";
    }

    void applyRule(std::shared_ptr<Tile<T>> data, size_t pipelineId) override {

        auto col = data->getCol();
        auto row = data->getRow();
        auto level = data->getLevel();


        if(level > 0) {
            auto gridCol = levelGridSizes[level-1][0];

            std::vector<std::shared_ptr<Tile<T>>> &l = this->pyramidCache.at(level - 1);

            for(std::shared_ptr<Tile<T>>& value: data->getOrigin()) {
                if(value!= nullptr) { //second value can be null for vertical block.
                    removeFromCache(l, value->getRow() * gridCol + value->getCol());
                    value.reset(); //delete from the origin vector so it can be reclaimed.
                }
            }
        }

        if(level == this->numLevel -1){
            VLOG(3) << "done generating last level." << std::endl;
            done = true;
            return;
        }

        std::ostringstream oss;
        oss << "applying pyramid rule \n" << "tile : (" << row << "," << col << ")" <<
                                          " - grid size at level: " << level << " (" << levelGridSizes[level][0] << "," <<  levelGridSizes[level][1] << ")";
        VLOG(3)  << oss.str() << std::endl;

        auto gridCol = levelGridSizes[level][0];
        auto gridRow = levelGridSizes[level][1];


        auto SOUTH = (row + 1) * gridCol + col;
        auto NORTH = (row - 1) * gridCol + col;
        auto EAST = row * gridCol + col + 1;
        auto WEST = row * gridCol + col - 1;
        auto NORTH_WEST = (row -1) * gridCol + col - 1;
        auto NORTH_EAST = (row -1) * gridCol + col + 1;
        auto SOUTH_WEST = (row +1) * gridCol + col - 1;
        auto SOUTH_EAST = (row +1) * gridCol + col + 1;

        pyramidCache.at(level).at(row * gridCol + col) = data;

        if(col >= gridCol -1 && row >= gridRow -1 && col % 2 ==0 && row % 2 ==0) {
            VLOG(3) << "corner case : block size 1 " << std::endl;
            //sendTile
            std::vector<std::shared_ptr<Tile<T>>> block{data};
            this->addResult(new BlockRequest<T>(block));
            return;
        }

        if(col >= gridCol -1 && col % 2 == 0){
            VLOG(3) << "corner case : column block size 2 " << std::endl;
            if(row % 2 == 0 && pyramidCache.at(level).at(SOUTH).get() != nullptr) {
                //send 2 tiles
                std::vector<std::shared_ptr<Tile<T>>> block{ data, nullptr, pyramidCache.at(level).at(SOUTH) };
                this->addResult(new BlockRequest<T>(block));
            }
            else if (row % 2 != 0 && pyramidCache.at(level).at(NORTH).get() != nullptr) {
                //send 2 tiles
                std::vector<std::shared_ptr<Tile<T>>> block{ pyramidCache.at(level).at(NORTH), nullptr, data };
                this->addResult(new BlockRequest<T>(block));
            }
            return;
        }

        if(row >= gridRow -1 && row % 2 == 0){
            VLOG(3) << "corner case : row block size 2 " << std::endl;
            if(col % 2 == 0 && pyramidCache.at(level).at(EAST).get() != nullptr) {
                //send 2 tiles
                std::vector<std::shared_ptr<Tile<T>>> block{ data, pyramidCache.at(level).at(EAST) };
                this->addResult(new BlockRequest<T>(block));
            }
            else if (col % 2 != 0 && pyramidCache.at(level).at(WEST).get() != nullptr ) {
                //send 2 tiles
                std::vector<std::shared_ptr<Tile<T>>> block{ pyramidCache.at(level).at(WEST), data };
                this->addResult(new BlockRequest<T>(block));
            }
            return;
        }

        if(col % 2 == 0 && row % 2 == 0) {
            VLOG(3) << "check SE " << std::endl;
            //check SE
            if( pyramidCache.at(level).at(EAST).get() != nullptr &&
                pyramidCache.at(level).at(SOUTH).get() != nullptr &&
                pyramidCache.at(level).at(SOUTH_EAST).get() != nullptr){
                //sendTile
                VLOG(3) << "new tile! " << std::endl;
                std::vector<std::shared_ptr<Tile<T>>> block{ data, pyramidCache.at(level).at(EAST), pyramidCache.at(level).at(SOUTH), pyramidCache.at(level).at(SOUTH_EAST)};
                this->addResult(new BlockRequest<T>(block));
            };
        }

        else if(col % 2 != 0 && row % 2 == 0){
            //check SW
            VLOG(3) << "check SW " << std::endl;
            if( pyramidCache.at(level).at(WEST).get() != nullptr &&
                pyramidCache.at(level).at(SOUTH).get() != nullptr &&
                pyramidCache.at(level).at(SOUTH_WEST).get() != nullptr){
                //sendTile
                VLOG(3) << "new tile! " << std::endl;
                std::vector<std::shared_ptr<Tile<T>>> block{ pyramidCache.at(level).at(WEST), data, pyramidCache.at(level).at(SOUTH_WEST), pyramidCache.at(level).at(SOUTH)};
                this->addResult(new BlockRequest<T>(block));
            }
        }

        else if(col % 2 == 0 && row % 2 != 0){
            //check NE
            VLOG(3) << "check NE " << std::endl;
            if( pyramidCache.at(level).at(NORTH).get() != nullptr &&
                pyramidCache.at(level).at(NORTH_EAST).get() != nullptr &&
                pyramidCache.at(level).at(EAST).get() != nullptr){
                //sendTile
                VLOG(3) << "new tile! " << std::endl;
                std::vector<std::shared_ptr<Tile<T>>> block{ pyramidCache.at(level).at(NORTH), pyramidCache.at(level).at(NORTH_EAST), data, pyramidCache.at(level).at(EAST)};
                this->addResult(new BlockRequest<T>(block));
            }
        }

        else if(col % 2 != 0 && row % 2 != 0){
            //check NW
            VLOG(3) << "check NW " << std::endl;
            if( pyramidCache.at(level).at(NORTH_WEST).get() != nullptr &&
                pyramidCache.at(level).at(NORTH).get() != nullptr &&
                pyramidCache.at(level).at(WEST).get() != nullptr){
                //sendTile
                VLOG(3) << "new tile! " << std::endl;
                std::vector<std::shared_ptr<Tile<T>>> block{ pyramidCache.at(level).at(NORTH_WEST), pyramidCache.at(level).at(NORTH), pyramidCache.at(level).at(WEST), data};
                this->addResult(new BlockRequest<T>(block));
            }
        }

    }

    bool canTerminateRule(size_t pipelineId) override {
        return done;
    }


private:
    void removeFromCache(std::vector<std::shared_ptr<Tile<T>>> &level, size_t index){
        assert(level.at(index) != nullptr);
        level[index].reset();
    }

    size_t numTileCol;
    size_t numTileRow;
    size_t numLevel;
    std::vector<std::array<size_t,2>> levelGridSizes;
    std::vector<std::vector<std::shared_ptr<Tile<T>>>> pyramidCache;

    bool done = false;



};

}

#endif //PYRAMIDBUILDING_PYRAMIDRULE_H