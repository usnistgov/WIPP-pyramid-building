//
// Created by gerardin on 4/19/19.
//

#ifndef PYRAMIDBUILDING_PARTIALFOV_H
#define PYRAMIDBUILDING_PARTIALFOV_H

#include "FOV.h"

namespace pb {

    class PartialFOV : public FOV {


    public:

        class Overlap {

        public:
            Overlap(size_t originX, size_t originY, size_t width, size_t height) : originX(originX), originY(originY),
                                                                                   width(width), height(height) {}

        private:
            size_t originX;
            size_t originY;
            size_t width;
            size_t height;

        };

        PartialFOV(const std::string &filename, uint32_t row, uint32_t col, uint32_t globalX, uint32_t globalY,
                       std::weak_ptr<FOVMetadata> metadata, Overlap* overlap) :
                       FOV(filename, row, col, globalX, globalY, metadata), overlap(overlap) {}

            Overlap* overlap;

    };
};

#endif //PYRAMIDBUILDING_PARTIALFOV_H
