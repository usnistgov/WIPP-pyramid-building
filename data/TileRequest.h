//
// Created by Gerardin, Antoine D. (Assoc) on 12/19/18.
//

#ifndef PYRAMIDBUILDING_TILEREQUEST_H
#define PYRAMIDBUILDING_TILEREQUEST_H

#include <htgs/api/IData.hpp>
#include <FastImage/api/FastImage.h>

template <class Type>
class TileRequest : public htgs::IData {

public:
    TileRequest(uint32_t level, const fi::View<Type> &view) : level(level), view(view) {}

    uint32_t getLevel() const {
        return level;
    }

    const fi::View<Type> &getView() const {
        return view;
    }

private:
    uint32_t level;
    fi::View<Type> view;



};

#endif //PYRAMIDBUILDING_TILEREQUEST_H
