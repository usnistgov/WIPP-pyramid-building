//
// Created by Gerardin, Antoine D. (Assoc) on 12/27/18.
//

#ifndef PYRAMIDBUILDING_TILE_H
#define PYRAMIDBUILDING_TILE_H


#include <glog/logging.h>
#include <htgs/api/IData.hpp>
#include <htgs/types/Types.hpp>
#include <FastImage/api/View.h>
#include <htgs/api/MemoryData.hpp>

namespace pb {

/**
 * @class Tile Tile.h <pyramidBuilding/data/Tile.h>
 * @brief class to hold pyramid tile data and metadata
 */
    template<class T>
    class Tile : public htgs::IData {

    public :
        /***
         * Constructor for a pyramid tile level at the base level.
         * @param _level the pyramid level at which this tile is located.
         * @param _row the row at which this tile is located.
         * @param _col the column at which this tile is located.
         * @param _width the width of this tile.
         * @param _height the height of this tile.
         * @param _data the underlying data held by this tile.
         */
        Tile(size_t _level, size_t _row, size_t _col, size_t _width, size_t _height, T *_data) : _level(_level),
                                                                                                 _row(_row), _col(_col),
                                                                                                 _width(_width),
                                                                                                 _height(_height),
                                                                                                 _data(_data) {}

        /***
         * Constructor for a pyramid tile level at levels above the base level.
         * We track the block of tiles which we have downsampled to produce this tile.
         * The goal is to have a easy way to delete those tiles in the cache stored in the bookeeper.
         * @param _level the pyramid level at which this tile is located.
         * @param _row the row at which this tile is located.
         * @param _col the column at which this tile is located.
         * @param _width the width of this tile.
         * @param _height the height of this tile.
         * @param _data the underlying data held by this tile.
         * @param origin
         */
        Tile(size_t _level, size_t _row, size_t _col, size_t _width, size_t _height, T *_data,
             std::vector<std::shared_ptr<Tile<T>>> &origin) : _level(_level), _row(_row), _col(_col), _width(_width),
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

        size_t getWidth() const {
            return _width;
        }

        size_t getHeight() const {
            return _height;
        }

        T *getData() const {
            return _data;
        }

        std::vector<std::shared_ptr<Tile<T>>> &getOrigin() {
            return _origin;
        }

        ~Tile() {
            delete[] _data;
            _data = nullptr;
            VLOG(3) << "Tile destroyed : " << getRow() << "," << getCol() << "," << getLevel() << std::endl;
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

}

#endif //PYRAMIDBUILDING_TILE_H
