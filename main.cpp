#include <iostream>
#include <FastImage/api/FastImage.h>
#include <FastImage/TileLoaders/GrayscaleTiffTileLoader.h>
#include "Helper.h"
#include "rules/WriteTileRule.h"
#include "tasks/WriteTileTask.h"
#include "rules/PyramidRule.h"
#include "tasks/CreateTileTask.h"

int main() {

    std::string pathImage = "/Users/gerardin/Documents/projects/pyramidio/pyramidio/src/test/resources/dataset2/images/tiled-pc/tiled_stitched_c01t020p1.ome.tif";

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

    auto graph = new htgs::TaskGraphConf<htgs::MemoryData<fi::View<uint32_t>>, htgs::VoidData >();

    auto bookeeper = new htgs::Bookkeeper< htgs::MemoryData<fi::View<uint32_t>> >();

    auto writeRule = new WriteTileRule();

    auto pyramidRule = new PyramidRule(numTileCol,numTileRow);

    auto writeTask = new WriteTileTask();
    auto createTileTask = new CreateTileTask();

    graph->setGraphConsumerTask(bookeeper);
    graph->addRuleEdge(bookeeper, writeRule, writeTask);
    graph->addRuleEdge(bookeeper, pyramidRule, createTileTask);

    htgs::TaskGraphRuntime *runtime = new htgs::TaskGraphRuntime(graph);

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
            view->get()->getTileHeight();
            view->get()->getTileWidth();
            data = view->get()->getData();
//            std::cout << data[0] << std::endl;
            // view->releaseMemory();
            graph->produceData(view);

        }
    }

    graph->finishedProducingData();

    delete fi;

    runtime->waitForRuntime();

    delete runtime;


    return 0;
}

