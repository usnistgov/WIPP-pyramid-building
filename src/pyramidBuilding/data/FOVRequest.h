//
// Created by Gerardin, Antoine D. (Assoc) on 4/8/19.
//

#ifndef PYRAMIDBUILDING_FOVREQUEST_H
#define PYRAMIDBUILDING_FOVREQUEST_H

#include <string>
#include <htgs/api/IData.hpp>

namespace pb {

    class FOVRequest : htgs::IData {


    public:
        FOVRequest(uint32_t row, uint32_t col) : row(row), col(col) {}

    private:
        uint32_t row;
        uint32_t col;

    };

}

#endif //PYRAMIDBUILDING_FOVREQUEST_H
