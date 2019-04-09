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
        FOVRequest(std::string filename) : filename(filename) {}


        const std::string &getFilename() const {
            return filename;
        }

    private:
        std::string filename;

    };

#endif //PYRAMIDBUILDING_FOVREQUEST_H
