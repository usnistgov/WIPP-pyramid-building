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


    Tile(size_t _level, size_t _row, size_t _col, size_t _width, size_t _height, UserType *_data) : _level(_level), _row(_row), _col(_col), _width(_width),
                                                        _height(_height), _data(_data), _origin(nullptr) {}

    // unpack a Fast Image View and keep track of the original for reclaiming memory at destruction.
    Tile(htgs::m_data_t<fi::View<UserType>> view) :  Tile(view->get()->getRow(), view->get()->getCol(),
            view->get()->getPyramidLevel(), view->get()->getViewWidth(), view->get()->getViewHeight() ,view->get()->getData())  {

        _origin = view;
    }

    size_t getLevel() const {
        return _level;
    }

    size_t getRow() const {
        return _row;
    }

    size_t getCol() const {
        return _col;
    }

    size_t get_width() const {
        return _width;
    }

    size_t get_height() const {
        return _height;
    }

    UserType *getData() const {
        return _data;
    }

    const htgs::m_data_t<fi::View<UserType>> &getOrigin() const {
        return _origin;
    }


private :

    size_t _level;
    size_t _row;
    size_t _col;
    size_t _width;
    size_t _height;
    UserType *_data;
    htgs::m_data_t<fi::View<UserType>> _origin;


};


#endif //PYRAMIDBUILDING_TILE_H
