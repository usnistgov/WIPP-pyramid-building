//
// Created by Gerardin, Antoine D. (Assoc) on 4/8/19.
//

#ifndef PYRAMIDBUILDING_IMAGEINFO_H
#define PYRAMIDBUILDING_IMAGEINFO_H

#include <string>

class FOVMetadata {

    public :
        FOVMetadata(uint32_t width, uint32_t height, short samplePerPixel, short bitsPerSample, short sampleFormat, std::string imageDirectory):
        _height(height), _width(width),
        _bitsPerSample(bitsPerSample), _samplePerPixel(samplePerPixel),_sampleFormat(sampleFormat), _directory(imageDirectory) {}

    short getBitsPerSample() const {
        return _bitsPerSample;
    }

    short getSamplePerPixel() const {
        return _samplePerPixel;
    }

    short getSampleFormat() const {
        return _sampleFormat;
    }

    const std::string &getDirectory() const {
        return _directory;
    }

    uint32_t getTileHeight() const {
        return _tileHeight;
    }

    uint32_t getTileWidth() const {
        return _tileWidth;
    }

    uint32_t getHeight() const {
        return _height;
    }

    uint32_t getWidth() const {
        return _width;
    }

    uint32_t getFullFovWidth() const {
        return _fullFovWidth;
    }

    uint32_t getFullFovHeight() const {
        return _fullFovHeight;
    }

    uint32_t getMaxRow() const {
        return _maxRow;
    }

    uint32_t getMaxCol() const {
        return _maxCol;
    }

    void setTileHeight(uint32_t tileHeight) {
        _tileHeight = tileHeight;
    }

    void setTileWidth(uint32_t tileWidth) {
        _tileWidth = tileWidth;
    }

    void setFullFovWidth(uint32_t fullFovWidth) {
        _fullFovWidth = fullFovWidth;
    }

    void setFullFovHeight(uint32_t fullFovHeight) {
        _fullFovHeight = fullFovHeight;
    }

    void setMaxRow(uint32_t maxRow) {
        _maxRow = maxRow;
    }

    void setMaxCol(uint32_t maxCol) {
        _maxCol = maxCol;
    }


private:
    short _bitsPerSample = 0,
            _samplePerPixel = 0,
            _sampleFormat = 0;

    uint32_t
            _tileHeight = 0,
            _tileWidth = 0,
            _height = 0,
            _width = 0;

    std::string _directory;

    uint32_t _fullFovWidth = 0,
             _fullFovHeight = 0;

    uint32_t _maxRow = 0,
             _maxCol = 0;
};


#endif //PYRAMIDBUILDING_IMAGEINFO_H
