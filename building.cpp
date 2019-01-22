#include <iostream>
#include <FastImage/api/FastImage.h>
#include <FastImage/TileLoaders/GrayscaleTiffTileLoader.h>
#include "Helper.h"
#include "rules/WriteTileRule.h"
#include "tasks/WriteTileTask.h"
#include "rules/PyramidRule.h"
#include "tasks/CreateTileTask.h"
#include "tasks/BaseTileTask.h"
#include "tasks/OutputTask.h"
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
#include <data/TileRequest.h>
#include "utils/SingleTiledTiffWriter.h"
#include "utils/MistStitchedImageReader.h"
#include "utils/BaseTileGenerator.h"


uint32_t* generateTile(uint32_t i, uint32_t j, std::map<std::pair<uint32_t, uint32_t>, std::vector<PartialFov *>> &grid, BaseTileGenerator *generator, std::string directory) {
    std::pair<uint32_t,uint32_t> index= std::make_pair(i,j);
    auto it = grid.find(index);
    assert(it != grid.end());
    uint32_t* tile = generator->generateTile(index);
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

//    std::string vector = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset1/stitching_vector/img-global-positions-1.txt";
//    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset1/tiled-images/";

    std::string vector = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset02/stitching_vector/img-global-positions-1.txt";
    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset02/images/";

//    std::string vector = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset01/stitching_vector/img-global-positions-1.txt";
//    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset01/images/";

    //pyramid
//    uint32_t pyramidTileSize = 256;
    uint32_t pyramidTileSize = 32;

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


    uint32_t numTileRow = reader->getGridMaxRow() + 1;
    uint32_t numTileCol = reader->getGridMaxCol() + 1;

    std::cout << "numTileRow : " << numTileRow << ", numTileCol : " << numTileCol << std::endl;

    auto graph = new htgs::TaskGraphConf<TileRequest, Tile<uint32_t> >();

    BaseTileGenerator* generator = new BaseTileGenerator(reader);
    auto baseTileTask = new BaseTileTask(generator);

    auto bookeeper = new htgs::Bookkeeper<Tile<uint32_t>>();

    auto writeRule = new WriteTileRule();

    auto pyramidRule = new PyramidRule(numTileCol,numTileRow);

    auto createTileTask = new CreateTileTask();

    auto writeTask = new WriteTileTask("output", pyramidTileSize);

    graph->setGraphConsumerTask(baseTileTask);

    //incoming edges from the bookeeper
    graph->addEdge(baseTileTask, bookeeper); //pyramid base level tile
    graph->addEdge(createTileTask,bookeeper); //pyramid higher level tile

    //outgoing edges
    graph->addRuleEdge(bookeeper, pyramidRule, createTileTask); //caching tiles and creating a tile at higher level;
    graph->addRuleEdge(bookeeper, writeRule, writeTask); //exiting the graph;

    //output task
    graph->addGraphProducerTask(writeTask);

//    auto matAlloc = new FakeTileAllocator();
//    graph->addMemoryManagerEdge("PYRAMID_TILE", createTileTask, matAlloc, 4, htgs::MMType::Static);

    htgs::TaskGraphRuntime *runtime = new htgs::TaskGraphRuntime(graph);

    htgs::TaskGraphSignalHandler::registerTaskGraph(graph);
    htgs::TaskGraphSignalHandler::registerSignal(SIGTERM   );

    runtime->executeRuntime();

    //HTGS Graph
    //Create tileRequest
    //Task1 : (BlockRequest, Tile) base level tile generation : generate individual pyramid tiles
    //Task2 : (Tile) Bookeeper : receive the pyramid tile and cache it. Generate request for higher level when it can.
    //Task3 : (Block, Tile) CreateDownscaledTile : create tile at higher level of the pyramid.
    //Task 4 : write the Tile;


    uint32_t numberBlockHeight,numberBlockWidth = 0;

    numberBlockHeight = ceil((double)numTileRow/2);
    numberBlockWidth = ceil((double)numTileCol/2);

    //we traverse the grid in blocks to minimize memory footprint of the pyramid generation.
    for(uint32_t j = 0; j < numberBlockHeight; j++){
        for(uint32_t i = 0; i < numberBlockWidth; i++){
            if(2*i < numTileCol && 2*j < numTileRow) {
                std::cout << 2*j << "," << 2*i << std::endl;
                auto tileRequest = new TileRequest(2 * j, 2 * i);
                graph->produceData(tileRequest);
            }
            if(2*i+1 < numTileCol) {
                std::cout << 2 * j << "," << 2 * i + 1 << std::endl;
                auto tileRequest = new TileRequest(2 * j, 2 * i + 1);
                graph->produceData(tileRequest);
            }

            if(2*j+1 < numTileRow) {
                std::cout << 2 * j + 1 << "," << 2 * i << std::endl;
                auto tileRequest = new TileRequest(2 * j + 1, 2 * i);
                graph->produceData(tileRequest);
            }

            if(2*j+1 < numTileRow && 2*i+1 < numTileCol) {
                std::cout << 2 * j + 1 << "," << 2 * i + 1 << std::endl;
                auto tileRequest = new TileRequest(2 * j + 1, 2 * i + 1);
                graph->produceData(tileRequest);
            }
        }
    }


    graph->finishedProducingData();

    while(!graph->isOutputTerminated()){
        auto r = graph->consumeData();
        if(r == nullptr){
            break;
        }
        std::cout << "output : " << r->getLevel() << ": " << r->getRow() << "," << r->getCol() << std::endl;
    }

    std::cout << "we should be done" << std::endl;
    runtime->waitForRuntime();

    delete runtime;

}





