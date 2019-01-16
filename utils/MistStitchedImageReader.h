//
// Created by Gerardin, Antoine D. (Assoc) on 1/2/19.
//

#ifndef PYRAMIDBUILDING_MISTSTITCHEDIMAGEREADER_H
#define PYRAMIDBUILDING_MISTSTITCHEDIMAGEREADER_H

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

#include <opencv/cv.h>
#include <map>

#include "../data/Fov.h"
#include <assert.h>

class MistStitchedImageReader {



private:
    uint32_t pyramidTileSize;
    std::string imageDirectoryPath;
    std::string stitchingVectorPath;
    std::map<std::pair<uint32_t,uint32_t> , std::vector<Fov*>> grid;

    uint32_t fovWidth = 0;
    uint32_t fovHeight = 0;
    uint32_t fovTileWidth = 0;
    uint32_t fovTileHeight = 0;



public:
    /**
     * @class MistStitchedImageReader MistStitchedImageReader.h
     * @brief Create pyramid tiles from a set of overlapping partial images.
     * @details A pyramid is composed at level 0 of tiles carved from a set of overlapping FOVs.
     * The structure is stored in a grid : (row,col) -> vector of partial images (openCV rectangles)
     */
    MistStitchedImageReader(const std::string &imageDirectoryPath,
                            const std::string &stitchingVectorPath, uint32_t pyramidTileSize) :
                                                                      imageDirectoryPath(imageDirectoryPath),
                                                                      stitchingVectorPath(stitchingVectorPath),
                                                                      pyramidTileSize(pyramidTileSize) {
        //inputs
        std::ifstream infile(stitchingVectorPath);
        uint32_t tileSize = pyramidTileSize;


        //full image dim
        uint32_t imageWidth = 0;
        uint32_t imageHeight = 0;

        //partial images (fovs)
        uint32_t fovGlobalX = 0;
        uint32_t fovGlobalY = 0;


        //pyramid
        //TODO NOT USED FOR NOW
        uint32_t gridRowSize = 0;
        uint32_t gridColSize = 0;


        std::vector<Fov*> fovs({});
        std::vector<cv::Rect> rects({});


        //parse stitching vector
        std::string line;
        std::string pair;
        while (std::getline(infile, line)) {
            std::istringstream iss(line);

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
                            fovGlobalX = std::stoi(matches[1].str());
                            fovGlobalY = std::stoi(matches[2].str());
                        } else {
                            throw "position coordinates cannot be converted to 4bytes signed integer";
                        }
                    }
                    else if( key == "file"){
                        file = val;
                    }
                }
            }

            //we check the first FOV to retrieve its width and height.
            //TODO NOTE we expect all FOVs to have the same size and the same tile size.
            if(fovWidth == 0 || fovHeight == 0 || fovTileWidth == 0 || fovTileHeight == 0){
                TIFF *tiff = TIFFOpen((imageDirectoryPath+file).c_str(), "r");
                TIFFGetField(tiff,TIFFTAG_IMAGEWIDTH,&fovWidth);
                TIFFGetField(tiff,TIFFTAG_IMAGELENGTH,&fovHeight);
                TIFFGetField(tiff, TIFFTAG_TILEWIDTH, &fovTileWidth);
                TIFFGetField(tiff, TIFFTAG_TILELENGTH, &fovTileHeight);
                TIFFClose(tiff);
            }

            cv::Rect fov = cv::Rect(fovGlobalX, fovGlobalY, fovWidth, fovHeight);

            //Find out across how many pyramid tiles span the current FOV
            uint32_t startCol, startRow, endCol, endRow = 0;
            startCol = fovGlobalX / (tileSize -1);
            startRow =  fovGlobalY / (tileSize -1);
            endCol = (fovGlobalX + fovWidth) / (tileSize -1);
            endRow = (fovGlobalY + fovHeight) / (tileSize -1);

            uint32_t relativeX;
            uint32_t relativeY;
            uint32_t fovRelativeX;
            uint32_t fovRelativeY;

            //compute overlap between an FOV and each pyramid tile.
            for(uint32_t i = startCol; i <= endCol ; i++) {
                for(uint32_t j = startRow; j <= endRow ; j++) {
                    cv::Rect tile = cv::Rect(i * tileSize, j * tileSize, tileSize, tileSize); //tile global coordinates

                    //global coordinates
                    cv::Rect intersection = tile & fov;

                    //coordinates relative to the tile origin
                    relativeX = intersection.x - i*tileSize;
                    relativeY = intersection.y - j*tileSize;
                    cv::Rect overlapInTileRef = cv::Rect(relativeX, relativeY, intersection.width, intersection.height);

                    //coordinates relative to the FOV origin
                    fovRelativeX = intersection.x - fovGlobalX;
                    fovRelativeY = intersection.y - fovGlobalY;
                    cv::Rect overlapInFovRef = cv::Rect(fovRelativeX, fovRelativeY, intersection.width, intersection.height);



                    Fov *fov3 = new Fov(file, fovGlobalX,fovGlobalY, overlapInTileRef, intersection, overlapInFovRef);


                    assert(overlapInTileRef.width == overlapInFovRef.width);
                    assert(overlapInTileRef.height == overlapInFovRef.height);

                    //add to a list for a map entry at key (i,j)
                    //For now we add to a vector;
                    rects.push_back(overlapInTileRef);

                    std::pair<uint32_t,uint32_t> index= std::make_pair(i,j);


                    auto it = grid.find(index);

                    if(it != grid.end()) {
                        it->second.push_back(fov3);
                    }
                    else {
                        std::vector<Fov*> tileFovs({fov3});
                        grid.insert(std::make_pair(index, tileFovs));
                    }
                }
            }


            Fov *fov2 = new Fov(file, fovGlobalX,fovGlobalY);
            fovs.push_back(fov2);
        }

        std::cout << "done" << std::endl;
    }



    const std::map<std::pair<uint32_t, uint32_t>, std::vector<Fov *>> &getGrid() const {
        return grid;
    }

    uint32_t getFovWidth() const {
        return fovWidth;
    }

    uint32_t getFovHeight() const {
        return fovHeight;
    }

    uint32_t getFovTileWidth() const {
        return fovTileWidth;
    }

    uint32_t getFovTileHeight() const {
        return fovTileHeight;
    }

};

#endif //PYRAMIDBUILDING_MISTSTITCHEDIMAGEREADER_H
