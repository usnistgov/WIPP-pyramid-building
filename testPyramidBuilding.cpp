#include <iostream>
#include <FastImage/api/FastImage.h>
#include <FastImage/TileLoaders/GrayscaleTiffTileLoader.h>
#include "src/utils/Helper.h"
#include "src/rules/WriteTileRule.h"
#include "src/tasks/WriteTileTask.h"
#include "src/rules/PyramidRule.h"
#include "src/tasks/CreateTileTask.h"
#include "src/tasks/BaseTileTask.h"
#include "src/tasks/OutputTask.h"
#include "src/utils/MatrixAllocator.h"
#include "src/data/Tile.h"
#include <htgs/log/TaskGraphSignalHandler.hpp>
#define uint64 uint64_hack_
#define int64 int64_hack_
#include <tiffio.h>
#undef uint64
#undef int64
#include <assert.h>
#include "src/data/TileRequest.h"
#include "src/utils/SingleTiledTiffWriter.h"
#include "src/utils/GridGenerator.h"
#include "src/utils/BaseTileGenerator.h"

#define DEBUG(x) do { std::cerr << x << std::endl; } while (0)


Tile<uint32_t>* generateTile(uint32_t i, uint32_t j, std::map<std::pair<uint32_t, uint32_t>, std::vector<PartialFov *>> &grid, BaseTileGenerator *generator, std::string directory) {
    std::pair<uint32_t,uint32_t> index= std::make_pair(i,j);
    auto it = grid.find(index);
    assert(it != grid.end());
    Tile<uint32_t>* tile = generator->generateTile(index);
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

    // Run the example surrounding with a chrono
    auto begin = std::chrono::high_resolution_clock::now();

//    std::string vector = "/Users/gerardin/Documents/projects/pyramidio/pyramidio/src/test/resources/dataset2/stitching_vector/tiled-pc/img-global-positions-1.txt";
//    std::string directory = "/Users/gerardin/Documents/projects/pyramidio/pyramidio/src/test/resources/dataset2/images/tiled-pc/";


    std::string vector = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset1/stitching_vector/img-global-positions-1.txt";
    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset1/tiled-images/";

//    std::string vector = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset02/stitching_vector/img-global-positions-1.txt";
//    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset02/images/";

//    std::string vector = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset01/stitching_vector/img-global-positions-1.txt";
//    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset01/images/";

    //pyramid
    uint32_t pyramidTileSize = 256;
//   uint32_t pyramidTileSize = 32;

    auto gridGenerator = new GridGenerator(directory, vector, pyramidTileSize);

    auto grid = gridGenerator->getGrid();

    //TODO CHECK we assume that all FOV have the same tiling scheme.
    auto tileWidth = gridGenerator->getFovTileWidth();
    auto tileHeight = gridGenerator->getFovTileHeight();

    //TODO CHECK we assume all tiles are square. This is not necessary but it is safe to assume for the first tests.
    assert(tileWidth == tileHeight);

    //TODO CHECK we could assume for now that pyramid tile size is a multiple of the underlying FOV tile size.
    //Will that be of any use?
    //assert(pyramidTileSize % tileWidth == 0);


    uint32_t numTileRow = gridGenerator->getGridMaxRow() + 1;
    uint32_t numTileCol = gridGenerator->getGridMaxCol() + 1;

    auto graph = new htgs::TaskGraphConf<TileRequest, Tile<uint32_t> >();

    BaseTileGenerator* generator = new BaseTileGenerator(gridGenerator);
    auto baseTileTask = new BaseTileTask(10, generator);

    auto bookeeper = new htgs::Bookkeeper<Tile<uint32_t>>();

    auto writeRule = new WriteTileRule();

    auto pyramidRule = new PyramidRule(numTileCol,numTileRow);

    auto createTileTask = new CreateTileTask(10);

    auto writeTask = new WriteTileTask(24, "output", pyramidTileSize);

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
    htgs::TaskGraphSignalHandler::registerSignal(SIGTERM);

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
    delete gridGenerator;
    delete generator;

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << " mS" << std::endl;

}





