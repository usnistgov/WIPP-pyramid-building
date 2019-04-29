//
// Created by Gerardin, Antoine D. (Assoc) on 3/21/19.
//

#ifndef PYRAMIDBUILDING_AVERAGEDOWNSAMPLER_H
#define PYRAMIDBUILDING_AVERAGEDOWNSAMPLER_H

#include <math.h>
#include "Downsampler.h"
#include <pyramidBuilding/utils/Utils.h>

namespace pb {

    template<class T>
    class AverageDownsampler : public Downsampler<T> {

    public :

        AverageDownsampler() = default;

        virtual ~AverageDownsampler() = default;


        void downsample(T* destinationArray,T *originalArray, size_t width, size_t height) override {

            auto downsampleWidth = static_cast<size_t>(ceil((double) width / 2));
            auto downsampleHeight = static_cast<size_t>(ceil((double) height / 2));

            for (size_t j = 0; j < downsampleHeight - 1; j++) {
                for (size_t i = 0; i < downsampleWidth - 1; i++) {
                    size_t index = j * downsampleWidth + i;
                    destinationArray[index] =
                            (originalArray[2 * j * width + 2 * i] + originalArray[2 * j * width + 2 * i + 1] +
                             originalArray[2 * (j + 1) * width + 2 * i] + originalArray[2 * (j + 1) * width + 2 * i + 1]) /
                            4;
                }
            }

            for (size_t i = 0; i < downsampleWidth - 1; i++) {
                size_t index = (downsampleHeight - 1) * downsampleWidth + i;
                destinationArray[index] =
                        (originalArray[(height - 1) * width + 2 * i] + originalArray[(height - 2) * width + 2 * i] +
                         originalArray[(height - 1) * width + 2 * i + 1] +
                         originalArray[(height - 2) * width + 2 * i + 1]) / 4;
            }

            for (size_t i = 0; i < downsampleHeight - 1; i++) {
                size_t index = downsampleWidth * i + downsampleWidth - 1;
                destinationArray[index] =
                        (originalArray[width * 2 * i + width - 1] + originalArray[width * 2 * i + width - 2] +
                         originalArray[width * (2 * i + 1) + width - 1] + originalArray[width * (2 * i + 1) + width - 1]) /
                        4;
            }

            destinationArray[downsampleWidth * downsampleHeight - 1] =
                    (originalArray[width * height - 1] + originalArray[width * (height - 1) - 1] +
                     originalArray[width * height - 2] + originalArray[width * (height - 1) - 2]) / 4;

        }



    };

}


#endif //PYRAMIDBUILDING_AVERAGEDOWNSAMPLER_H
