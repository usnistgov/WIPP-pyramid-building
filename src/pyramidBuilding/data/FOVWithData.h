//
// Created by Gerardin, Antoine D. (Assoc) on 4/9/19.
//

#ifndef PYRAMIDBUILDING_FOVWITHDATA_H
#define PYRAMIDBUILDING_FOVWITHDATA_H

#include <htgs/types/Types.hpp>
#include "FOV.h"

namespace pb {

    template <class T>
    class FOVWithData : public htgs::IData {

    public:
        FOVWithData(std::shared_ptr<FOV> fov, const htgs::m_data_t<T> &data) : fov(fov), data(data) {}

        std::shared_ptr<FOV> getFov() const {
            return fov;
        }

        const htgs::m_data_t<T> &getData() const {
            return data;
        }

        ~FOVWithData(){
            VLOG(4) << "remove FOV with data" << std::endl;
        }

    private:

        std::shared_ptr<FOV> fov;
        htgs::m_data_t<T> data;

    };
}

#endif //PYRAMIDBUILDING_FOVWITHDATA_H
