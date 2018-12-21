//
// Created by Gerardin, Antoine D. (Assoc) on 12/20/18.
//

#ifndef PYRAMIDBUILDING_PYRAMIDRULE_H
#define PYRAMIDBUILDING_PYRAMIDRULE_H

#include <FastImage/api/FastImage.h>
#include <math.h>
#include <array>

class PyramidRule : public htgs::IRule<htgs::MemoryData<fi::View<uint32_t>>,htgs::MemoryData<fi::View<uint32_t>>> {

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

    void applyRule(std::shared_ptr<MemoryData<fi::View<uint32_t>>> data, size_t pipelineId) override {

        auto col = data->get()->getCol();
        auto row = data->get()->getRow();
        auto level = data->get()->getPyramidLevel();

        auto blockCol = floor(col / 2);
        auto blockRow = floor(row / 2);

        std::cout << "apply pyramid rule" << std::endl;
        std::cout << "tile : (" << col << "," << row << ") ; block (" << blockCol << "," << blockRow << ")"  << std::endl;
        std::cout << "level: " << level << " ; grid size: (" << levelGridSizes[level][0] << "," <<  levelGridSizes[level][1] << ")" << std::endl;

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
            this->addResult(data);
            return;
        }

        if(col >= gridCol -1 && col % 2 == 0){
            std::cout << "corner case : column block size 2 " << std::endl;
            if(row % 2 == 0 && grids.at(level).at(SOUTH).get() != nullptr) {
                //send 2 tiles
                this->addResult(data);
            }
            else if (row % 2 != 0 && grids.at(level).at(NORTH).get() != nullptr) {
                //send 2 tiles
                this->addResult(data);
            }
            return;
        }

        if(row >= gridRow -1 && row % 2 == 0){
            std::cout << "corner case : row block size 2 " << std::endl;
            if(col % 2 == 0 && grids.at(level).at(EAST).get() != nullptr) {
                //send 2 tiles
                this->addResult(data);
            }
            else if (col % 2 != 0 && grids.at(level).at(WEST).get() != nullptr ) {
                //send 2 tiles
                this->addResult(data);
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
                //std::shared_ptr<MemoryData<fi::View<uint32_t>>> t[4] = {data, data, data, data};
                std::cout << "new tile! " << std::endl;
                this->addResult(data);
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
                this->addResult(data);
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
                this->addResult(data);
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
                this->addResult(data);
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


    const std::vector<std::vector<std::shared_ptr<MemoryData<fi::View<uint32_t>>>>> &getGrids() const {
        return grids;
    }


private:
    uint32_t numTileCol;
    uint32_t numTileRow;
    uint32_t numLevel;
    std::vector<std::array<uint32_t,2>> levelGridSizes;
    std::vector<std::vector<std::vector<fi::View<uint32_t>*>>> levels;
    std::vector<std::vector<std::shared_ptr<htgs::MemoryData<fi::View<uint32_t>>>>> grids;



};

#endif //PYRAMIDBUILDING_PYRAMIDRULE_H
