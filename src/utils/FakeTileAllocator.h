//
// Created by Gerardin, Antoine D. (Assoc) on 12/27/18.
//

#ifndef PYRAMIDBUILDING_FAKETILEALLOCATOR_H
#define PYRAMIDBUILDING_FAKETILEALLOCATOR_H

#include <htgs/api/IMemoryAllocator.hpp>
#include <cstdio>
#include "../data/FakeTile.h"

class FakeTileAllocator : public htgs::IMemoryAllocator<FakeTile<uint32_t >> {
public:

    FakeTileAllocator() : htgs::IMemoryAllocator<FakeTile<uint32_t>>(1) {}

    FakeTile<uint32_t> *memAlloc(size_t size) {
        FakeTile<uint32_t> *mem = new FakeTile<uint32_t>();
        return mem;
    }

    FakeTile<uint32_t> *memAlloc() {
        FakeTile<uint32_t> *mem = new FakeTile<uint32_t>();
        return mem;
    }

    void memFree(FakeTile<uint32_t> *&memory) {
        delete memory;
    }

};

#endif //PYRAMIDBUILDING_FAKETILEALLOCATOR_H
