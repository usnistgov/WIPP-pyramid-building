//
// Created by Gerardin, Antoine D. (Assoc) on 3/4/19.
//

#ifndef PYRAMIDBUILDING_FOVCACHE_H
#define PYRAMIDBUILDING_FOVCACHE_H


#include <string>
#include <map>
#include <mutex>
#include <glog/logging.h>
#include <tiffio.h>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

#include <experimental/filesystem>
#include <atomic>
#include "TileLoader.h"

namespace pb {

using namespace std::experimental;

template <class T>
class FOVCache {


public:

    //number of FOVs
    std::atomic<uint32_t> readCount;
    TileLoader<T>* tileLoader;

    FOVCache(std::string directory, std::map<std::string, uint32_t> fovsUsageCount) : _directory(directory), fovsUsageCount(fovsUsageCount) {
        tileLoader = new TileLoader<T>(directory);
    };

    ~FOVCache() {

        assert(fovsCache.size() == 0);
        assert(fovsUsageCount.size() == 0);

        for (auto it = fovsCache.begin(); it!=fovsCache.end(); ++it) {
            fovsCache.erase(it);
        }

        for (auto it = fovsUsageCount.begin(); it!=fovsUsageCount.end(); ++it) {
            fovsUsageCount.erase(it);
        }

        delete tileLoader;
    }

    T* getFOV(const std::string &filename){

        std::lock_guard<std::mutex> guard(lock);

        T* fov = nullptr;

        auto it = fovsCache.find(filename);
        if(it == fovsCache.end()) {
            readCount++;
            VLOG(3) << "FOV not already loaded : " << filename << std::endl;
            fov = tileLoader->loadFullFOV(filename);
            fovsCache[filename] = fov;
            if(fovsCache.size() > cacheMaxCount) {cacheMaxCount = fovsCache.size();}
        }
        else{
            VLOG(3) << "FOV already loaded : " << filename <<  std::endl;
            fov = it->second;
        }

        return fov;
    }


    uint32_t releaseFOV(std::string filename) {
        std::lock_guard<std::mutex> guard(lockCount);

        auto count = --fovsUsageCount[filename];

        if(count == 0) {
            VLOG(3) << "delete " << filename << std::endl;
            auto it = fovsCache.find(filename);
            if(it == fovsCache.end()) {
                std::runtime_error("error in cache implementation " + filename);
            }
            delete [] it->second;
            fovsCache.erase(it);

            auto it2 = fovsUsageCount.find(filename);
            if(it2 == fovsUsageCount.end()) {
                std::runtime_error("error in cache implementation " + filename);
            }
            fovsUsageCount.erase(it2);
        }

        return count;
    }

    size_t getCacheCount(){
        return fovsCache.size();
    }

    size_t getCacheMaxCount(){
        return cacheMaxCount;
    }


private:

    size_t cacheMaxCount = 0;

    std::mutex lockCount;

    //TODO CHANGE. We are making a copy! Check how to store a reference.
    std::map<std::string, uint32_t> fovsUsageCount;

    std::string _directory;

    std::map<std::string, T*> fovsCache;

    std::mutex lock;


};


}

#endif //PYRAMIDBUILDING_FOVCACHE_H
