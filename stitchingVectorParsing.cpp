//
// Created by Gerardin, Antoine D. (Assoc) on 1/2/19.
//


#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <regex>
#include "data/Fov.h"
//#include <opencv2/opencv.hpp>


int main() {
    std::ifstream infile("/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset1/stitching_vector/img-global-positions-1.txt");

    //full image dim
    uint32_t imageWidth = 0;
    uint32_t imageHeight = 0;

    //partial images
    uint32_t fovGridRowSize = 0;
    uint32_t fovGridColSize = 0;
    uint32_t fovWidth = 0;
    uint32_t fovHeight = 0;

    //pyramid
    uint32_t tileSize = 0;
    uint32_t gridRowSize = 0;
    uint32_t gridColSize = 0;

    //for each tile in the grid, we record the subregion of each intersecting FOVs
    // Map (row,col) -> list of partial images (rectangle)

    std::string line;
    std::string pair;

    std::vector<Fov*> fovs({});



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

        Fov *fov = new Fov(file, x,y);
        fovs.push_back(fov);
    }

    std::cout << "done" << std::endl;



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
