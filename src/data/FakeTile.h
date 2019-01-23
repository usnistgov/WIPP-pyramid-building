//
// Created by Gerardin, Antoine D. (Assoc) on 12/21/18.
//

#ifndef PYRAMIDBUILDING_FAKETILE_H
#define PYRAMIDBUILDING_FAKETILE_H

#include <FastImage/api/FastImage.h>

template <class Type>
class FakeTile : public fi::View<Type> {

public :

    FakeTile() :  fi::View<uint32_t>(512,512) {
        _level = 0;
}

    FakeTile(const uint32_t &row, const uint32_t &col, const uint32_t &level) : fi::View<Type>(row, col) {
        _level = level;
    }


    uint32_t getPyramidLevel() const { return _level; }


private:

    uint32_t _level;


};

#endif //PYRAMIDBUILDING_FAKETILE_H
