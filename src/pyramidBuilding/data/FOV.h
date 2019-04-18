//
// Created by Gerardin, Antoine D. (Assoc) on 4/9/19.
//

#ifndef PYRAMIDBUILDING_FOV_H
#define PYRAMIDBUILDING_FOV_H

#include <string>
#include <htgs/api/IData.hpp>
#include "FOVMetadata.h"
#include <htgs/api/MemoryData.hpp>

namespace pb {

    class FOV : public htgs::IData {

    public:

        FOV(const std::string &filename, uint32_t row, uint32_t col, uint32_t globalX, uint32_t globalY,
            FOVMetadata *metadata) : filename(filename), row(row), col(col), globalX(globalX), globalY(globalY),
                                     metadata(metadata) {}

        const std::string &getFilename() const {
            return filename;
        }

        uint32_t getRow() const {
            return row;
        }

        uint32_t getCol() const {
            return col;
        }

        uint32_t getGlobalX() const {
            return globalX;
        }

        uint32_t getGlobalY() const {
            return globalY;
        }

        FOVMetadata *getMetadata() const {
            return metadata;
        }

        ~FOV(){
            VLOG(4) << "remove FOV (" << row << "," << col << ")" << std::endl;
        }


    private:
        std::string filename;
        uint32_t row;
        uint32_t col;
        uint32_t globalX;
        uint32_t globalY;
        FOVMetadata *metadata;
    };

}

#endif //PYRAMIDBUILDING_FOV_H
