//
// Created by Gerardin, Antoine D. (Assoc) on 4/9/19.
//

#ifndef PYRAMIDBUILDING_STITCHINGVECTORPARSER_H
#define PYRAMIDBUILDING_STITCHINGVECTORPARSER_H

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
#include <assert.h>
#include <glog/logging.h>

#include "../api/OptionsType.h"
#include "../data/PartialFov.h"
#include "../data/FOVMetadata.h"
#include "../data/FOV.h"

namespace pb {

    class StitchingVectorParser {


    private:

        std::string imageDirectoryPath;
        std::string stitchingVectorPath;
        uint32_t pyramidTileSize;

        std::map<std::pair<size_t,size_t>, FOV*> grid;
        std::map<std::pair<size_t,size_t>, u_int8_t> fovUsageCount = {};
        std::map<std::pair<size_t,size_t>, std::vector<std::pair<size_t,size_t>>> fovUsage = {};

        FOVMetadata* fovMetadata = nullptr;

        uint32_t fullFovWidth = 0;
        uint32_t fullFovHeight = 0;

        uint32_t maxRow = 0;
        uint32_t maxCol = 0;

        u_int8_t maxFovUsage = 0;



    public:
        /**
         *
         * @param imageDirectoryPath where to locate the FOVs
         * @param stitchingVectorPath  where to locate the corresponding stitching vector.
         */
        StitchingVectorParser(const std::string &imageDirectoryPath,
                                 const std::string &stitchingVectorPath, uint32_t pyramidTileSize) :
                imageDirectoryPath(imageDirectoryPath),
                stitchingVectorPath(stitchingVectorPath),
                pyramidTileSize(pyramidTileSize) {

            std::ifstream infile(stitchingVectorPath);

            uint32_t row = 0;
            uint32_t col = 0;

            uint32_t fovGlobalX = 0;
            uint32_t fovGlobalY = 0;

            uint32_t maxFovGlobalX = 0;
            uint32_t maxFovGlobalY = 0;

            //parse stitching vector
            std::string line;
            std::string pair;

            std::string filename;

            VLOG(2) << "parsing stitching vector...";

            while (std::getline(infile, line)) {
                std::istringstream iss(line);
                if(line == ""){
                    continue;
                }
                while(std::getline(iss,pair,';')) {
                    std::string key, val;
                    std::istringstream iss2(pair);
                    while (std::getline(std::getline(iss2 >> std::ws, key, ':') >> std::ws, val)) {
                        VLOG(3) << key << ": " << val << std::endl;

                        if (key == "position") {
                            std::regex rgx("\\(([0-9]+), ([0-9]+)\\)");
                            std::smatch matches;
                            if (std::regex_search(val, matches, rgx)) {
                                fovGlobalX = (uint32_t)std::strtoul(matches[1].str().data(), nullptr, 10);
                                fovGlobalY = (uint32_t)std::strtoul(matches[2].str().data(), nullptr, 10);

                                if (fovGlobalX > maxFovGlobalX) maxFovGlobalX = fovGlobalX;
                                if (fovGlobalY > maxFovGlobalY) maxFovGlobalY = fovGlobalY;
                            } else {
                                throw "position coordinates cannot be converted to 32bits signed integer";
                            }
                        } else if (key == "file") {
                            filename = val;
                        } else if (key == "grid") {
                            std::regex rgx("\\(([0-9]+), ([0-9]+)\\)");
                            std::smatch matches;
                            if (std::regex_search(val, matches, rgx)) {
                                col = (uint32_t)std::strtoul(matches[1].str().data(), nullptr, 10);
                                row = (uint32_t)std::strtoul(matches[2].str().data(), nullptr, 10);

                                if (col > maxCol) maxCol = col;
                                if (row > maxRow) maxRow = row;
                            }
                        }
                    }
                }


                //we check the first FOV to retrieve its width and height.
                //IMPORTANT we expect all FOVs to have the same size and the same tile size.
                if(fovMetadata == nullptr){

                    TIFF *tiff = TIFFOpen((imageDirectoryPath + filename).c_str(), "r");
                    if(tiff == nullptr){
                        std::string errorMsg = "unable to open tiff file: " + imageDirectoryPath + filename +
                                               ". Please check that the stitching vector matches the image folder.";
                        throw std::runtime_error(errorMsg);
                    }

                    uint32_t width = 0, height = 0, tileWidth = 0, tileHeight = 0;
                    short samplePerPixel = 0, bitsPerSample = 0, sampleFormat = 0;

                    // Load/parse header
                    // For full reference, see https://www.awaresystems.be/imaging/tiff/tifftags.html
                    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
                    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
                    TIFFGetField(tiff, TIFFTAG_TILEWIDTH, &tileWidth);
                    TIFFGetField(tiff, TIFFTAG_TILELENGTH, &tileHeight);
                    TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &samplePerPixel);
                    TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
                    TIFFGetField(tiff, TIFFTAG_SAMPLEFORMAT, &sampleFormat);
                    TIFFClose(tiff);

                    if(tileWidth == 0 || tileHeight == 0){
                        throw std::runtime_error("Unsupported file format. Images should be tiled tiff.");
                    }

                    fovMetadata = new FOVMetadata(width, height, samplePerPixel, bitsPerSample, sampleFormat, imageDirectoryPath);
                    fovMetadata->setTileWidth(tileWidth);
                    fovMetadata->setTileHeight(tileHeight);
                }

                //Coordinates are inversed to keep consistency => (row,col)
                std::pair<size_t,size_t> index= std::make_pair(row,col);
                auto fov = new FOV(filename,row,col,fovGlobalX,fovGlobalY, fovMetadata);
                grid.insert(std::make_pair(index,fov));

                uint32_t colMin = fovGlobalX / pyramidTileSize;
                uint32_t rowMin = fovGlobalY / pyramidTileSize;
                uint32_t colMax = (fovGlobalX + fovMetadata->getWidth() - 1) / pyramidTileSize;
                uint32_t rowMax = (fovGlobalY + fovMetadata->getHeight() - 1) / pyramidTileSize;

                for(auto tileCol = colMin; tileCol <= colMax; tileCol++){
                    for (auto tileRow = rowMin; tileRow <= rowMax; tileRow++){
                        fovUsageCount[{tileRow,tileCol}] += 1;
                        if(fovUsageCount[{tileRow,tileCol}] > maxFovUsage){ maxFovUsage = fovUsageCount[{tileRow,tileCol}];}
                        fovUsage[{tileRow,tileCol}].push_back({row,col});
                    }
                }
            }

