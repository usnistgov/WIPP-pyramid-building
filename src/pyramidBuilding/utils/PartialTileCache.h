//
// Created by Gerardin, Antoine D. (Assoc) on 4/8/19.
//

#ifndef PYRAMIDBUILDING_TILECACHE_H
#define PYRAMIDBUILDING_TILECACHE_H

#include <string>
#include <map>
#include <mutex>
#include <glog/logging.h>
#include <tiffio.h>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

#include <experimental/filesystem>
#include <atomic>
#include "TiffImageLoader.h"

namespace pb {

    using namespace std::experimental;

    template <class T>
    class PartialTileCache {


    public:

        //number of FOVs
        std::atomic<uint32_t> readCount;
        TiffImageLoader<T>* imageLoader;

        PartialTileCache(std::string directory, std::map<std::string, uint32_t> fovsUsageCount) : _directory(directory) {
            imageLoader = new TiffImageLoader<T>(directory);
        };

        ~PartialTileCache() {
            delete imageLoader;
        }

        T* getTile(const std::string &filename){

            std::lock_guard<std::mutex> guard(lock);

            T* tile = nullptr;

            auto it = tileCache.find(filename);
            if(it == tileCache.end()) {
                readCount++;
                VLOG(3) << "FOV not already loaded : " << filename << std::endl;
                size_t pyramidTileWidth = (index.second != maxGridCol) ? pyramidTileSize : fullFovWidth -
                                                                                           col * pyramidTileSize;
                size_t pyramidTileHeight = (index.first != maxGridRow) ? pyramidTileSize : fullFovHeight -
                                                                                           row * pyramidTileSize;
                T *tile = new T[pyramidTileWidth *
                                pyramidTileHeight]();  //the pyramid tile we will be filling from partial FOVs.
                tileCache[filename] = tile;
                if(tileCache.size() > cacheMaxCount) {cacheMaxCount = tileCache.size();}
            }
            else{
                VLOG(3) << "FOV already loaded : " << filename <<  std::endl;
                tile = it->second;
            }

            return tile;
        }


        uint32_t releaseTile(std::string filename) {
            return 0;
        }

        size_t getCacheCount(){
            return tileCache.size();
        }

        size_t getCacheMaxCount(){
            return cacheMaxCount;
        }


    private:

        size_t cacheMaxCount = 0;

        std::mutex lockCount;


        std::string _directory;

        std::map<std::string, T*> tileCache;

        std::mutex lock;


    };


}

#endif //PYRAMIDBUILDING_TILECACHE_H
