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
#include "GridGenerator.h"
#include "../data/Tile.h"
#include "../api/Datatype.h"
#include <experimental/filesystem>
#include "FOVCache.h"

#define DEBUG(x) do { std::cerr << x << std::endl; } while (0)

using namespace std::experimental;

/**
 * @class BaseTileGenerator BaseTileGenerator.h
 * @brief Generate pyramid base level tile.
 * @details
 * The pyramid base level is composed of tiles carved from a set of overlapping FOVs.
 * For a given pyramid tile size, we can generate a grid structure : (row,col) -> vector of partial overlapping FOVs.
 * We can then used this information to generate each tile.
 */
template <class T>
class BaseTileGenerator {

public:

    /**
     * The pyramid base level tile generator needs information on the general structure of the full FOVs.
     * @param reader the MistStitchedImageReader that contains information on partial FOV overlaps.
     */
    BaseTileGenerator(GridGenerator *reader, BlendingMethod blendingMethod): grid(reader->getGrid()), directory(reader->getImageDirectoryPath()), tileWidth(
            reader->getFovTileWidth()), tileHeight(reader->getFovTileHeight()), pyramidTileSize(reader->getPyramidTileSize()),
                    fullFovWidth(reader->getFullFovWidth()), fullFovHeight(reader->getFullFovHeight()),
                    maxGridCol(reader->getGridMaxCol()), maxGridRow(reader->getGridMaxRow()), blendingMethod(blendingMethod) {
        fovsCache = new FOVCache<T>(reader->getImageDirectoryPath(), reader->getCache());
    }

