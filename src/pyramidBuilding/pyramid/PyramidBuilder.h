//
// Created by Gerardin, Antoine D. (Assoc) on 4/9/19.
//

#ifndef PYRAMIDBUILDING_TILEREQUESTBUILDER_H
#define PYRAMIDBUILDING_TILEREQUESTBUILDER_H

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

#include <map>
#include <assert.h>
#include <glog/logging.h>
#include <pyramidBuilding/api/OptionsType.h>
#include <pyramidBuilding/data/FOVMetadata.h>
#include <pyramidBuilding/pyramid/data/PartialFOV.h>
#include <pyramidBuilding/pyramid/data/TileRequest.h>
#include <pyramidBuilding/api/PyramidBuilding.h>
#include <pyramidBuilding/data/Pyramid.h>

namespace pb {

    class PyramidBuilder {


    private:

        std::string imageDirectoryPath;
        std::string stitchingVectorPath;
        uint32_t pyramidTileSize;


        uint32_t fullFovWidth = 0;
        uint32_t fullFovHeight = 0;

        uint32_t maxRow = 0;
        uint32_t maxCol = 0;

        std::shared_ptr<FOVMetadata> fovMetadata = nullptr;
        std::map<std::pair<size_t,size_t>, TileRequest*> tileRequests = {};
        Pyramid pyramid;



    public:
        /**
         * Build the problem representation we will use to stitch tile at the base level.
         * @param imageDirectoryPath where to locate the FOVs.
         * @param stitchingVectorPath  where to locate the corresponding stitching vector.
         */
        PyramidBuilder(const std::string &imageDirectoryPath,
                                 const std::string &stitchingVectorPath, uint32_t pyramidTileSize) :
                imageDirectoryPath(imageDirectoryPath),
                stitchingVectorPath(stitchingVectorPath),
                pyramidTileSize(pyramidTileSize) {

            std::ifstream infile(stitchingVectorPath);

            if(infile.peek() == std::ifstream::traits_type::eof()) {
                throw std::runtime_error("empty stitching vector file or error reading the stitching vector.");
            }


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
                                throw std::runtime_error("position coordinates cannot be converted to 32bits signed integer");
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

                        std::ostringstream err;
                        err << "unable to open tiff file:" << imageDirectoryPath <<  filename << ". Please check that the stitching vector matches the image folder.";
                        throw std::runtime_error(err.str());
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

                    //make_unique not available on Clang
                    fovMetadata = std::make_shared<FOVMetadata>(width, height, samplePerPixel, bitsPerSample, sampleFormat, imageDirectoryPath);
                    fovMetadata->setTileWidth(tileWidth);
                    fovMetadata->setTileHeight(tileHeight);
                }

                uint32_t colMin = fovGlobalX / pyramidTileSize;
                uint32_t rowMin = fovGlobalY / pyramidTileSize;
                uint32_t colMax = (fovGlobalX + fovMetadata->getWidth() - 1) / pyramidTileSize;
                uint32_t rowMax = (fovGlobalY + fovMetadata->getHeight() - 1) / pyramidTileSize;

                for(auto tileCol = colMin; tileCol <= colMax; tileCol++){
                    for (auto tileRow = rowMin; tileRow <= rowMax; tileRow++){

                        std::pair<size_t,size_t> index = std::make_pair(tileRow,tileCol);

                        auto startX = std::max(tileCol * pyramidTileSize, fovGlobalX);
                        auto startY = std::max(tileRow * pyramidTileSize, fovGlobalY);
                        auto endX = std::min(fovGlobalX + fovMetadata->getWidth(), (tileCol + 1) * pyramidTileSize);
                        auto endY = std::min(fovGlobalY + fovMetadata->getHeight(), (tileRow + 1) * pyramidTileSize);

                        auto width = endX - startX;
                        auto height = endY - startY;

                        auto fovOverlap = new PartialFOV::Overlap(startX - fovGlobalX, startY - fovGlobalY, width, height);


                        auto tileOverlapX = startX - tileCol * pyramidTileSize;
                        auto tileOverlapY = startY - tileRow * pyramidTileSize;

                        auto tileOverlap = new PartialFOV::Overlap(tileOverlapX,tileOverlapY, width, height);

                        auto fov = new PartialFOV(filename, fovOverlap, tileOverlap);

                        auto it = tileRequests.find(index);

                        if(it != tileRequests.end()) {
                            it->second->getFovs().push_back(fov);
                        }
                        else {
                            std::vector<PartialFOV*> fovs({fov});
                            tileRequests.insert(std::make_pair(index, new TileRequest(tileRow, tileCol, fovs)));
                        }
                    }
                }
            }

            infile.close();

            //dimensions of the fullFOV
            fullFovWidth = maxFovGlobalX + fovMetadata->getWidth();
            fullFovHeight = maxFovGlobalY + fovMetadata->getHeight();

            fovMetadata->setFullFovWidth(fullFovWidth);
            fovMetadata->setFullFovHeight(fullFovHeight);
            fovMetadata->setMaxRow(maxRow);
            fovMetadata->setMaxCol(maxCol);

            pyramid = Pyramid(fullFovWidth,fullFovHeight, pyramidTileSize);

            VLOG(3) << "parsing info : " << std::endl;
            VLOG(3) << "fov grid max row : " << maxRow << std::endl;
            VLOG(3) << "fov grid max col : " << maxCol << std::endl;
            VLOG(3) << "full fov width : " << fullFovWidth << std::endl;
            VLOG(3) << "full fov height : " << fullFovHeight << std::endl;
        }


        ~PyramidBuilder(){
            for(auto tileRequestEntry : tileRequests){
                delete tileRequestEntry.second;
            }
            tileRequests.clear();
            fovMetadata.reset();
        }


        std::shared_ptr<FOVMetadata> getFovMetadata() const {
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

        const std::map<std::pair<size_t, size_t>, TileRequest *> &getTileRequests() const {
            return tileRequests;
        }

        const Pyramid &getPyramid() const {
            return pyramid;
        }
    };

}

#endif //PYRAMIDBUILDING_TILEREQUESTBUILDER_H
