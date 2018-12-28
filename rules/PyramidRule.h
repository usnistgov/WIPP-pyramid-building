//
// Created by Gerardin, Antoine D. (Assoc) on 12/20/18.
//

#ifndef PYRAMIDBUILDING_PYRAMIDRULE_H
#define PYRAMIDBUILDING_PYRAMIDRULE_H

#include <FastImage/api/FastImage.h>
#include <math.h>
#include <array>
#include "../data/Tile.h"
#include "../data/TileRequest.h"

class PyramidRule : public htgs::IRule<Tile<uint32_t>, TileRequest<uint32_t>> {

public:
    PyramidRule(uint32_t numTileCol, uint32_t numTileRow) :  numTileCol(numTileCol), numTileRow(numTileRow) {
        auto maxDim = std::max(numTileCol,numTileRow);

        numLevel = ceil(log2(maxDim)) + 1;


        double levelCol, levelRow;
        levelCol = numTileCol;
        levelRow = numTileRow;

        for(uint32_t l=0; l<numLevel; l++){
            std::array<uint32_t,2> gridSize = { (uint32_t)levelCol, (uint32_t)levelRow };
            levelGridSizes.push_back(gridSize);
            levelCol = ceil(levelCol/2);
            levelRow = ceil(levelRow /2);
        }

        grids.resize(numLevel);

        levelCol = numTileCol;
        levelRow = numTileRow;

        for (auto it = grids.begin() ; it != grids.end(); ++it) {
            it->resize(levelCol * levelRow);
            levelCol = ceil(levelCol/2);
            levelRow = ceil(levelRow /2);
        }

    }

