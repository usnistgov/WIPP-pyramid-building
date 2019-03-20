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
#include <assert.h>
#include <glog/logging.h>

#include "../data/PartialFov.h"
#include "../api/Datatype.h"


namespace pb {

    //TODO CHECK Following the previous Java implementation, we expect all FOVs to have the same size and the same tile size.
    //Lifiting this assumption would mean we would need to open each file to get each FOV size.
    //The best would then be to modified the stitching vector to contain this information already.
    /**
     * @class MistStitchedImageReader MistStitchedImageReader.h
     * @brief Parse the MIST stitching vector representing overlapping FOVs.
     * @details
     * The pyramid base level is composed of tiles carved from a set of overlapping FOVs.
     * For a given pyramid tile size, we can generate a grid structure : (row,col) -> vector of partial overlapping FOVs.
     */
    class GridGenerator {

    private:
        size_t pyramidTileSize;
        std::string imageDirectoryPath;
        std::string stitchingVectorPath;

        std::map<std::pair<size_t,size_t> , std::vector<PartialFov*>> grid;

        size_t fovWidth = 0;
        size_t fovHeight = 0;
        size_t fovTileWidth = 0;
        size_t fovTileHeight = 0;
        size_t fullFovWidth = 0;
        size_t fullFovHeight = 0;

        size_t gridMaxRow = 0;
        size_t gridMaxCol = 0;

        std::map<std::string, uint32_t> cache;

        size_t numLevel = 0;
        uint32_t counter = 0;



    public:

