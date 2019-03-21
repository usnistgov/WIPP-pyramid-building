//
// Created by Gerardin, Antoine D. (Assoc) on 3/21/19.
//

#ifndef PYRAMIDBUILDING_AVERAGEDOWNSAMPLER_H
#define PYRAMIDBUILDING_AVERAGEDOWNSAMPLER_H

#include <math.h>
#include "Downsampler.h"

namespace pb {

    template<class T>
    class AverageDownsampler : public Downsampler<T> {

    public :

        AverageDownsampler() = default;

        virtual ~AverageDownsampler() = default;


        T* downsample(T *newTileData, size_t width, size_t height) override {

            //TODO check this kind of conversion throughout
            //in particular : when we have size_t as inputs, how do we ensure there is no overflow
            auto downsampleWidth = static_cast<size_t>(ceil((double) width / 2));
            auto downsampleHeight = static_cast<size_t>(ceil((double) height / 2));

            T *downsampleData = new T[downsampleWidth * downsampleHeight]();

            for (size_t j = 0; j < downsampleHeight - 1; j++) {
                for (size_t i = 0; i < downsampleWidth - 1; i++) {
                    size_t index = j * downsampleWidth + i;
                    downsampleData[index] =
                            (newTileData[2 * j * width + 2 * i] + newTileData[2 * j * width + 2 * i + 1] +
                             newTileData[2 * (j + 1) * width + 2 * i] + newTileData[2 * (j + 1) * width + 2 * i + 1]) /
                            4;
                }
            }

            for (size_t i = 0; i < downsampleWidth - 1; i++) {
                size_t index = (downsampleHeight - 1) * downsampleWidth + i;
                downsampleData[index] =
                        (newTileData[(height - 1) * width + 2 * i] + newTileData[(height - 2) * width + 2 * i] +
                         newTileData[(height - 1) * width + 2 * i + 1] +
                         newTileData[(height - 2) * width + 2 * i + 1]) / 4;
            }

            for (size_t i = 0; i < downsampleHeight - 1; i++) {
                size_t index = downsampleWidth * i + downsampleWidth - 1;
                downsampleData[index] =
                        (newTileData[width * 2 * i + width - 1] + newTileData[width * 2 * i + width - 2] +
                         newTileData[width * (2 * i + 1) + width - 1] + newTileData[width * (2 * i + 1) + width - 1]) /
                        4;
            }

            downsampleData[downsampleWidth * downsampleHeight - 1] =
                    (newTileData[width * height - 1] + newTileData[width * (height - 1) - 1] +
                     newTileData[width * height - 2] + newTileData[width * (height - 1) - 2]) / 4;

            return downsampleData;
        }
    };

}


#endif //PYRAMIDBUILDING_AVERAGEDOWNSAMPLER_H