            //dimensions of the fullFOV
            fullFovWidth = maxFovGlobalX + fovMetadata->getWidth();
            fullFovHeight = maxFovGlobalY + fovMetadata->getHeight();

            fovMetadata->setFullFovWidth(fullFovWidth);
            fovMetadata->setFullFovHeight(fullFovHeight);
            fovMetadata->setMaxRow(maxRow);
            fovMetadata->setMaxCol(maxCol);

            VLOG(3) << "parsing info : " << std::endl;
            VLOG(3) << "fov grid max row : " << maxRow << std::endl;
            VLOG(3) << "fov grid max col : " << maxCol << std::endl;
            VLOG(3) << "full fov width : " << fullFovWidth << std::endl;
            VLOG(3) << "full fov height : " << fullFovHeight << std::endl;

            infile.close();
        }


        const std::map<std::pair<size_t, size_t>, FOV *> &getGrid() const {
            return grid;
        }

        FOVMetadata *getFovMetadata() const {
            return fovMetadata;
        }

        uint32_t getFullFovWidth() const {
            return fullFovWidth;
        }

        uint32_t getFullFovHeight() const {
            return fullFovHeight;
        }

        uint32_t getMaxRow() const {
            return maxRow;
        }

        uint32_t getMaxCol() const {
            return maxCol;
        }

        std::map<std::pair<size_t, size_t>, u_int8_t> &getFovUsageCount() {
            return fovUsageCount;
        }

        u_int8_t getMaxFovUsage() const {
            return maxFovUsage;
        }

        const std::map<std::pair<size_t, size_t>, std::vector<std::pair<size_t, size_t>>> &getFovUsage() const {
            return fovUsage;
        }

        uint32_t getPyramidTileSize() const {
            return pyramidTileSize;
        }

        //FOV are passed to the graph as shared_ptr and are destroyed when the FOV data is copied to each overlapping
        //tile. Thus no call to destructor is necessary
        ~StitchingVectorParser(){
            grid.clear();
            delete fovMetadata;
            fovMetadata = nullptr;
        }

    };

}

#endif //PYRAMIDBUILDING_STITCHINGVECTORPARSER_H
