//
// Created by gerardin on 3/11/19.
//

#ifndef PYRAMIDBUILDING_BLENDER_H
#define PYRAMIDBUILDING_BLENDER_H

#include <cstddef>
#include "pyramidBuilding/api/OptionsType.h"

namespace pb {

    template<class T>
    class Blender {

    private:
        const BlendingMethod blendingMethod;

    public:
        Blender(BlendingMethod const &blendingMethod) : blendingMethod(blendingMethod) {}

        void blend(T *srcStartOffset,size_t range, T *destStartOffset){
            switch (blendingMethod) {
                case BlendingMethod::OVERLAY:
                    std::copy_n(srcStartOffset, range, destStartOffset);
                    break;
                case BlendingMethod::MAX:
                    throw std::runtime_error("range copy for MAX blending not implemented.");
            }
        }

        void blend(T *tile, size_t index, T val) {
            switch (blendingMethod) {
                case BlendingMethod::MAX:
                    if (val > tile[index]) {
                        tile[index] = val;
                    }
                    break;
                case BlendingMethod::OVERLAY:
                default:
                    tile[index] = val;
                    break;
            }
        }

    };

}

#endif //PYRAMIDBUILDING_BLENDER_H
