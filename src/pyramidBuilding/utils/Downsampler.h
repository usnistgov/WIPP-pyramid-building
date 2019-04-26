//
// Created by Gerardin, Antoine D. (Assoc) on 3/21/19.
//

#ifndef PYRAMIDBUILDING_DOWNSAMPLER_H
#define PYRAMIDBUILDING_DOWNSAMPLER_H

#include <cstdlib>

namespace pb {

    /***
     * @class Downsampler Downsampler.h <pyramidBuilding/utils/Downsampler.h>
     * @brief controls how we downsample an image.
     * @details this class is abstract.
     */
    template<class T>
    class Downsampler {

    public :

        virtual void downsample(T*destinationArray, T *originalArray, size_t originalWidth, size_t originalHeight) = 0;

    };

}

#endif //PYRAMIDBUILDING_DOWNSAMPLER_H
