//
// Created by Gerardin, Antoine D. (Assoc) on 1/19/19.
//

#ifndef PYRAMIDBUILDING_BASETILEGENERATOR_H
#define PYRAMIDBUILDING_BASETILEGENERATOR_H

#include <string>
#include <iostream>
#include <map>
#include <assert.h>
#include "SingleTiledTiffWriter.h"
#include "../data/PartialFov.h"
#include "Helper.h"
#include <FastImage/api/FastImage.h>
#include <FastImage/TileLoaders/GrayscaleTiffTileLoader.h>
#include "StitchingVectorParser.h"

/**
 * @class BaseTileGenerator BaseTileGenerator.h
 * @brief Generate pyramid base level tile.
 * @details
 * The pyramid base level is composed of tiles carved from a set of overlapping FOVs.
 * For a given pyramid tile size, we can generate a grid structure : (row,col) -> vector of partial overlapping FOVs.
 * We can then used this information to generate each tile.
 */
class BaseTileGenerator {

public:

    /**
     * The pyramid base level tile generator needs information on the general structure of the full FOVs.
     * @param reader the MistStitchedImageReader that contains information on partial FOV overlaps.
     */
    BaseTileGenerator(StitchingVectorParser *reader): grid(reader->getGrid()), directory(reader->getImageDirectoryPath()), tileWidth(
            reader->getFovTileWidth()), tileHeight(reader->getFovTileHeight()), pyramidTileSize(reader->getPyramidTileSize()),
                    fullFovWidth(reader->getFovTileWidth()), fullFovHeight(reader->getFovTileHeight()),
                    maxGridCol(reader->getGridMaxCol()), maxGridRow(reader->getGridMaxRow()) {}

