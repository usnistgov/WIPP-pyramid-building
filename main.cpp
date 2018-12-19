#include <iostream>
#include <FastImage/api/FastImage.h>
#include <FastImage/TileLoaders/GrayscaleTiffTileLoader.h>
#include "Helper.h"

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


    auto
            numTileRow = fi->getNumberTilesWidth(0),
            numTileCol = fi->getNumberTilesHeight(0);


    std::cout << numTileRow << "," << numTileCol << std::endl;

    fi->configureAndRun();

    for(auto i = 0; i <= ceil(numTileRow/2); i++){
        for(auto j = 0; j <= ceil(numTileCol/2); j++){
            std::cout << 2*i << "," << 2*j << std::endl;
            std::cout << 2*i << "," << 2*j+1 << std::endl;
            std::cout << 2*i+1 << "," << 2*j << std::endl;
            std::cout << 2*i+1 << "," << 2*j+1 << std::endl;


            fi->requestTile(i,j,false,0);
        }
    }

    fi->finishedRequestingTiles();

    int32_t tileWidth = 0;
    int32_t tileHeight = 0;
    uint32_t* data;


    while(fi->isGraphProcessingTiles()){
        auto view = fi->getAvailableViewBlocking();
        if(view != nullptr){
             view->get()->getTileHeight();
             view->get()->getTileWidth();
             data = view->get()->getData();
             std::cout << data[0] << std::endl;
             view->releaseMemory();
        }
    }


    std::cout << "Hello, World!" << std::endl;


    delete fi;


    return 0;
}

