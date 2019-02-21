//
// Created by Gerardin, Antoine D. (Assoc) on 12/27/18.
//

#ifndef PYRAMIDBUILDING_TILE_H
#define PYRAMIDBUILDING_TILE_H

#include <htgs/api/IData.hpp>
#include <htgs/types/Types.hpp>
#include <FastImage/api/View.h>
#include <htgs/api/MemoryData.hpp>

template <class T>
class Tile : public htgs::IData {

public :

    Tile(size_t _level, size_t _row, size_t _col, size_t _width, size_t _height, T *_data) : _level(_level), _row(_row), _col(_col), _width(_width),
                                                        _height(_height), _data(_data) {}


    Tile(size_t _level, size_t _row, size_t _col, size_t _width, size_t _height, T *_data, std::vector<std::shared_ptr<Tile<T>>> &origin) : _level(_level), _row(_row), _col(_col), _width(_width),
                                                                                                    _height(_height), _data(_data), _origin(origin) {}

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

    T *getData() const {
        return _data;
    }

    std::vector<std::shared_ptr<Tile<T>>> &getOrigin() {
        return _origin;
    }

    ~Tile(){
        delete[] _data;
        _data = nullptr;
        std::cout << "I am dead! " << getRow() << "," << getCol() <<"," << getLevel() << std::endl;
    }


private :

    size_t _level;
    size_t _row;
    size_t _col;
    size_t _width;
    size_t _height;
    T *_data;
    std::vector<std::shared_ptr<Tile<T>>> _origin = {};


};


#endif //PYRAMIDBUILDING_TILE_H
