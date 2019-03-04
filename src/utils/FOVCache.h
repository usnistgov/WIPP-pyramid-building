//
// Created by Gerardin, Antoine D. (Assoc) on 3/4/19.
//

#ifndef PYRAMIDBUILDING_FOVCACHE_H
#define PYRAMIDBUILDING_FOVCACHE_H


#include <string>
#include <map>
#include <mutex>
#include <FastImage/api/FastImage.h>
#include <FastImage/TileLoaders/GrayscaleTiffTileLoader.h>

template <class T>
class FOVCache {


public:

    FOVCache(std::string directory, std::map<std::string, uint32_t> fovsUsageCount) : _directory(directory), fovsUsageCount(fovsUsageCount) {};

    fi::FastImage<T> * getFOVReader(std::string filename){

        std::lock_guard<std::mutex> guard(lock);
        std::cout << "Inside Lock Getting FI: " << filename << std::endl;


        fi::FastImage<T> *fi = nullptr;

        auto it = fovsCache.find(filename);
        if(it == fovsCache.end()) {
            std::cout << "FI not already loaded" << std::endl;
            fi::ATileLoader<T> *tileLoader = new fi::GrayscaleTiffTileLoader<T>(_directory + filename, 1);
            fi = new fi::FastImage<T>(tileLoader, 0);
            fi->getFastImageOptions()->setNumberOfViewParallel(2500);
            fi->configureAndRun();
            fovsCache[filename] = fi;
        }
        else{

            std::cout << "FI already loaded" << std::endl;
            fi = it->second;
        }

        return fi;
    }


    uint32_t releaseFOVReader(fi::FastImage<T> * fi, std::string filename) {
        std::lock_guard<std::mutex> guard(lockCount);

        auto count = --fovsUsageCount[filename];

        if(count == 0) {
            //TODO CHECK we should eventually cache the fast image instances since they are used for each overlap.
            //depending on the overlap factor, some performance should be expected.
            std::cout << "delete " << filename << std::endl;
            delete fi;
            fovsCache[filename] = nullptr;
        }

        return count;


    }

private:

    std::mutex lockCount;

    //TODO CHANGE. We are making a copy! Check how to store a reference.
    std::map<std::string, uint32_t> fovsUsageCount;

    std::string _directory;

    std::map<std::string, fi::FastImage<T> *> fovsCache;

    std::mutex lock;


};

#endif //PYRAMIDBUILDING_FOVCACHE_H
