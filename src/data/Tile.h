//
// Created by Gerardin, Antoine D. (Assoc) on 12/27/18.
//

#ifndef PYRAMIDBUILDING_TILE_H
#define PYRAMIDBUILDING_TILE_H

#include <htgs/api/IData.hpp>
#include <htgs/types/Types.hpp>
#include <FastImage/api/View.h>
#include <htgs/api/MemoryData.hpp>

template <class UserType>
class Tile : public htgs::IData {

public :


    Tile(uint32_t row, uint32_t col, uint32_t level, UserType *data ) : _row(row), _col(col), _level(level), _data(data), _origin(nullptr) {};

    // unpack a Fast Image View and keep track of the original for reclaiming memory at destruction.
    Tile(htgs::m_data_t<fi::View<UserType>> view) :  Tile(view->get()->getRow(), view->get()->getCol(), view->get()->getPyramidLevel(), view->get()->getData())  {
        _origin = view;
    }

//    ~Tile() {
//        if(_origin){
//            _origin->releaseMemory();
//        }
//        delete _data;
//    }

    uint32_t getLevel() const {
        return _level;
    }

    uint32_t getRow() const {
        return _row;
    }

    uint32_t getCol() const {
        return _col;
    }

    UserType *getData() const {
        return _data;
    }

    const htgs::m_data_t<fi::View<UserType>> &getOrigin() const {
        return _origin;
    }


private :

    uint32_t _level;
    uint32_t _row;
    uint32_t _col;
    UserType *_data;
    htgs::m_data_t<fi::View<UserType>> _origin;


};


#endif //PYRAMIDBUILDING_TILE_H
