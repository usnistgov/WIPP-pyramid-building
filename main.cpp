#include <iostream>
#include <FastImage/api/FastImage.h>
#include <FastImage/TileLoaders/GrayscaleTiffTileLoader.h>
#include "src/utils/Helper.h"
#include "src/rules/WriteTileRule.h"
#include "src/tasks/WriteTileTask.h"
#include "src/rules/PyramidRule.h"
#include "src/tasks/CreateTileTask.h"
#include "src/utils/MatrixAllocator.h"
#include "src/data/Tile.h"
#include <htgs/log/TaskGraphSignalHandler.hpp>

int main() {

   // std::string pathImage = "/Users/gerardin/Documents/projects/pyramidio/pyramidio/src/test/resources/dataset2/images/tiled-pc/tiled_stitched_c01t020p1.ome.tif";
   // std::string pathImage ="/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/tiledMaison.tiff";
    std::string pathImage ="/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/circleTiled.tiff";
  //  std::string pathImage ="/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/tiled16_tracer.tiff";

    std::string tiffOut ="/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/tiffOut.tiff";


    fi::ATileLoader<uint32_t> *tileLoader = nullptr;

    auto extension = Helper::getExtension(pathImage);
    if (extension == "tiff" || extension == "tif") {
        tileLoader = new fi::GrayscaleTiffTileLoader<uint32_t>(pathImage, 10);
    } else {
        std::cout << "File Format not recognized !" << std::endl;
    }

    // Create the fast image with the tile loader
    auto *fi = new fi::FastImage<uint32_t>(tileLoader, 0);

    fi->getFastImageOptions()->setNumberOfViewParallel(4);

    auto
            numTileRow = fi->getNumberTilesHeight(0),
            numTileCol = fi->getNumberTilesWidth(0);



    std::cout << numTileRow << "," << numTileCol << std::endl;

    auto graph = new htgs::TaskGraphConf<Tile<uint32_t>, Tile<uint32_t> >();

    auto bookeeper = new htgs::Bookkeeper<Tile<uint32_t>>();

    auto writeRule = new WriteTileRule();

    auto pyramidRule = new PyramidRule(numTileCol,numTileRow);

    auto writeTask = new WriteTileTask(tiffOut.c_str() , fi->getImageWidth(), fi->getImageHeight(), fi->getTileWidth(), fi->getTileHeight());
    auto createTileTask = new CreateTileTask();

    graph->setGraphConsumerTask(bookeeper);
    graph->addEdge(createTileTask,bookeeper);
    graph->addRuleEdge(bookeeper, writeRule, writeTask);
    graph->addRuleEdge(bookeeper, pyramidRule, createTileTask);
    graph->addGraphProducerTask(writeTask);
//    auto matAlloc = new FakeTileAllocator();
//    graph->addMemoryManagerEdge("PYRAMID_TILE", createTileTask, matAlloc, 4, htgs::MMType::Static);

    htgs::TaskGraphRuntime *runtime = new htgs::TaskGraphRuntime(graph);

    htgs::TaskGraphSignalHandler::registerTaskGraph(graph);
    htgs::TaskGraphSignalHandler::registerSignal(SIGTERM   );

    runtime->executeRuntime();


    fi->configureAndRun();

    uint32_t numberBlockHeight,numberBlockWidth = 0;

    numberBlockHeight = ceil((double)numTileRow/2);
    numberBlockWidth = ceil((double)numTileCol/2);

    for(auto i = 0; i < numberBlockHeight; i++){
        for(auto j = 0; j < numberBlockWidth; j++){
            if(2*j < numTileCol && 2*i < numTileRow) {
//                std::cout << 2*i << "," << 2*j << std::endl;
                fi->requestTile(2 * i, 2 * j, false, 0);
            }
            if(2*j+1 < numTileCol) {
//                std::cout << 2 * i << "," << 2 * j + 1 << std::endl;
                fi->requestTile(2 * i, 2 * j + 1, false, 0);
            }
            if(2*i+1 < numTileRow) {
//                std::cout << 2 * i + 1 << "," << 2 * j << std::endl;
                fi->requestTile(2 * i + 1, 2 * j, false, 0);
            }
            if(2*j+1 < numTileCol && 2*i+1 < numTileRow) {
//                std::cout << 2 * i + 1 << "," << 2 * j + 1 << std::endl;
                fi->requestTile(2 * i + 1, 2 * j + 1, false, 0);
            }

        }
    }


    fi->finishedRequestingTiles();




    int32_t tileWidth = 0;
    int32_t tileHeight = 0;
    uint32_t* data;

    while(fi->isGraphProcessingTiles()) {
        auto view = fi->getAvailableViewBlocking();
        if(view != nullptr){
      //      view->get()->getTileHeight();
    //        view->get()->getTileWidth();
  //          data = view->get()->getData();
//            std::cout << data[0] << std::endl;
            auto tile = new Tile<uint32_t>(view);
            graph->produceData(tile);
        }

    }

    graph->finishedProducingData();

    while(!graph->isOutputTerminated()){
        auto r = graph->consumeData();
        if(r == nullptr){
            break;
        }
        std::cout << r->getLevel() << ": " << r->getRow() << "," << r->getCol() << std::endl;
    }

    std::cout << "we should be done" << std::endl;
    runtime->waitForRuntime();

    delete fi;
    delete runtime;

}

