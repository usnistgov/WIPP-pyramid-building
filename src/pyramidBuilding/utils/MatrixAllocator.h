//
// Created by Gerardin, Antoine D. (Assoc) on 12/26/18.
//

#ifndef PYRAMIDBUILDING_MATRIXALLOCATOR_H
#define PYRAMIDBUILDING_MATRIXALLOCATOR_H

#include <htgs/api/IMemoryAllocator.hpp>

namespace pb {

    template<class Type>
    class MatrixAllocator : public htgs::IMemoryAllocator<Type> {
    public:
        MatrixAllocator(size_t width, size_t height) : htgs::IMemoryAllocator<Type>(width * height) {}

        Type *memAlloc(size_t size) {
            Type *mem = new Type[size];
            return mem;
        }

        Type *memAlloc() {
            Type *mem = new Type[this->size()];
            return mem;
        }

        void memFree(Type *&memory) {
            delete[] memory;
        }

    };

}
#endif //PYRAMIDBUILDING_MATRIXALLOCATOR_H
