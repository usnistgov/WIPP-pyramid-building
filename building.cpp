#include <iostream>
#include <FastImage/api/FastImage.h>
#include <FastImage/TileLoaders/GrayscaleTiffTileLoader.h>
#include "Helper.h"
#include "rules/WriteTileRule.h"
#include "tasks/WriteTileTask.h"
#include "rules/PyramidRule.h"
#include "tasks/CreateTileTask.h"
#include "utils/MatrixAllocator.h"
#include "utils/FakeTileAllocator.h"
#include "data/Tile.h"
#include <htgs/log/TaskGraphSignalHandler.hpp>
#define uint64 uint64_hack_
#define int64 int64_hack_
#include <tiffio.h>
#undef uint64
#undef int64
#include <assert.h>
#include "utils/SingleTiledTiffWriter.h"
#include "utils/MistStitchedImageReader.h"
#include "utils/BaseTileGenerator.h"


uint32_t* generateTile(uint32_t i, uint32_t j, std::map<std::pair<uint32_t, uint32_t>, std::vector<PartialFov *>> &grid, BaseTileGenerator *generator, std::string directory) {
    std::pair<uint32_t,uint32_t> index= std::make_pair(i,j);
    auto it = grid.find(index);
    assert(it != grid.end());
    uint32_t* tile = generator->generateTile(index, it->second, directory);
    return tile;
}


void writeTile(uint32_t row, uint32_t col, uint32_t* tile, uint32 pyramidTileSize){
    //write as a tif output
    auto outputFilename = "img_r" + std::to_string(row) + "_c" + std::to_string(col) + ".tif";
    auto outputdir = "output_";
    auto w = new SingleTiledTiffWriter(outputdir + outputFilename, pyramidTileSize);
    w->write(tile);
    //graph->produceData(tile);
}

int main() {

    std::string vector = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset1/stitching_vector/img-global-positions-1.txt";
    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset1/tiled-images/";

//    std::string vector = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset02/stitching_vector/img-global-positions-1.txt";
//    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset02/images/";

//    std::string vector = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset01/stitching_vector/img-global-positions-1.txt";
//    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset01/images/";

    //pyramid
    uint32_t pyramidTileSize = 256;
//    uint32_t pyramidTileSize = 32;

    auto reader = new MistStitchedImageReader(directory, vector, pyramidTileSize);
    auto grid = reader->getGrid();

    //TODO CHECK we assume that all FOV have the same tiling scheme.
    auto tileWidth = reader->getFovTileWidth();
    auto tileHeight = reader->getFovTileHeight();

    //TODO CHECK we assume all tiles are square. This is not necessary but it is safe to assume for the first tests.
    assert(tileWidth == tileHeight);

    //TODO CHECK we could assume for now that pyramid tile size is a multiple of the underlying FOV tile size.
    //Will that be of any use?
    //assert(pyramidTileSize % tileWidth == 0);

    //generating the lowest level of the pyramid represented by a grid of pyramid tile.
    //TODO instead of simply iterating through the grid entries, we could traverse blocks to speed up pyramid generation.
    //TODO Make this an HTGS task and decouple from the TIFF write operation (this could be a PNG Write as well)
    //TODO wrap the tile represented as a raw array into a Tile object and send it through the graph.


    uint32_t numTileRow = reader->getGridMaxRow();
    uint32_t numTileCol = reader->getGridMaxCol();

    std::cout << "numTileRow : " << numTileRow << ", numTileCol : " << numTileCol << std::endl;

//    auto graph = new htgs::TaskGraphConf<Tile<uint32_t>, Tile<uint32_t> >();
//
//    auto bookeeper = new htgs::Bookkeeper<Tile<uint32_t>>();
//
//    auto writeRule = new WriteTileRule();
//
//    auto pyramidRule = new PyramidRule(numTileCol,numTileRow);
//
//    auto createTileTask = new CreateTileTask();
//
//    graph->setGraphConsumerTask(bookeeper);
//    graph->addEdge(createTileTask,bookeeper);
//    graph->addRuleEdge(bookeeper, writeRule, writeTask);
//    graph->addRuleEdge(bookeeper, pyramidRule, createTileTask);
//    graph->addGraphProducerTask(writeTask);
////    auto matAlloc = new FakeTileAllocator();
////    graph->addMemoryManagerEdge("PYRAMID_TILE", createTileTask, matAlloc, 4, htgs::MMType::Static);
//
//    htgs::TaskGraphRuntime *runtime = new htgs::TaskGraphRuntime(graph);
//
//    htgs::TaskGraphSignalHandler::registerTaskGraph(graph);
//    htgs::TaskGraphSignalHandler::registerSignal(SIGTERM   );
//
//    runtime->executeRuntime();





    uint32_t numberBlockHeight,numberBlockWidth = 0;

    numberBlockHeight = ceil((double)numTileRow/2);
    numberBlockWidth = ceil((double)numTileCol/2);

    //generate a pyramid tile
    auto generator = new BaseTileGenerator(tileWidth, tileHeight, pyramidTileSize);

    for(uint32_t j = 0; j < numberBlockHeight; j++){
        for(uint32_t i = 0; i < numberBlockWidth; i++){

            if(2*i < numTileCol && 2*j < numTileRow) {
                std::cout << 2*j << "," << 2*i << std::endl;
                auto tile = generateTile(2 * j, 2 * i, grid, generator, directory);
                writeTile(2 * j, 2 * i,tile, pyramidTileSize);
            }
            if(2*i+1 < numTileCol) {
                std::cout << 2 * j << "," << 2 * i + 1 << std::endl;
                auto tile = generateTile(2 * j, 2 * i + 1, grid, generator, directory);
                writeTile(2 * j, 2 * i + 1, tile, pyramidTileSize);
            }

            if(2*j+1 < numTileRow) {
                std::cout << 2 * j + 1 << "," << 2 * i << std::endl;
                auto tile = generateTile(2 * j + 1, 2 * i, grid, generator, directory);
                writeTile(2 * j + 1, 2 * i, tile, pyramidTileSize);
            }

            if(2*j+1 < numTileRow && 2*i+1 < numTileCol) {
                std::cout << 2 * j + 1 << "," << 2 * i + 1 << std::endl;
                auto tile = generateTile(2 * j + 1, 2 * i + 1, grid, generator, directory);
                writeTile(2 * j + 1, 2 * i + 1, tile, pyramidTileSize);
            }
        }
    }


//    graph->finishedProducingData();
//
//    while(!graph->isOutputTerminated()){
//        auto r = graph->consumeData();
//        if(r == nullptr){
//            break;
//        }
//        std::cout << r->getLevel() << ": " << r->getRow() << "," << r->getCol() << std::endl;
//    }
//
//    std::cout << "we should be done" << std::endl;
//    runtime->waitForRuntime();
//
//    delete runtime;

}





