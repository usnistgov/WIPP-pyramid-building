//
// Created by Gerardin, Antoine D. (Assoc) on 1/23/19.
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


//TODO openCV does not support 32 bits unsigned integers. We force them into signed 32 bits which potential bugs for value out of scope.
//TODO pyrdown supports only an even samller subset so now we have to convert to 8bits unsigned! Quite unusable.
/**
 *
 *  Test downscaling using OpenCV
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


        uint32_t *tile1 = tiles[0];
        uint32_t *tile2 = tiles[1];
        uint32_t *tile3 = tiles[2];
        uint32_t *tile4 = tiles[3];


        cv::Mat small_image0(32, 32, CV_32SC1, tile1);
        cv::Mat small_image1(32, 32, CV_32SC1, tile2);
        cv::Mat small_image2(32, 32, CV_32SC1, tile3);
        cv::Mat small_image3(32, 32, CV_32SC1, tile4);

        std::cout << "M = "<< std::endl << " "  << small_image0 << std::endl << std::endl;
        std::cout << "M = "<< std::endl << " "  << small_image1 << std::endl << std::endl;
        std::cout << "M = "<< std::endl << " "  << small_image2 << std::endl << std::endl;
        std::cout << "M = "<< std::endl << " "  << small_image3 << std::endl << std::endl;

        //TODO CHECK 32 signed?
        cv::Mat big_image(cv::Size(2 * pyramidTileSize, 2 * pyramidTileSize), CV_32SC1);
        small_image0.copyTo(big_image(cv::Rect(0,0,small_image0.cols, small_image0.rows)));
        small_image1.copyTo(big_image(cv::Rect(32,0,small_image1.cols, small_image1.rows)));
        small_image2.copyTo(big_image(cv::Rect(0,32,small_image2.cols, small_image2.rows)));
        small_image3.copyTo(big_image(cv::Rect(32,32,small_image3.cols, small_image3.rows)));

        cv::Mat tmp(32, 32, CV_8UC1);
        big_image.convertTo(tmp, CV_8UC1);
        cv::Mat final_image(32, 32, CV_8UC1);
        cv::pyrDown( tmp, final_image, cv::Size( big_image.cols/2, big_image.rows/2 ));


        std::cout << "final  = "<< std::endl << " "  << final_image << std::endl << std::endl;

        cv::imwrite("alpha.png", final_image);
};