    /**
     * Generate a pyramid base level tile at a specific coordinates.
     * @param index (row,col) of the tile to generate.
     * @return
     */
    Tile<T>* generateTile(std::pair<size_t, size_t> index){

        std::cout << "generating base tile at index (" << index.first << "," << index.second << ")" << std::endl;

        auto row = index.first;
        auto col = index.second;

        size_t pyramidTileWidth = (index.second != maxGridCol) ? pyramidTileSize : fullFovWidth  - col * pyramidTileSize;
        size_t pyramidTileHeight = (index.first != maxGridRow) ? pyramidTileSize : fullFovHeight - row * pyramidTileSize;
        T* tile = new T[ pyramidTileWidth * pyramidTileHeight ]();  //the pyramid tile we will be filling from partial FOVs.

        auto it = grid.find(index);



        //Dealing with corner case.
        //It should never happen with real data, but we might have missing tiles or have so much overlap between FOVs that
        //some gap appears in the image. If this is the case, we generate an empty tile.
        if(it == grid.end()){
            DEBUG("A gap was found in the grid of tiles at (" + std::to_string(index.first) + "," + std::to_string(index.second) + "). Generating a empty tile.");
            return new Tile<T>(0, index.first,index.second, pyramidTileWidth, pyramidTileHeight, tile);
        }

        std::vector<PartialFov *> fovs = it->second;

        uint32_t counter = 0;

        //iterating over each partial FOV.
        for(auto it2 = fovs.begin(); it2 != fovs.end(); ++it2) {

            auto fov = *it2;
            auto filename = fov->getPath();
            auto extension = Helper::getExtension(filename);

            if(extension != "tiff" && extension != "tif") {
                std::cerr << "File Format not recognized !" << std::endl;
                exit(1);
            }

                auto overlapFov = fov->getFovCoordOverlap();

                //coordinates in the grid of FOV tiles to load
                size_t startRow, startCol, endRow, endCol;
                startCol = overlapFov.x /  tileWidth;
                startRow = overlapFov.y / tileHeight;
                //TODO CHECK if -1 is correct
                endCol = ( overlapFov.x + overlapFov.width - 1 ) / tileWidth;
                endRow = ( overlapFov.y + overlapFov.height - 1 ) / tileHeight;

                //nb of tiles to load - we load all tiles in parallel
                auto nbOfTileToLoad = (endCol - startCol + 1) * (endRow - startRow + 1);

                fi::FastImage<T> *fi = fovsCache->getFOVReader(filename);
                std::cout << "Fast Image Get: " << fi << std::endl;

//                auto it = fovsCache.find(filename);

//                if(it == fovsCache.end()) {
//                    fi::ATileLoader<T> *tileLoader = new fi::GrayscaleTiffTileLoader<T>(directory + filename, 1);
//                    fi = new fi::FastImage<T>(tileLoader, 0);
//                    fi->getFastImageOptions()->setNumberOfViewParallel(nbOfTileToLoad);
//                    fi->configureAndRun();
//                    fovsCache[filename] = fi;
//                }
//                else{
//                   fi = it->second;
//                }

                //request all tiles for this partial FOV
                for(auto i=startRow; i <= endRow; i++ ){
                    for (auto j=startCol; j <= endCol; ++j){
                        //TODO CHECK if we should keep using size_t. Here FAST IMAGE uses uint32.
                        fi->requestTile(i,j,false,0);
                    }
                }
                fi->finishedRequestingTiles();

                size_t xOriginGlobal, yOriginGlobal, xOrigin,yOrigin,width,height;

                //processing each tile
                while(fi->isGraphProcessingTiles()) {

                    auto pview = fi->getAvailableViewBlocking();

                    if(pview != nullptr){

                        auto view = pview->get();
                        //tile origin in FOV global coordinates
                        size_t tileOriginX = view->getGlobalXOffset();
                        size_t tileOriginY = view->getGlobalYOffset();

                        //start index in FOV global coordinates (top left corner of the rectangle to copy)
                        xOriginGlobal = std::max<size_t>(tileOriginX, overlapFov.x);
                        yOriginGlobal = std::max<size_t>(tileOriginY, overlapFov.y);

                        //start index in local coordinates (top left corner of the ROI rectangle in the current FOV tile)
                        xOrigin = xOriginGlobal - tileOriginX;
                        yOrigin = yOriginGlobal - tileOriginY;

                        //nb of pixels left in the ROI at this point
                        width = overlapFov.width - (xOriginGlobal - overlapFov.x);
                        height = overlapFov.height - (yOriginGlobal - overlapFov.y);

                        //how many fit in this tile? (width and height of the ROI rectangle)
                        auto endX = std::min(width + xOrigin, tileWidth);
                        auto endY = std::min(height + yOrigin, tileHeight);

                        //get all pixels for this ROI
                        for(auto j = yOrigin; j < endY ; j++){
                            for(auto i = xOrigin; i < endX ; i++){
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
                                auto index1D = yInTile * pyramidTileWidth + xInTile;

                                assert( 0 <= index1D && index1D < pyramidTileWidth * pyramidTileHeight);

                                //        std::cout << index1D << ": " << val << std::endl;

//                                if(tile[index1D] != 0){
//                                    std::cout << "overwriting at index " << index1D << " old value : " << tile[index1D] << " with value : " << val << std:: endl;
//                                }

                                //TODO Rather inject a blending strategy?
                                switch(blendingMethod) {
                                    case BlendingMethod::MAX:
                                        if (val > tile[index1D]) {
                                            tile[index1D] = val;
                                        }
                                        break;
                                    case BlendingMethod::OVERLAY:
                                    default:
                                        tile[index1D] = val;
                                        break;
                                }
                            }
                        } //DONE copying the relevant portion of one tile of the FOV in this pyramid tile

                        pview->releaseMemory();

                    }

                } //DONE copying the relevant portion of the FOV in this pyramid tile

            //auto count = --fovsUsageCount[fov->getPath()];
              fovsCache->releaseFOVReader(fi, fov->getPath());

                //TODO REMOVE. FOR DEBUGGING MIST DATASET
//            if(col == 16){
//            if(! filesystem::exists(filesystem::current_path() / "debugBaseTile")) {
//                filesystem::create_directory(filesystem::current_path() / "debugBaseTile");
//            }
//                    cv::Mat image(pyramidTileHeight, pyramidTileWidth, CV_8UC1, tile);
//                    std::string fullImagePath = "/home/gerardin/Documents/pyramidBuilding/cmake-build-debug/debugBaseTile/" + std::to_string(col) + "_" + std::to_string(row) + "_" + std::to_string(counter) + ".png";
//                    cv::imwrite(fullImagePath, image);
//                    ++counter;
//                }

//            if(count == 0) {
//                //TODO CHECK we should eventually cache the fast image instances since they are used for each overlap.
//                //depending on the overlap factor, some performance should be expected.
//                std::cout << "delete " << fov->getPath() << std::endl;
//      //          delete fi;
//            }



        } //DONE generating the pyramid tile

        std::cout << "base tile generated at index (" << index.first << "," << index.second << ")" << std::endl;

        return new Tile<T>(0, index.first,index.second, pyramidTileWidth, pyramidTileHeight, tile);
    }

private:
    FOVCache<T> *fovsCache;
    const std::map<std::pair<size_t, size_t>, std::vector<PartialFov *>> grid;
    const std::string directory;
    const size_t tileWidth;
    const size_t tileHeight;
    const size_t pyramidTileSize;
    const size_t fullFovWidth;
    const size_t fullFovHeight;
    const size_t maxGridRow;
    const size_t maxGridCol;
    const BlendingMethod blendingMethod;


};

#endif //PYRAMIDBUILDING_BASETILEGENERATOR_H
