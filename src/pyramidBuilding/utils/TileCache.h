//
// Created by gerardin on 4/11/19.
//

#ifndef PYRAMIDBUILDING_TILECACHE_H
#define PYRAMIDBUILDING_TILECACHE_H

#include <mutex>
#include <glog/logging.h>
#include <pyramidBuilding/data/Tile.h>

namespace pb {

    template <class T>
    class TileCache {


    public :


        TileCache(uint32_t fullWidth, uint32_t fullHeight, uint32_t tileSize,
                  const std::map<std::pair<size_t, size_t>, u_int8_t> &fovUsageCount) : fullWidth(fullWidth),
                                                                                        fullHeight(fullHeight),
                                                                                        tileSize(tileSize),
                                                                                        fovUsageCount(fovUsageCount) {}


        Tile<T>* getTile(uint32_t row, uint32_t col){

            std::lock_guard<std::mutex> guard(lock);
            //     Tile<T>* t = nullptr;
            std::pair<size_t,size_t> index= std::make_pair(row,col);
            auto it = tileCache.find(index);
            if(it == tileCache.end()){
                VLOG(3) << " building new tile :  (" << row << "," << col << ")" << std::endl;
                VLOG(3) << " number of FOV needed to fill tile (" << row << "," << col << ") : " << (int)fovUsageCount[{row,col}] << std::endl;
                auto width = std::min(tileSize, fullWidth - col * tileSize);
                auto height = std::min(tileSize, fullHeight - row * tileSize);
                Tile<T>* t = new Tile<T>(0,row,col,width,height, new T[width * height]());
                tileCache.insert({index,t});
            }
            else{
                VLOG(3) << "tile already in cache :  (" << row << "," << col << ")" << std::endl;
                //   t = it->second;
            }

            return tileCache.find(index)->second;
            //return t;
        }

        void deleteTile(uint32_t row, uint32_t col){
            tileCache.erase({row,col});
        }

        size_t size() {
            return tileCache.size();
        }

        void doneCopyingFOVintoTile(uint32_t row, uint32_t col){
            fovUsageCount[{row,col}] -= 1;
            VLOG(3) << " number of FOV needed to complete tile (" << row << "," << col << ") : " << (int)fovUsageCount[{row,col}] << std::endl;
            assert((fovUsageCount[{row,col}]) >= 0);
        }

        bool tileReady(uint32_t row, uint32_t col){
            return fovUsageCount[{row,col}] == 0;
    }


    private :
        std::map<std::pair<size_t,size_t>, Tile<T>*> tileCache;
        std::mutex lock;
        uint32_t fullWidth;
        uint32_t fullHeight;
        uint32_t tileSize;
        std::map<std::pair<size_t,size_t>, u_int8_t> fovUsageCount;
    };


}

#endif //PYRAMIDBUILDING_TILECACHE_H
