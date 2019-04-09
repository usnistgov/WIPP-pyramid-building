//
// Created by Gerardin, Antoine D. (Assoc) on 4/8/19.
//

#ifndef PYRAMIDBUILDING_IMAGEINFO_H
#define PYRAMIDBUILDING_IMAGEINFO_H

#include <string>

class FOVMetadata {

    public :
        FOVMetadata(uint32_t width, uint32_t height, short samplePerPixel, short bitsPerSample, short sampleFormat):
        _height(height), _width(width),
        _bitsPerSample(bitsPerSample), _samplePerPixel(samplePerPixel),_sampleFormat(sampleFormat) {}

    short getBitsPerSample() const {
        return _bitsPerSample;
    }

    short getSamplePerPixel() const {
        return _samplePerPixel;
    }

    short getSampleFormat() const {
        return _sampleFormat;
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

    void setTileHeight(uint32_t tileHeight) {
        _tileHeight = tileHeight;
    }

    void setTileWidth(uint32_t tileWidth) {
        _tileWidth = tileWidth;
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
};


#endif //PYRAMIDBUILDING_IMAGEINFO_H
