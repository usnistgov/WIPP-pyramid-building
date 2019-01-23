//
// Created by Gerardin, Antoine D. (Assoc) on 1/2/19.
//

#include "src/utils/StitchingVectorParser.h"
#include "src/utils/BaseTileGenerator.h"
#define uint64 uint64_hack_
#define int64 int64_hack_
#include <tiffio.h>
#undef uint64
#undef int64
#include <assert.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "src/utils/SingleTiledTiffWriter.h"

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

//    std::string vector = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset02/stitching_vector/img-global-positions-1.txt";
//    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset02/images/";

//    std::string vector = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset01/stitching_vector/img-global-positions-1.txt";
//    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset01/images/";

    //pyramid
    uint32_t pyramidTileSize = 256;
 //  uint32_t pyramidTileSize = 32;

    auto reader = new StitchingVectorParser(directory, vector, pyramidTileSize);
    auto grid = reader->getGrid();

    //TODO CHECK we assume that all FOV have the same tiling scheme.
    auto tileWidth = reader->getFovTileWidth();
    auto tileHeight = reader->getFovTileHeight();

    //TODO CHECK we tested only for square tiles. This is not necessarily the case. The algorithm normally support any tile shape.
    //assert(tileWidth == tileHeight);

    //TODO CHECK we could assume for now that pyramid tile size is a multiple of the underlying FOV tile size.
    //Will that be of any use?
    //assert(pyramidTileSize % tileWidth == 0);

    //generating the lowest level of the pyramid represented by a grid of pyramid tile.
    //TODO instead of simply iterating through the grid entries, we could traverse blocks to speed up pyramid generation.
    //TODO Make this an HTGS task and decouple from the TIFF write operation (this could be a PNG Write as well)
    //TODO wrap the tile represented as a raw array into a Tile object and send it through the graph.

    std::vector<uint32_t*> tiles;

    for ( auto it = grid.begin(); it != grid.end(); ++it ) {

        //generate a pyramid tile
        auto generator = new BaseTileGenerator(reader);
        uint32_t* tile = generator->generateTile(it->first);

        tiles.push_back(tile);

        //write as a tif output
        auto outputFilename = "img_r" + std::to_string(it->first.second) + "_c" + std::to_string(it->first.first) + ".tif";
        auto outputdir = "output_";
        auto w = new SingleTiledTiffWriter(outputdir + outputFilename, pyramidTileSize);
        w->write(tile);

    } //DONE generating the lowest level of the pyramid
}
