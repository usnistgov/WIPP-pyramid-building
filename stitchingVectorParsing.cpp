//
// Created by Gerardin, Antoine D. (Assoc) on 1/2/19.
//

#include <FastImage/api/FastImage.h>
#include <FastImage/TileLoaders/GrayscaleTiffTileLoader.h>
#include "Helper.h"

#include "utils/MistStitchedImageReader.h"


#define uint64 uint64_hack_
#define int64 int64_hack_
#include <tiffio.h>
#undef uint64
#undef int64
#include <assert.h>

/**
 * This algorithm generates the lowest level of the pyramid.
 * Each pyramid tile is composed of a set of partial FOVs.
 * FOVs are tiled-tiff. Each ROI of each tile is processed asynchronously and copied back in the pyramid tile.
 * The algorithm is parallelized at the FOV tile-level using FastImage.
 * @return a map that associated 2D coordinates to a tile.
 */

int main() {
    std::string vector = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset1/stitching_vector/img-global-positions-1.txt";
    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset1/tiled-images/";

    //pyramid
    uint32_t pyramidTileSize = 256;

    //TODO test that directory and vector and not mixed up
    auto reader = new MistStitchedImageReader(directory, vector, pyramidTileSize);
    auto grid = reader->getGrid();

    //TODO CHECK we assume that all FOV have the same tiling scheme.
    auto tileWidth = reader->getFovTileWidth();
    auto tileHeight = reader->getFovTileHeight();

    //TODO CHECK we assume all tiles are square. This is not necessary but it is safe to assume for the first tests.
    assert(tileWidth == tileHeight);

    //TODO CHECK we also assume for now that pyramid tile size is a multiple of the underlying FOV tile size.
    //assert(pyramidTileSize % tileWidth == 0);

    //TODO CHECK might not be necessary, but used to determine the max number of threads used by the tile loader.
    auto maxNumberOfTilesToLoad = ceil( (float)pyramidTileSize / tileWidth ) * ceil( (float)pyramidTileSize / tileHeight );


    //generating the lowest level of the pyramid represented by a grid of pyramid tile.
    for ( auto it = grid.begin(); it != grid.end(); ++it ) {

        uint32_t tile[ pyramidTileSize*pyramidTileSize ]; //the pyramid tile we will be filling from partial FOVs.

        //iterating over each partial FOV.
        for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {

            auto fov = *it2;
            auto filename = fov->getPath();
            auto file = (directory + filename).c_str();
            auto extension = Helper::getExtension(filename);

            if(extension != "tiff" && extension != "tif") {
                std::cout << "File Format not recognized !" << std::endl;
            }

            else {
                fi::ATileLoader<uint32_t> *tileLoader = new fi::GrayscaleTiffTileLoader<uint32_t>(directory + filename, maxNumberOfTilesToLoad);

                auto overlapFov = fov->getFovCoordOverlap();

                //coordinates in the grid of FOV tiles to load
                uint32_t startRow, startCol, endRow, endCol;
                startCol = overlapFov.x /  tileWidth;
                startRow = overlapFov.y / tileHeight;
                endCol = ( overlapFov.x + overlapFov.width ) / tileWidth;
                endRow = ( overlapFov.y + overlapFov.height ) / tileHeight;

                //nb of tiles to load
                auto nbOfTileToLoad = (endCol - startCol) * (endRow - startRow);

                auto *fi = new fi::FastImage<uint32_t>(tileLoader, 0);
                fi->getFastImageOptions()->setNumberOfViewParallel(nbOfTileToLoad);
                fi->configureAndRun();

                //load all tiles for this FOV.
                for (uint32_t j=startCol; j <= endCol; ++j){
                    for(uint32_t i=startRow; i <= endRow; i++ ){
                        fi->requestTile(j,i,false,0);
                    }
                }
                fi->finishedRequestingTiles();

                uint32_t xOriginGlobal, yOriginGlobal, xOrigin,yOrigin,width,height;

                while(fi->isGraphProcessingTiles()) {

                    auto view = fi->getAvailableViewBlocking()->get();

                    if(view != nullptr){
                        //tile origin in FOV global coordinates
                        uint32_t tileOriginX = view->getGlobalXOffset();
                        uint32_t tileOriginY = view->getGlobalYOffset();

                        //start index in FOV global coordinates
                        xOriginGlobal = std::max<uint32_t>(tileOriginX, overlapFov.x);
                        yOriginGlobal = std::max<uint32_t>(tileOriginY, overlapFov.y);

                        //start index in local coordinates (FOV tile)
                        xOrigin = xOriginGlobal - tileOriginX;
                        yOrigin = yOriginGlobal - tileOriginY;

                        //nb of pixels left in the ROI
                        width = overlapFov.width - (xOriginGlobal - overlapFov.x);
                        height = overlapFov.height - (yOriginGlobal - overlapFov.y);

                        //how many fit in this tile?
                        auto endX = std::min(width, tileWidth);
                        auto endY = std::min(height, tileHeight);

                        //get all pixels for this ROI
                        for(uint32_t j = yOrigin; j < endY -1 ; j++){
                            for(uint32_t i = xOrigin; i < endX -1; i++){
                                auto val = view->getPixel(i,j);
                                //FOVOverlap coordinates (those are not the global coordinates, but relative to the partial FOV)
                                auto xInFOVOverlap = tileOriginX + i - overlapFov.x;
                                auto yInFOVOverlap = tileOriginY + j - overlapFov.y;

                                //TileOverlap coordinates
                                auto overlapTile = fov->getTileOverlap();

                                //to get the final tile, we report the coordinates of the pixel obtained in the FOVOverlap coordinates
                                //into the the tileOverlap coordinates.
                                auto index1D = (overlapTile.y + yInFOVOverlap) * overlapTile.width + overlapTile.x + xInFOVOverlap;
                                tile[ index1D ] = val;
                            }
                        } //DONE copying the relevant portion of one tile of the FOV in this pyramid tile
                    }
                } //DONE copying the relevant portion of the FOV in this pyramid tile

                //TODO CHECK we should eventually cache the fast image instances since they are used for each overlap.
                //depending on the overlap factor, some performance should be expected.
                delete fi;
            }

        } //DONE generating the pyramid tile

    } //DONE generating the lowest level of the pyramid


}
