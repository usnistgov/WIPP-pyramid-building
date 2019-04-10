//
// Created by Gerardin, Antoine D. (Assoc) on 4/9/19.
//

#ifndef PYRAMIDBUILDING_FOVALLOCATOR_H
#define PYRAMIDBUILDING_FOVALLOCATOR_H


#include <htgs/api/IMemoryAllocator.hpp>
#include "../data/Tile.h"

namespace pb {

    template<class T>
    class FOVAllocator : public htgs::IMemoryAllocator<T> {

    public:

        FOVAllocator(const size_t &width, const size_t &height) : htgs::IMemoryAllocator<T>(width * height) {}

        T *memAlloc(size_t size) override {
            return new T[size];
        }

        T *memAlloc() override {
            return new T[this->size()];
        }

        void memFree(T *&memory) override {
            delete memory;
        }

    };

}

#endif //PYRAMIDBUILDING_FOVALLOCATOR_H
