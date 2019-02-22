#include <iostream>
#include <FastImage/api/FastImage.h>
#include <FastImage/TileLoaders/GrayscaleTiffTileLoader.h>
#include "src/utils/Helper.h"
#include "src/rules/WriteTileRule.h"
#include "src/tasks/Write8UPngTileTask.h"
#include "src/rules/PyramidRule.h"
#include "src/tasks/CreateTileTask.h"
#include "src/tasks/BaseTileTask.h"
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
#include "src/tasks/WriteDeepZoomTileTask.h"
#include "src/rules/DeepZoomDownsamplingRule.h"
#include <string>
#include <sstream>

#define DEBUG(x) do { std::cerr << x << std::endl; } while (0)

typedef uint8_t px_t;
int main() {

    // Run the example surrounding with a chrono
    auto begin = std::chrono::high_resolution_clock::now();

//    std::string vector = "/home/gerardin/Documents/images/dataset2/img-global-positions-1.txt";
//    std::string directory = "/home/gerardin/Documents/images/dataset2/images/";

//    std::string vector = "/home/gerardin/Documents/images/dataset5_big/img-global-positions-1.txt";
//    std::string directory = "/home/gerardin/Documents/images/dataset5_big/images/";

//    std::string vector = "/home/gerardin/Documents/images/dataset4/img-global-positions-1.txt";
//    std::string directory = "/home/gerardin/Documents/images/dataset4/images/";
//
//    std::string vector = "/home/gerardin/Documents/pyramidBuilding/resources/dataset03/stitching_vector/img-global-positions-1.txt";
//    std::string directory = "/home/gerardin/Documents/pyramidBuilding/resources/dataset03/images/";


//    std::string vector = "/home/gerardin/Documents/pyramidBuilding/resources/dataset1/stitching_vector/img-global-positions-1.txt";
//    std::string directory = "/home/gerardin/Documents/pyramidBuilding/resources/dataset1/tiled-images/";

    std::string vector = "/Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset1/stitching_vector/img-global-positions-1.txt";
    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset1/tiled-images/";

//    std::string vector = "/home/gerardin/Documents/pyramidBuilding/resources/dataset02/stitching_vector/img-global-positions-1.txt";
//    std::string directory = "/home/gerardin/Documents/pyramidBuilding/resources/dataset02/images/";

//    std::string vector = "/Users/gerardin/Documents/projects/wipp++/pyramidBuildingCleanup/resources/dataset01/stitching_vector/img-global-positions-1.txt";
//    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuildingCleanup/resources/dataset01/images/";

    //pyramid
   // size_t pyramidTileSize = 1024;
//TODO inform user if wrong tile size
   size_t pyramidTileSize = 256;
//  size_t pyramidTileSize = 16;

    int overlap = 0;

    std::string format = "png";

    std::string pyramidName = "output";

    auto gridGenerator = new GridGenerator(directory, vector, pyramidTileSize);

    auto grid = gridGenerator->getGrid();

    //TODO CHECK we assume that all FOV have the same tiling scheme.

#ifndef NDEBUG
    auto tileWidth = gridGenerator->getFovTileWidth();
    auto tileHeight = gridGenerator->getFovTileHeight();
    //TODO CHECK we assume all tiles are square. This is not necessary but it is safe to assume for the first tests.
    assert(tileWidth == tileHeight);
#endif



    //TODO CHECK we could assume for now that pyramid tile size is a multiple of the underlying FOV tile size.
    //Will that be of any use?
    //assert(pyramidTileSize % tileWidth == 0);


    size_t numTileRow = gridGenerator->getGridMaxRow() + 1;
    size_t numTileCol = gridGenerator->getGridMaxCol() + 1;


    size_t fullFovWidth = gridGenerator->getPyramidBaseWidth();
    size_t fullFovHeight = gridGenerator->getPyramidBaseHeight();
    int deepZoomLevel = 0;
    //calculate pyramid depth
    auto maxDim = std::max(fullFovWidth,fullFovHeight);
    deepZoomLevel = int(ceil(log2(maxDim)) + 1);


    auto graph = new htgs::TaskGraphConf<TileRequest, Tile<px_t>>();

    auto generator = new BaseTileGenerator<px_t>(gridGenerator);
    auto baseTileTask = new BaseTileTask<px_t>(10, generator);

    auto bookkeeper = new htgs::Bookkeeper<Tile<px_t>>();

    auto writeRule = new WriteTileRule<px_t>();

    auto pyramidRule = new PyramidRule<px_t>(numTileCol,numTileRow);

    auto deepzoomDownsamplingRule = new DeepZoomDownsamplingRule<px_t>(numTileCol,numTileRow,deepZoomLevel, pyramidName + "_files");

    auto createTileTask = new CreateTileTask<px_t>(10);

    auto writeTask = new WriteDeepZoomTileTask<px_t>(10, pyramidName + "_files",deepZoomLevel);

    graph->setGraphConsumerTask(baseTileTask);

    //incoming edges from the bookeeper
    graph->addEdge(baseTileTask, bookkeeper); //pyramid base level tile
    graph->addEdge(createTileTask,bookkeeper); //pyramid higher level tile

    //outgoing edges
    graph->addRuleEdge(bookkeeper, pyramidRule, createTileTask); //caching tiles and creating a tile at higher level;

    //TODO CHECK for now we link to the writeTask but do not use it. We could. If large latency in write, it could be worthwhile. Otherwise thread management will dominate.
    graph->addRuleEdge(bookkeeper, deepzoomDownsamplingRule, writeTask); //caching tiles and creating a tile at higher level;
    graph->addRuleEdge(bookkeeper, writeRule, writeTask); //exiting the graph;

    //output task
    graph->addGraphProducerTask(writeTask);

//    auto matAlloc = new FakeTileAllocator();
//    graph->addMemoryManagerEdge("PYRAMID_TILE", createTileTask, matAlloc, 4, htgs::MMType::Static);

    auto *runtime = new htgs::TaskGraphRuntime(graph);

    htgs::TaskGraphSignalHandler::registerTaskGraph(graph);
    htgs::TaskGraphSignalHandler::registerSignal(SIGTERM);

    runtime->executeRuntime();

    //HTGS Graph
    //Create tileRequest
    //Task1 : (BlockRequest, Tile) base level tile generation : generate individual pyramid tiles
    //Task2 : (Tile) Bookeeper : receive the pyramid tile and cache it. Generate request for higher level when it can.
    //Task3 : (Block, Tile) CreateDownscaledTile : create tile at higher level of the pyramid.
    //Task 4 : write the Tile;


    size_t numberBlockHeight,numberBlockWidth = 0;

    numberBlockHeight = static_cast<size_t>(ceil((double)numTileRow/2));
    numberBlockWidth = static_cast<size_t>(ceil((double)numTileCol/2));

    //we traverse the grid in blocks to minimize memory footprint of the pyramid generation.
    for(size_t j = 0; j < numberBlockHeight; j++){
        for(size_t i = 0; i < numberBlockWidth; i++){
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
      //  std::cout << "output : " << r->getLevel() << ": " << r->getRow() << "," << r->getCol() << std::endl;
    }

    //'<?xml version="1.0" encoding="utf-8"?><Image TileSize="254" Overlap="1" Format="jpg" xmlns="http://schemas.microsoft.com/deepzoom/2008"><Size Width="13920" Height="10200"/></Image>';

    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"utf-8\"?><Image TileSize=\"" << pyramidTileSize << "\" Overlap=\"" << overlap
    << "\" Format=\""  << format << "\" xmlns=\"http://schemas.microsoft.com/deepzoom/2008\"><Size Width=\""
    << gridGenerator->getPyramidBaseWidth() << "\" Height=\"" << gridGenerator->getPyramidBaseHeight() << "\"/></Image>";

    std::ofstream outFile;
    outFile.open(pyramidName +".dzi");
    outFile << oss.str();
    outFile.close();



    std::cout << "we should be done" << std::endl;
    runtime->waitForRuntime();

    graph->writeDotToFile("graph", DOTGEN_FLAG_SHOW_ALL_THREADING | DOTGEN_COLOR_COMP_TIME);

    delete runtime;
    delete gridGenerator;
    delete generator;

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " mS" << std::endl;

}





