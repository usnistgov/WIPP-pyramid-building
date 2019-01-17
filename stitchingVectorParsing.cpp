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

    //TODO REMOVE FOR DEBUG ONLY
    int counter=0;


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

        uint32_t tile[ pyramidTileSize * pyramidTileSize ]; //the pyramid tile we will be filling from partial FOVs.
        memset( tile, 0, pyramidTileSize * pyramidTileSize*sizeof(uint32_t) );

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
                //TODO CHANGE BACK AFTER DEBUGGING
               // auto nbOfTileToLoad = (endCol - startCol) * (endRow - startRow);
                auto nbOfTileToLoad = 1;

                auto *fi = new fi::FastImage<uint32_t>(tileLoader, 0);
                fi->getFastImageOptions()->setNumberOfViewParallel(nbOfTileToLoad);
                fi->configureAndRun();

                //load all tiles for this FOV.
                for(uint32_t i=startRow; i <= endRow; i++ ){
                    for (uint32_t j=startCol; j <= endCol; ++j){
                        fi->requestTile(i,j,false,0);
                    }
                }
                fi->finishedRequestingTiles();

                uint32_t xOriginGlobal, yOriginGlobal, xOrigin,yOrigin,width,height;

                while(fi->isGraphProcessingTiles()) {

                    auto pview = fi->getAvailableViewBlocking();

                     std::cout << "debug view : " << pview;

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
                                auto val = view->getPixel(i,j);
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

//                                std::cout << index1D << ": " << val << std::endl;

                                if(tile[index1D] != 0){
//                                    std::cout << "overwriting at index " << index1D << " old value : " << tile[index1D] << " with value : " << val << std:: endl;
                                }

                                tile[ index1D ] = val;


                            }
                        } //DONE copying the relevant portion of one tile of the FOV in this pyramid tile
                        pview->releaseMemory();

                        ++counter;
                        auto outputFilename = "img_r" + std::to_string(it->first.first) + "_c" + std::to_string(it->first.second) + "_t" + std::to_string(counter) + ".tif";
                        auto outputdir = "output_";
                        auto outputfile = (outputdir + outputFilename).c_str();
                        TIFF* tif = TIFFOpen(outputfile, "w");
                        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, pyramidTileSize);
                        TIFFSetField(tif, TIFFTAG_IMAGELENGTH, pyramidTileSize);
                        TIFFSetField(tif, TIFFTAG_TILELENGTH, pyramidTileSize);
                        TIFFSetField(tif, TIFFTAG_TILEWIDTH, pyramidTileSize);
                        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 32);
                        TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
                        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1);
                        TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
                        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
                        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
                        TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
                        TIFFWriteTile(tif, tile, 0, 0, 0, 0);
                        TIFFClose(tif);
                    }
                } //DONE copying the relevant portion of the FOV in this pyramid tile

                //TODO CHECK we should eventually cache the fast image instances since they are used for each overlap.
                //depending on the overlap factor, some performance should be expected.
                delete fi;
            }



        } //DONE generating the pyramid tile



    } //DONE generating the lowest level of the pyramid


}
