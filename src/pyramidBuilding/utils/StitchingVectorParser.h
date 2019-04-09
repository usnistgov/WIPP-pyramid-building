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

        std::map<std::pair<size_t,size_t> , FOV*> gridFOVs;
        FOVMetadata* fovMetadata = nullptr;

        uint32_t fullFovWidth = 0;
        uint32_t fullFovHeight = 0;

        uint32_t FOVGridMaxRow = 0;
        uint32_t FOVGridMaxCol = 0;

    public:
        /**
         *
         * @param imageDirectoryPath where to locate the FOVs
         * @param stitchingVectorPath  where to locate the corresponding stitching vector.
         */
        StitchingVectorParser(const std::string &imageDirectoryPath,
                                 const std::string &stitchingVectorPath) :
                imageDirectoryPath(imageDirectoryPath),
                stitchingVectorPath(stitchingVectorPath) {

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

                                if (col > FOVGridMaxCol) FOVGridMaxCol = col;
                                if (row > FOVGridMaxRow) FOVGridMaxRow = row;
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

                    uint32_t width, height, tileWidth, tileHeight;
                    short samplePerPixel, bitsPerSample, sampleFormat;

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

                    fovMetadata = new FOVMetadata(width, height, samplePerPixel, bitsPerSample, sampleFormat);
                    fovMetadata->setTileWidth(tileWidth);
                    fovMetadata->setTileHeight(tileHeight);

                    //store as static member so it can found whenever we need it
                    FOV::setMetadata(fovMetadata);
                }

                //Coordinates are inversed to keep consistency => (row,col)
                std::pair<size_t,size_t> index= std::make_pair(row,col);
                auto fov = new FOV(filename,row,col,fovGlobalX,fovGlobalY);
                gridFOVs.insert(std::make_pair(index,fov));
            }

            //dimensions of the fullFOV
            fullFovWidth = maxFovGlobalX + fovMetadata->getWidth();
            fullFovHeight = maxFovGlobalY + fovMetadata->getHeight();

            infile.close();
        }

        //TODO CHECK we destroyed this properly
        ~StitchingVectorParser(){
            for(auto &elt : gridFOVs){
                delete elt.second;
            }
            gridFOVs.clear();
        }
    };

}

#endif //PYRAMIDBUILDING_STITCHINGVECTORPARSER_H