    void applyRule(std::shared_ptr<Tile<uint32_t>> data, size_t pipelineId) override {

        auto col = data->getCol();
        auto row = data->getRow();
        auto level = data->getLevel();



        if(level > 0) {
            auto l = grids[level - 1];
            auto gridCol = levelGridSizes[level-1][0];
            auto gridRow = levelGridSizes[level-1][1];

            size_t i1 = 2 *row * gridCol + 2 * col;
            size_t i2 = 2* row * gridCol + 2 * col + 1;
            size_t i3 = (2 *row + 1) * gridCol + 2 * col;
            size_t i4 = (2 * row + 1) * gridCol + 2 * col + 1;
            removeFromCache(l, i1);
            removeFromCache(l, i2);
            removeFromCache(l, i3);
            removeFromCache(l, i4);
        }

        if(level == this->numLevel){
            return;
        }

        std::ostringstream oss;
        oss << "applying pyramid rule \n" << "tile : (" << row << "," << col << ")" <<
                                          " - grid size at level: " << level << " (" << levelGridSizes[level][0] << "," <<  levelGridSizes[level][1] << ")";
        std::cout  << oss.str() << std::endl;

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

        grids.at(level).at(row * gridCol + col) = data;

        if(col >= gridCol -1 && row >= gridRow -1 && col % 2 ==0 && row % 2 ==0) {
            std::cout << "corner case : block size 1 " << std::endl;
            //sendTile
            std::vector<std::shared_ptr<Tile<uint32_t>>> block{data};
            this->addResult(new TileRequest<uint32_t>(block));
            return;
        }

        if(col >= gridCol -1 && col % 2 == 0){
            std::cout << "corner case : column block size 2 " << std::endl;
            if(row % 2 == 0 && grids.at(level).at(SOUTH).get() != nullptr) {
                //send 2 tiles
                std::vector<std::shared_ptr<Tile<uint32_t>>> block{ data, nullptr, grids.at(level).at(SOUTH) };
                this->addResult(new TileRequest<uint32_t>(block));
            }
            else if (row % 2 != 0 && grids.at(level).at(NORTH).get() != nullptr) {
                //send 2 tiles
                std::vector<std::shared_ptr<Tile<uint32_t>>> block{ grids.at(level).at(NORTH), nullptr, data };
                this->addResult(new TileRequest<uint32_t>(block));
            }
            return;
        }

        if(row >= gridRow -1 && row % 2 == 0){
            std::cout << "corner case : row block size 2 " << std::endl;
            if(col % 2 == 0 && grids.at(level).at(EAST).get() != nullptr) {
                //send 2 tiles
                std::vector<std::shared_ptr<Tile<uint32_t>>> block{ data, grids.at(level).at(EAST) };
                this->addResult(new TileRequest<uint32_t>(block));
            }
            else if (col % 2 != 0 && grids.at(level).at(WEST).get() != nullptr ) {
                //send 2 tiles
                std::vector<std::shared_ptr<Tile<uint32_t>>> block{ grids.at(level).at(WEST), data };
                this->addResult(new TileRequest<uint32_t>(block));
            }
            return;
        }

        if(col % 2 == 0 && row % 2 == 0) {
            std::cout << "check SE " << std::endl;
            //check SE
            if( grids.at(level).at(EAST).get() != nullptr &&
                grids.at(level).at(SOUTH).get() != nullptr &&
                grids.at(level).at(SOUTH_EAST).get() != nullptr){
                //sendTile
                std::cout << "new tile! " << std::endl;
                std::vector<std::shared_ptr<Tile<uint32_t>>> block{ data, grids.at(level).at(EAST), grids.at(level).at(SOUTH), grids.at(level).at(SOUTH_EAST)};
                this->addResult(new TileRequest<uint32_t>(block));
            };
        }

        else if(col % 2 != 0 && row % 2 == 0){
            //check SW
            std::cout << "check SW " << std::endl;
            if( grids.at(level).at(WEST).get() != nullptr &&
                grids.at(level).at(SOUTH).get() != nullptr &&
                grids.at(level).at(SOUTH_WEST).get() != nullptr){
                //sendTile
                std::cout << "new tile! " << std::endl;
                std::vector<std::shared_ptr<Tile<uint32_t>>> block{ grids.at(level).at(WEST), data, grids.at(level).at(SOUTH_WEST), grids.at(level).at(SOUTH)};
                this->addResult(new TileRequest<uint32_t>(block));
            }
        }

        else if(col % 2 == 0 && row % 2 != 0){
            //check NE
            std::cout << "check NE " << std::endl;
            if( grids.at(level).at(NORTH).get() != nullptr &&
                grids.at(level).at(NORTH_EAST).get() != nullptr &&
                grids.at(level).at(EAST).get() != nullptr){
                //sendTile
                std::cout << "new tile! " << std::endl;
                std::vector<std::shared_ptr<Tile<uint32_t>>> block{ grids.at(level).at(NORTH), grids.at(level).at(NORTH_EAST), data, grids.at(level).at(EAST)};
                this->addResult(new TileRequest<uint32_t>(block));
            }
        }

        else if(col % 2 != 0 && row % 2 != 0){
            //check NW
            std::cout << "check NW " << std::endl;
            if( grids.at(level).at(NORTH_WEST).get() != nullptr &&
                grids.at(level).at(NORTH).get() != nullptr &&
                grids.at(level).at(WEST).get() != nullptr){
                //sendTile
                std::cout << "new tile! " << std::endl;
                std::vector<std::shared_ptr<Tile<uint32_t>>> block{ grids.at(level).at(NORTH_WEST), grids.at(level).at(NORTH), grids.at(level).at(WEST), data};
                this->addResult(new TileRequest<uint32_t>(block));
            }
        }



        //add to block
//        std::vector<std::vector<fi::View<uint32_t>>> l = levels.at(level);
//        std::vector<fi::View<uint32_t>> b = l.at(blockCol * levelGridSizes[level][0] + blockRow);
//        b.push_back(data->get(0));
//
//
//        //if block is full, send block
//        if(blockCol == floor(levelGridSizes[level][0]) && blockRow == floor(levelGridSizes[level][1]) && b.size() == 1){
//            //block is full
//        }
//        if(blockCol == floor(levelGridSizes[level][0]) && blockRow && b.size() == 2){
//
//        }
//        if(blockRow == floor(levelGridSizes[level][1]) && blockRow && b.size() == 2){
//
//        }
//        if(blockRow && b.size() == 4){
//
//        }



    }


private:

    void removeFromCache(std::vector<std::shared_ptr<Tile<uint32_t>>> l, size_t index){
        if ((index >= 0 && index < l.size()) && (l[index] != NULL)){
            auto tile = l[index].get();
            if(tile->getLevel() == 0){
                tile->getOrigin()->releaseMemory();
            }
            delete l[index].get();
        }
    }

    uint32_t numTileCol;
    uint32_t numTileRow;
    uint32_t numLevel;
    std::vector<std::array<uint32_t,2>> levelGridSizes;
    std::vector<std::vector<std::shared_ptr<Tile<uint32_t>>>> grids;



};

#endif //PYRAMIDBUILDING_PYRAMIDRULE_H
