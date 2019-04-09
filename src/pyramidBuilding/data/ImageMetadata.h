//
// Created by Gerardin, Antoine D. (Assoc) on 4/8/19.
//

#ifndef PYRAMIDBUILDING_IMAGEINFO_H
#define PYRAMIDBUILDING_IMAGEINFO_H

#include <string>
#include <tiffio.h>

class ImageMetadata {

    public :
        ImageMetadata(std::string filePath) {
            auto file = filePath.c_str();
            TIFF* tiff = TIFFOpen(file, "r");
            // Load/parse header
            // For full reference, see https://www.awaresystems.be/imaging/tiff/tifftags.html
            TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &_FOVWidth);
            TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &_FOVHeight);
            TIFFGetField(tiff, TIFFTAG_TILEWIDTH, &_tileWidth);
            TIFFGetField(tiff, TIFFTAG_TILELENGTH, &_tileHeight);
            TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &_samplePerPixel);
            TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &_bitsPerSample);
            TIFFGetField(tiff, TIFFTAG_SAMPLEFORMAT, &_sampleFormat);

    }


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

    uint32_t getFOVHeight() const {
        return _FOVHeight;
    }

    uint32_t getFOVWidth() const {
        return _FOVWidth;
    }

private:
    short _bitsPerSample = 0,
            _samplePerPixel = 0,
            _sampleFormat = 0;

    uint32_t
            _tileHeight = 0,
            _tileWidth = 0,
            _FOVHeight = 0,
            _FOVWidth = 0;
};


#endif //PYRAMIDBUILDING_IMAGEINFO_H