    /**
     * Generate a pyramid base level tile at a specific coordinates.
     * @param index (row,col) of the tile to generate.
     * @return
     */
    uint32_t* generateTile(std::pair<uint32_t, uint32_t> index){

        uint32_t pyramidTileWidth = (index.second != maxGridCol) ? pyramidTileSize : fullFovWidth % maxGridCol;
        uint32_t pyramidTileHeight = (index.first != maxGridRow) ? pyramidTileSize : fullFovHeight % maxGridRow;

        auto it = grid.find(index);

        //Dealing with corner case.
        //It should never happen with real data, but we might have missing tiles or have so much overlap between FOVs that
        //some gap appears in the image. If this is the case, we generate an empty tile.
        if(it == grid.end()){
            //TODO we need to make sure we dont go beyond fullFOV width and height
            uint32_t* tile = new uint32_t[ pyramidTileSize * pyramidTileSize ];  //the pyramid tile we will be filling from partial FOVs.
            memset( tile, 0, pyramidTileSize * pyramidTileSize*sizeof(uint32_t) );
            return tile;
        }

        std::vector<PartialFov *> fovs = it->second;

        uint32_t* tile = new uint32_t[ pyramidTileSize * pyramidTileSize ];  //the pyramid tile we will be filling from partial FOVs.

        //TODO CHECK. MIGHT NOT BE NECESSARY. Set all values to 0
        memset( tile, 0, pyramidTileSize * pyramidTileSize*sizeof(uint32_t) );

        //iterating over each partial FOV.
        for(auto it2 = fovs.begin(); it2 != fovs.end(); ++it2) {

            auto fov = *it2;
            auto filename = fov->getPath();
            auto file = (directory + filename).c_str();
            auto extension = Helper::getExtension(filename);

            if(extension != "tiff" && extension != "tif") {
                std::cout << "File Format not recognized !" << std::endl;
                exit(1);
            }

                auto overlapFov = fov->getFovCoordOverlap();

                //coordinates in the grid of FOV tiles to load
                uint32_t startRow, startCol, endRow, endCol;
                startCol = overlapFov.x /  tileWidth;
                startRow = overlapFov.y / tileHeight;
                endCol = ( overlapFov.x + overlapFov.width - 1 ) / tileWidth;
                endRow = ( overlapFov.y + overlapFov.height - 1 ) / tileHeight;

                //nb of tiles to load - we load all tiles in parallel
                auto nbOfTileToLoad = (endCol - startCol + 1) * (endRow - startRow + 1);

                fi::ATileLoader<uint32_t> *tileLoader = new fi::GrayscaleTiffTileLoader<uint32_t>(directory + filename, nbOfTileToLoad);

                auto *fi = new fi::FastImage<uint32_t>(tileLoader, 0);
                fi->getFastImageOptions()->setNumberOfViewParallel(nbOfTileToLoad);
                fi->configureAndRun();

                //request all tiles for this partial FOV
                for(uint32_t i=startRow; i <= endRow; i++ ){
                    for (uint32_t j=startCol; j <= endCol; ++j){
                        fi->requestTile(i,j,false,0);
                    }
                }
                fi->finishedRequestingTiles();

                uint32_t xOriginGlobal, yOriginGlobal, xOrigin,yOrigin,width,height;

                //processing each tile
                while(fi->isGraphProcessingTiles()) {

                    auto pview = fi->getAvailableViewBlocking();

                    if(pview != nullptr){

                        auto view = pview->get();
                        //tile origin in FOV global coordinates
                        uint32_t tileOriginX = view->getGlobalXOffset();
                        uint32_t tileOriginY = view->getGlobalYOffset();

                        //start index in FOV global coordinates (top left corner of the rectangle to copy)
                        xOriginGlobal = std::max<uint32_t>(tileOriginX, overlapFov.x);
                        yOriginGlobal = std::max<uint32_t>(tileOriginY, overlapFov.y);

                        //start index in local coordinates (top left corner of the ROI rectangle in the current FOV tile)
                        xOrigin = xOriginGlobal - tileOriginX;
                        yOrigin = yOriginGlobal - tileOriginY;

                        //nb of pixels left in the ROI at this point
                        width = overlapFov.width - (xOriginGlobal - overlapFov.x);
                        height = overlapFov.height - (yOriginGlobal - overlapFov.y);

                        //how many fit in this tile? (width and height of the ROI rectangle)
                        auto endX = std::min(width, tileWidth);
                        auto endY = std::min(height, tileHeight);

                        //get all pixels for this ROI
                        for(uint32_t j = yOrigin; j < endY ; j++){
                            for(uint32_t i = xOrigin; i < endX ; i++){
                                auto val = view->getPixel(j,i);
                                //FOVOverlap coordinates (those are not the global coordinates, but relative to the partial FOV)
                                auto xInFOVOverlap = tileOriginX + i - overlapFov.x;
                                auto yInFOVOverlap = tileOriginY + j - overlapFov.y;

                                //TileOverlap coordinates
                                auto overlapTile = fov->getTileOverlap();

                                auto xInTile = overlapTile.x + xInFOVOverlap;
                                auto yInTile = overlapTile.y + yInFOVOverlap;

                                //to get the final tile, we report the coordinates of the pixel obtained in the FOVOverlap coordinates
                                //into the the tileOverlap coordinates.
                                auto index1D = yInTile * pyramidTileSize + xInTile;

                                assert( 0 <= index1D && index1D < 256 * 256);

                                //        std::cout << index1D << ": " << val << std::endl;

//                                if(tile[index1D] != 0){
//                                    std::cout << "overwriting at index " << index1D << " old value : " << tile[index1D] << " with value : " << val << std:: endl;
//                                }

                                tile[ index1D ] = val;

                            }
                        } //DONE copying the relevant portion of one tile of the FOV in this pyramid tile

                        pview->releaseMemory();

                    }
                } //DONE copying the relevant portion of the FOV in this pyramid tile

                //TODO CHECK we should eventually cache the fast image instances since they are used for each overlap.
                //depending on the overlap factor, some performance should be expected.
                delete fi;

        } //DONE generating the pyramid tile

        return tile;
    }

private:

    std::map<std::pair<uint32_t, uint32_t>, std::vector<PartialFov *>> grid;
    std::string directory;
    uint32_t tileWidth;
    uint32_t tileHeight;
    uint32_t pyramidTileSize;
    uint32_t fullFovWidth;
    uint32_t fullFovHeight;
    uint32_t maxGridRow;
    uint32_t maxGridCol;


};

#endif //PYRAMIDBUILDING_BASETILEGENERATOR_H
