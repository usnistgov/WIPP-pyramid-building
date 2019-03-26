//
// Created by Gerardin, Antoine D. (Assoc) on 1/19/19.
//

#ifndef PYRAMIDBUILDING_BASETILEGENERATORLIBTIFFWITHCACHE_H
#define PYRAMIDBUILDING_BASETILEGENERATORLIBTIFFWITHCACHE_H

#include <string>
#include <iostream>
#include <map>
#include <assert.h>
#include "../data/PartialFov.h"
#include "Helper.h"
#include <FastImage/api/FastImage.h>
#include <FastImage/TileLoaders/GrayscaleTiffTileLoader.h>
#include "GridGenerator.h"
#include "../data/Tile.h"
#include "pyramidBuilding/api/OptionsType.h"
#include <experimental/filesystem>
#include "FOVCache.h"
#include "Blender.h"
#include "BaseTileGenerator.h"

namespace pb {

    using namespace std::experimental;

/**
 * @class BaseTileGenerator BaseTileGenerator.h
 * @brief Generate pyramid base level tile.
 * @details
 * The pyramid base level is composed of tiles carved from a set of overlapping FOVs.
 * For a given pyramid tile size, we can generate a grid structure : (row,col) -> vector of partial overlapping FOVs.
 * We can then used this information to generate each tile.
 */
    template<class T>
    class BaseTileGeneratorLibTiffWithCache  : public BaseTileGenerator<T>  {

    public:

        /**
         * The pyramid base level tile generator needs information on the general structure of the full FOVs.
         * @param reader the MistStitchedImageReader that contains information on partial FOV overlaps.
         */
        BaseTileGeneratorLibTiffWithCache(GridGenerator *reader, BlendingMethod blendingMethod) : grid(reader->getGrid()), directory(
                reader->getImageDirectoryPath()), tileWidth(
                reader->getFovTileWidth()), tileHeight(reader->getFovTileHeight()), pyramidTileSize(
                reader->getPyramidTileSize()),
                                                                                  fullFovWidth(
                                                                                          reader->getFullFovWidth()),
                                                                                  fullFovHeight(
                                                                                          reader->getFullFovHeight()),
                                                                                  maxGridCol(reader->getGridMaxCol()),
                                                                                  maxGridRow(reader->getGridMaxRow()),
                                                                                  blendingMethod(blendingMethod),
                                                                                  fovWidth(reader->getFovWidth()),
                                                                                  fovHeight(reader->getFovHeight()) {
            fovsCache = new FOVCache<T>(reader->getImageDirectoryPath(), reader->getCache());
            blender = new Blender<T>(blendingMethod);
        }

        ~BaseTileGeneratorLibTiffWithCache() {
            delete fovsCache;
            delete blender;
        }

        /**
         * Generate a pyramid base level tile at a specific coordinates.
         * @param index (row,col) of the tile to generate.
         * @return
         */
        Tile<T> *generateTile(std::pair<size_t, size_t> index) override {

            VLOG(2) << "generating base tile at index (" << index.first << "," << index.second << ")" << std::endl;

            auto row = index.first;
            auto col = index.second;

            size_t pyramidTileWidth = (index.second != maxGridCol) ? pyramidTileSize : fullFovWidth -
                                                                                       col * pyramidTileSize;
            size_t pyramidTileHeight = (index.first != maxGridRow) ? pyramidTileSize : fullFovHeight -
                                                                                       row * pyramidTileSize;
            T *tile = new T[pyramidTileWidth *
                            pyramidTileHeight]();  //the pyramid tile we will be filling from partial FOVs.



            auto it = grid.find(index);

            //Dealing with corner case.
            //It should never happen with real data, but we might have missing tiles or have so much overlap between FOVs that
            //some gap appears in the image. If this is the case, we generate an empty tile.
            if (it == grid.end()) {
                DLOG(WARNING) << "A gap was found in the grid of tiles at (" + std::to_string(index.first) + "," +
                                 std::to_string(index.second) + "). Generating a empty tile." << std::endl;
                return new Tile<T>(0, index.first, index.second, pyramidTileWidth, pyramidTileHeight, tile);
            }

            std::vector<PartialFov *> fovs = it->second;


            //iterating over each partial FOV.
            for (auto it2 = fovs.begin(); it2 != fovs.end(); ++it2) {

                auto fov = *it2;
                auto filename = fov->getPath();
                auto extension = getFileExtension(filename);

                if (extension != "tiff" && extension != "tif") {
                    throw std::runtime_error("File Format not recognized: " + extension );
                }

                auto overlapFov = fov->getFovCoordOverlap();
                auto tileOverlap = fov->getTileOverlap();

                T *image = fovsCache->getFOV(filename);

                VLOG(3) << "fovCacheCount :  " + std::to_string(fovsCache->getCacheCount()) << std::endl;
                VLOG(3) << " max fovCacheCount :  " + std::to_string(fovsCache->getCacheMaxCount()) << std::endl;

                auto destOffset = tileOverlap.y * pyramidTileWidth + tileOverlap.x;

                for (auto y = overlapFov.y; y < overlapFov.y + overlapFov.height; y++) {
                    //copy each row of the partial FOV overlap to correct position in the pyramid tile.
                    std::copy_n(image + y * fovWidth + overlapFov.x, overlapFov.width, tile + destOffset + (y - overlapFov.y) * pyramidTileWidth);
                }

                fovsCache->releaseFOV(fov->getPath());

            } //DONE generating the pyramid tile

            VLOG(2) << "Base tile generated (" << index.first << "," << index.second << ")" << std::endl;

            return new Tile<T>(0, index.first, index.second, pyramidTileWidth, pyramidTileHeight, tile);
        }


        //TODO remove for DEBUG ONLY
        FOVCache<T> *getFovsCache() const {
            return fovsCache;
        }


    private:
        FOVCache<T> *fovsCache;
        const std::map<std::pair<size_t, size_t>, std::vector<PartialFov *>> grid;
        const std::string directory;
        const size_t tileWidth;
        const size_t tileHeight;
        const size_t pyramidTileSize;
        const size_t fullFovWidth;
        const size_t fullFovHeight;
        const size_t maxGridRow;
        const size_t maxGridCol;
        const BlendingMethod blendingMethod;
        const size_t fovWidth;
        const size_t fovHeight;
        Blender<T> *blender;


    };

}

#endif //PYRAMIDBUILDING_BASETILEGENERATORLIBTIFFWITHCACHE_H
