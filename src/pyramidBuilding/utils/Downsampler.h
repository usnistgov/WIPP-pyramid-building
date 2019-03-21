//
// Created by Gerardin, Antoine D. (Assoc) on 3/21/19.
//

#ifndef PYRAMIDBUILDING_DOWNSAMPLER_H
#define PYRAMIDBUILDING_DOWNSAMPLER_H

#include <cstdlib>

namespace pb {

    template<class T>
    class Downsampler {

    public :

        virtual T* downsample(T *orignalArray, size_t originalWidth, size_t originalHeight) = 0;

    };

}

#endif //PYRAMIDBUILDING_DOWNSAMPLER_H
