//
// Created by gerardin on 3/28/19.
//

#ifndef PYRAMIDBUILDING_TILEALLOCATOR_H
#define PYRAMIDBUILDING_TILEALLOCATOR_H


#include <htgs/api/IMemoryAllocator.hpp>
#include "../data/Tile.h"

namespace pb {

    template<class T>
    class TileAllocator : public htgs::IMemoryAllocator<T> {

    public:
        TileAllocator(const size_t &width, const size_t &height) : htgs::IMemoryAllocator<Tile< T>>(0),
        width (width), height(height) {}

        T *memAlloc(size_t size) override {
            return new Tile<T>(0,0,0,width,height,new T[width * height]);
        }

        T *memAlloc() override {
            return new Tile<T>(0,0,0,width,height,new T[width * height]);
        }

        void memFree(T *&memory) override {
            delete memory;
        }


    private:
        size_t
                width,
                height;

    };

}

#endif //PYRAMIDBUILDING_TILEALLOCATOR_H
