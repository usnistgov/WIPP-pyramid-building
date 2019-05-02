//
// Created by gerardin on 4/19/19.
//

#ifndef PYRAMIDBUILDING_FITILEREQUEST_H
#define PYRAMIDBUILDING_FITILEREQUEST_H

#include <htgs/api/IData.hpp>
# include <pyramidBuilding/pyramid/data/PartialFOV.h>

namespace pb {

    class TileRequest : htgs::IData {


    public:

        TileRequest(size_t row, size_t col,std::vector<PartialFOV*> &fovs) : row(
                row), col(col), fovs(fovs) {}


        size_t getRow() const {
            return row;
        }

        size_t getCol() const {
            return col;
        }

        std::vector<PartialFOV*> &getFovs() {
            return fovs;
        }

        virtual ~TileRequest() {
            for(auto fov : fovs){
                delete fov;
            }
            fovs.clear();
        }


    private:

        size_t row;
        size_t col;
        std::vector<PartialFOV*> fovs;


    };

}

#endif //PYRAMIDBUILDING_FITILEREQUEST_H
