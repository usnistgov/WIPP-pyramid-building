//
// Created by gerardin on 4/19/19.
//

#ifndef PYRAMIDBUILDING_PARTIALFOV_H
#define PYRAMIDBUILDING_PARTIALFOV_H

namespace pb {

    class PartialFOV {


    public:

        class Overlap {

        public:
            Overlap(size_t originX, size_t originY, size_t width, size_t height) : originX(originX), originY(originY),
                                                                                   width(width), height(height) {}

        public:
            size_t originX;
            size_t originY;
            size_t width;
            size_t height;


        };

        PartialFOV(const std::string &filename, Overlap* fovOverlap, Overlap* tileOverlap) : filename(filename), fovOverlap(fovOverlap), tileOverlap(tileOverlap) {}

        public:
            Overlap *getFovOverlap() const {
                return fovOverlap;
            }

            Overlap *getTileOverlap() const {
                return tileOverlap;
            }

            const std::string &getFilename() const {
                return filename;
            }


    private :
            Overlap* fovOverlap;
            Overlap* tileOverlap;
            std::string filename;

    };
};

#endif //PYRAMIDBUILDING_PARTIALFOV_H
