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

#include "../data/PartialFov.h"
#include <assert.h>

//TODO CHECK we expect all FOVs to have the same size and the same tile size.
/**
 * @class MistStitchedImageReader MistStitchedImageReader.h
 * @brief Parse the MIST stitching vector representing overlapping FOVs.
 * @details
 * The pyramid base level is composed of tiles carved from a set of overlapping FOVs.
 * For a given pyramid tile size, we can generate a grid structure : (row,col) -> vector of partial overlapping FOVs.
 */
class StitchingVectorParser {


private:
    uint32_t pyramidTileSize;
    std::string imageDirectoryPath;
    std::string stitchingVectorPath;

    std::map<std::pair<uint32_t,uint32_t> , std::vector<PartialFov*>> grid;

    uint32_t fovWidth = 0;
    uint32_t fovHeight = 0;
    uint32_t fovTileWidth = 0;
    uint32_t fovTileHeight = 0;

    uint32_t gridMaxRow = 0;
    uint32_t gridMaxCol = 0;

    uint32_t gridSize;



public:

    //TODO test that directory and vector and not mixed up.
    //TODO CHECK we cast fovGlobalX and fovGlobalY into signed 32bits integer. Enough?
    //TODO CHECK we cast most values to 32bits integers.
    /**
     *
     * @param imageDirectoryPath where to locate the FOVs
     * @param stitchingVectorPath  where to locate the corresponding stitching vector.
     * @param pyramidTileSize size of pyramid tile.
     */
    StitchingVectorParser(const std::string &imageDirectoryPath,
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
        int32_t fovGlobalX = 0;
        int32_t fovGlobalY = 0;

        //parse stitching vector
        std::string line;
        std::string pair;
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            if(line == ""){
                continue;
            }
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
            //TODO CHECK we expect all FOVs to have the same size and the same tile size.
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
            startCol = fovGlobalX / tileSize;
            startRow =  fovGlobalY / tileSize;
            endCol =  (fovGlobalX + fovWidth - 1) / tileSize;
            endRow = (fovGlobalY + fovHeight - 1) / tileSize;

            //compute overlap between an FOV and each pyramid tile.

            for(uint32_t j = startRow; j <= endRow ; j++) {
                for(uint32_t i = startCol; i <= endCol ; i++) {
                    cv::Rect tile = cv::Rect(i * tileSize, j * tileSize, tileSize, tileSize); //tile global coordinates

                    //global coordinates
                    cv::Rect intersection = tile & fov;

                    assert(intersection.width != 0 || intersection.height != 0);

                    int32_t relativeX = 0;
                    int32_t relativeY = 0;
                    int32_t fovRelativeX = 0;
                    int32_t fovRelativeY = 0;

                    //coordinates relative to the tile origin
                    relativeX = intersection.x - i*tileSize;
                    relativeY = intersection.y - j*tileSize;
                    cv::Rect overlapInTileRef = cv::Rect(relativeX, relativeY, intersection.width, intersection.height);

                    //coordinates relative to the FOV origin
                    fovRelativeX = intersection.x - fovGlobalX;
                    fovRelativeY = intersection.y - fovGlobalY;
                    cv::Rect overlapInFovRef = cv::Rect(fovRelativeX, fovRelativeY, intersection.width, intersection.height);

                    PartialFov *partialFov = new PartialFov(file, fovGlobalX,fovGlobalY, overlapInTileRef, intersection, overlapInFovRef);

                    assert(overlapInTileRef.width == overlapInFovRef.width);
                    assert(overlapInTileRef.height == overlapInFovRef.height);
                    assert(overlapInTileRef.x >= 0);
                    assert(overlapInTileRef.y >= 0);
                    assert(overlapInFovRef.x >= 0);
                    assert(overlapInFovRef.y >= 0);

                    // add the partial FOV to the corresponding pyramid tile.
                    std::pair<uint32_t,uint32_t> index= std::make_pair(j,i);
                    auto it = grid.find(index);

                    if(it != grid.end()) {
                        it->second.push_back(partialFov);
                    }
                    else {
                        std::vector<PartialFov*> tileFovs({partialFov});
                        grid.insert(std::make_pair(index, tileFovs));

                        if(j > gridMaxRow){
                            gridMaxRow = j;
                        }
                        if(i > gridMaxCol){
                            gridMaxCol = i;
                        }
                    }
                }
            }

        }
    }


    const std::map<std::pair<uint32_t, uint32_t>, std::vector<PartialFov *>> &getGrid() const {
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

    uint32_t getFullFovWidth() const {
        return gridMaxCol * fovWidth;
    }

    uint32_t getFullFovHeight() const {
        return gridMaxRow * fovHeight;
    }

    uint32_t getGridMaxRow() const {
        return gridMaxRow;
    }

    uint32_t getGridMaxCol() const {
        return gridMaxCol;
    }


    const std::string &getImageDirectoryPath() const {
        return imageDirectoryPath;
    }

    uint32_t getPyramidTileSize() const {
        return pyramidTileSize;
    }

};

#endif //PYRAMIDBUILDING_MISTSTITCHEDIMAGEREADER_H
