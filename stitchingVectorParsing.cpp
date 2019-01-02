//
// Created by Gerardin, Antoine D. (Assoc) on 1/2/19.
//


#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <regex>

#define uint64 uint64_hack_
#define int64 int64_hack_
#include <tiffio.h>
#undef uint64
#undef int64

#include "data/Fov.h"
#include <opencv/cv.h>

#include <map>


int main() {
    std::ifstream infile("/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset1/stitching_vector/img-global-positions-1.txt");

    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset1/images/";

    std::map<std::pair<uint32_t,uint32_t> , std::vector<cv::Rect>> grid;

    //full image dim
    uint32_t imageWidth = 0;
    uint32_t imageHeight = 0;

    //partial images
    uint32_t fovGridRowSize = 0;
    uint32_t fovGridColSize = 0;
    uint32_t fovWidth = 0;
    uint32_t fovHeight = 0;

    //pyramid
    uint32_t tileSize = 256;
    uint32_t gridRowSize = 0;
    uint32_t gridColSize = 0;

    //for each tile in the grid, we record the subregion of each intersecting FOVs
    // Map (row,col) -> list of partial images (rectangle)

    std::string line;
    std::string pair;

    std::vector<Fov*> fovs({});
    std::vector<cv::Rect> rects({});



    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        uint32_t x,y;
        std::string file;
        while(std::getline(iss,pair,';')) {
            std::string key, val;
            std::istringstream iss2(pair);
            while( std::getline(std::getline(iss2 >> std::ws , key, ':') >> std::ws, val)) {
                std::cout << key << "||||||" << val << std::endl;

               if(key  == "position") {
                   std::regex rgx("\\(([0-9]+), ([0-9]+)\\)");
                   std::smatch matches;
                   if(std::regex_search(val, matches, rgx)) {
                       x = std::stoi(matches[1].str());
                       y = std::stoi(matches[2].str());
                   } else {
                       return -1;
                   }
                 }
                 else if( key == "file"){
                     file = val;
                 }
            }
        }

        if(fovWidth == 0 || fovHeight == 0){
            TIFF *tiff = TIFFOpen((directory+file).c_str(), "r");
            TIFFGetField(tiff,TIFFTAG_IMAGEWIDTH,&fovWidth);
            TIFFGetField(tiff,TIFFTAG_IMAGELENGTH,&fovHeight);
        }

        cv::Rect fov = cv::Rect(x, y, fovWidth, fovHeight);

        uint32_t startX, startY, endX, endY = 0;
        startX = x / tileSize;
        startY =  y / tileSize;
        endX = (x + fovWidth) / tileSize;
        endY = (y + fovHeight) / tileSize;

        uint32_t relativeX;
        uint32_t relativeY;

        for(uint32_t i = startX; i <= endX ; i++) {
            for(uint32_t j = startY; j <= endY ; j++) {
                cv::Rect tile = cv::Rect(i * tileSize, j * tileSize, tileSize, tileSize);
                cv::Rect intersection = tile & fov;
                relativeX = intersection.x - i*tileSize;
                relativeY = intersection.y - j*tileSize;
                cv::Rect tileFOV = cv::Rect(relativeX, relativeY, intersection.width, intersection.height);

                //add to a list for a map entry at key (i,j)
                //FOr now we add to a vector;
                rects.push_back(tileFOV);

                std::pair<uint32_t,uint32_t> index= std::make_pair(i,j);

                auto it = grid.find(index);

                if(it != grid.end()) {
                    it->second.push_back(tileFOV);
                }
                else {
                    std::vector<cv::Rect> tileFovs({tileFOV});
                    grid.insert(std::make_pair(index, tileFovs));
                }
            }
        }



        Fov *fov2 = new Fov(file, x,y);
        fovs.push_back(fov2);
    }

    std::cout << "done" << std::endl;

    //read the first tile to get the tile width and height

    //for each fov divide by grid size to figure in which tile it appears.
    //add subregion to each tile.

    //calculate image width and height (accumulating in the loop).

    //calculate grid size




//    Mat image;
//    image = imread(, CV_LOAD_IMAGE_ANYDEPTH);   // Read the file
//
//    if(! image.data )                              // Check for invalid input
//    {
//        cout <<  "Could not open or find the image" << std::endl ;
//        return -1;
//    }
//
//    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
//    imshow( "Display window", image );                   // Show our image inside it.
//
//    waitKey(0);                                          // Wait for a keystroke in the window
//    return 0;




// process pair (a,b)
}