        //TODO test that directory and vector are not mixed up.
        //TODO CHECK we cast all values to size_t
        /**
         *
         * @param imageDirectoryPath where to locate the FOVs
         * @param stitchingVectorPath  where to locate the corresponding stitching vector.
         * @param pyramidTileSize size of pyramid tile.
         */
        GridGenerator(const std::string &imageDirectoryPath,
                                const std::string &stitchingVectorPath, size_t pyramidTileSize) :
                                                                          imageDirectoryPath(imageDirectoryPath),
                                                                          stitchingVectorPath(stitchingVectorPath),
                                                                          pyramidTileSize(pyramidTileSize) {
            //inputs
            //TODO CHECK THAT THE FILE EXISTS (otherwise it will silently break)
            std::ifstream infile(stitchingVectorPath);
            size_t tileSize = pyramidTileSize;

            //partial images (fovs)
            size_t fovGlobalX = 0;
            size_t fovGlobalY = 0;

            //fullFOV dim
            size_t maxFovGlobalX = 0;
            size_t maxFovGlobalY = 0;

            //parse stitching vector
            std::string line;
            std::string pair;

            VLOG(2) << "parsing stitching vector...";

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
                        VLOG(3) << key << ": " << val << std::endl;

                        if(key  == "position") {
                            std::regex rgx("\\(([0-9]+), ([0-9]+)\\)");
                            std::smatch matches;
                            if(std::regex_search(val, matches, rgx)) {
                                fovGlobalX = std::strtoul(matches[1].str().data(), nullptr, 10);
                                fovGlobalY = std::strtoul(matches[2].str().data(), nullptr, 10);

                                if(fovGlobalX > maxFovGlobalX) maxFovGlobalX = fovGlobalX;
                                if(fovGlobalY > maxFovGlobalY) maxFovGlobalY = fovGlobalY;

                                //TODO CHECK best function to parse stitching vector coords (spec?)
    //                            fovGlobalX = static_cast<size_t>(std::stoi(matches[1].str()));
    //                            fovGlobalY = static_cast<size_t>(std::stoi(matches[2].str()));
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

                fullFovWidth = maxFovGlobalX + fovWidth;
                fullFovHeight = maxFovGlobalY + fovHeight;

                //TODO CHECK openCV only accepts int, float or double. Should we keep size_t or int, uint32?
                //Definitely should use uint32 if we use openCV.
                cv::Rect fov = cv::Rect(fovGlobalX, fovGlobalY, fovWidth, fovHeight);

                //Find out across how many pyramid tiles span the current FOV
                size_t startCol, startRow, endCol, endRow = 0;
                startCol = fovGlobalX / tileSize;
                startRow =  fovGlobalY / tileSize;
                //TODO CHECK no ceil?
                endCol = (fovGlobalX + fovWidth - 1) / tileSize; // -1 because we need the pixel col index
                endRow = (fovGlobalY + fovHeight - 1) / tileSize;

                uint32_t count = ((endCol - startCol) + 1) * (endRow - startRow + 1);
                cache.insert({file, count});

                counter += count;

                //compute overlap between an FOV and each pyramid tile.

                for(size_t j = startRow; j <= endRow ; j++) {
                    for(size_t i = startCol; i <= endCol ; i++) {
                        cv::Rect tile = cv::Rect(i * tileSize, j * tileSize, tileSize, tileSize); //tile global coordinates

                        //global coordinates
                        cv::Rect intersection = tile & fov;

                        assert(intersection.width != 0 || intersection.height != 0);

                        size_t relativeX = 0;
                        size_t relativeY = 0;
                        size_t fovRelativeX = 0;
                        size_t fovRelativeY = 0;

                        //coordinates relative to the tile origin
                        relativeX = intersection.x - i*tileSize;
                        relativeY = intersection.y - j*tileSize;
                        cv::Rect overlapInTileRef = cv::Rect(relativeX, relativeY, intersection.width, intersection.height);

                        //coordinates relative to the FOV origin
                        fovRelativeX = intersection.x - fovGlobalX;
                        fovRelativeY = intersection.y - fovGlobalY;
                        cv::Rect overlapInFovRef = cv::Rect(fovRelativeX, fovRelativeY, intersection.width, intersection.height);

                        auto *partialFov = new PartialFov(file, fovGlobalX,fovGlobalY, overlapInTileRef, intersection, overlapInFovRef);

                        assert(overlapInTileRef.width == overlapInFovRef.width);
                        assert(overlapInTileRef.height == overlapInFovRef.height);
                        assert(overlapInTileRef.x >= 0);
                        assert(overlapInTileRef.y >= 0);
                        assert(overlapInFovRef.x >= 0);
                        assert(overlapInFovRef.y >= 0);

                        // add the partial FOV to the corresponding pyramid tile.
                        std::pair<size_t,size_t> index= std::make_pair(j,i);
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


                auto maxDim = std::max(gridMaxRow + 1,gridMaxCol + 1);
                numLevel = static_cast<size_t>(ceil(log2(maxDim)) + 1);

            }
        }


        ~GridGenerator(){
          for(auto &elt : grid){
              for(auto &fov : elt.second){
                  delete fov;
              }
              elt.second.clear();
          }
        }




        const std::map<std::pair<size_t, size_t>, std::vector<PartialFov *>> &getGrid() const {
            return grid;
        }

        size_t getFovWidth() const {
            return fovWidth;
        }

        size_t getFovHeight() const {
            return fovHeight;
        }

        size_t getFovTileWidth() const {
            return fovTileWidth;
        }

        size_t getFovTileHeight() const {
            return fovTileHeight;
        }

        size_t getFullFovWidth() const {
            return fullFovWidth;
        }

        size_t getFullFovHeight() const {
            return fullFovHeight;
        }


        size_t getFullFovWidthAtLevel(size_t level) const{
            auto levelWidth = static_cast<size_t>(ceil((double)fullFovWidth / pow(2, level)));
            return levelWidth;
        }


        size_t getFullFovHeightAtLevel(size_t level) const{
            auto levelHeight = static_cast<size_t>(ceil((double)fullFovHeight / pow(2, level)));
            return levelHeight;
        }


        size_t getGridMaxRow(size_t level = 0) const {
            return static_cast<size_t>(ceil(getFullFovHeightAtLevel(level) / pyramidTileSize));
        }

        size_t getGridMaxCol(size_t level = 0) const {
            return static_cast<size_t>(ceil(getFullFovWidthAtLevel(level) / pyramidTileSize));
        }

        const std::string &getImageDirectoryPath() const {
            return imageDirectoryPath;
        }

        size_t getPyramidTileSize() const {
            return pyramidTileSize;
        }

        const std::map<std::string, uint32_t> &getCache() const {
            return cache;
        }

        uint32_t getCounter() const {
            return counter;
        }

        size_t getNumLevel() const {
            return numLevel;
        }
    };


}

#endif //PYRAMIDBUILDING_MISTSTITCHEDIMAGEREADER_H
