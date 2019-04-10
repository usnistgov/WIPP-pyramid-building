//
// Created by gerardin on 3/25/19.
//

#ifndef PYRAMIDBUILDING_TILELOADER_H
#define PYRAMIDBUILDING_TILELOADER_H



#include <string>
#include <map>
#include <mutex>
#include <glog/logging.h>
#include <tiffio.h>
#include <experimental/filesystem>
#include "htgs/api/MemoryData.hpp"

namespace pb {


    template <class T>
    class TiffImageLoader {

    private:

        bool ref = false;

        short _bitsPerSample = 0,
                _samplePerPixel = 0,
                _sampleFormat = 0;

        uint32_t
                _tileHeight = 0,
                _tileWidth = 0,
                _FOVHeight = 0,
                _FOVWidth = 0;

    public :


        TiffImageLoader(std::string directory) : _directory(directory) {}



        void collectMetadata(TIFF* tiff){
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


        void loadFullImage(T* image, std::string filename) {
            auto fullPath = _directory + filename;
            auto file = fullPath.c_str();
            TIFF* tiff = TIFFOpen(file, "r");

            collectMetadata(tiff);

            if (tiff) {
                void *buf = _TIFFmalloc(TIFFTileSize(tiff));

                for (uint32_t posRow = 0; posRow < _FOVHeight; posRow += _tileHeight) {
                    for (uint32_t posCol = 0; posCol < _FOVWidth; posCol += _tileWidth) {
                        loadAndCastImageTile(tiff, image, buf, posRow, posCol);
                    }
                }
                _TIFFfree(buf);
                TIFFClose(tiff);
            }
        }

        T *loadFullImage(std::string filename) {

            auto fullPath = _directory + filename;
            auto file = fullPath.c_str();
            TIFF* tiff = TIFFOpen(file, "r");

            collectMetadata(tiff);

            T *region = new T[_FOVWidth * _FOVHeight]();
            if (tiff) {

                void *buf = _TIFFmalloc(TIFFTileSize(tiff));

                for (uint32_t posRow = 0; posRow < _FOVHeight; posRow += _tileHeight) {
                    for (uint32_t posCol = 0; posCol < _FOVWidth; posCol += _tileWidth) {
                        loadAndCastImageTile(tiff, region, buf, posRow, posCol);
                    }
                }
                _TIFFfree(buf);
                TIFFClose(tiff);
            }
            return region;
        }


    private :

        std::string _directory;

        void loadAndCastImageTile(TIFF *tiff, T *region, tdata_t buf, uint32_t rowMin, uint32_t
        colMin) {
            TIFFReadTile(tiff,
                         buf,
                         colMin,
                         rowMin,
                         0,
                         0);
            switch (this->_sampleFormat) {
                case 1 :
                    switch (this->_bitsPerSample) {
                        case 8:
                            loadTile<uint8_t>(buf, region, rowMin, colMin);
                            break;
                        case 16:
                            loadTile<uint16_t>(buf, region, rowMin, colMin);
                            break;
                        case 32:
                            loadTile<uint32_t>(buf, region, rowMin, colMin);
                            break;
                        case 64:
                            loadTile<uint64_t>(buf, region, rowMin, colMin);
                            break;
                        default:
                            LOG(FATAL) << "bitsPerSample" << this->_bitsPerSample << "is unknown or unsupported"
                                       << std::endl;
                            exit(3);
                    }
                    break;
                case 2:
                    switch (this->_bitsPerSample) {
                        case 8:
                            loadTile<int8_t>(buf, region, rowMin, colMin);
                            break;
                        case 16:
                            loadTile<int16_t>(buf, region, rowMin, colMin);
                            break;
                        case 32:
                            loadTile<int32_t>(buf, region, rowMin, colMin);
                            break;
                        case 64:
                            loadTile<int64_t>(buf, region, rowMin, colMin);
                            break;
                        default:
                            LOG(FATAL) << "bitsPerSample" << this->_bitsPerSample << "is unknown or unsupported"
                                       << std::endl;
                            exit(3);
                    }
                    break;
                case 3:
                    switch (this->_bitsPerSample) {
                        case 8:
                            loadTile<float>(buf, region, rowMin, colMin);
                            break;
                        case 16:
                            loadTile<float>(buf, region, rowMin, colMin);
                            break;
                        case 32:
                            loadTile<float>(buf, region, rowMin, colMin);
                            break;
                        case 64:
                            loadTile<double>(buf, region, rowMin, colMin);
                            break;
                        default:
                            LOG(FATAL) << "bitsPerSample" << this->_bitsPerSample << "is unknown or unsupported"
                                       << std::endl;
                            exit(3);
                    }
                    break;
                default:
                    LOG(FATAL) << "sampleFormat" << this->_sampleFormat << "is unknown or unsupported" << std::endl;
                    exit(2);
            }
        }


        template<typename FileType>
        void loadTile(tdata_t src, T *dest, uint32_t rowMin, uint32_t colMin) {
            uint32_t
                    rowMax = std::min(rowMin + _tileHeight, _FOVHeight),
                    colMax = std::min(colMin + _tileWidth, _FOVWidth),
                    width = colMax - colMin;

            for (uint32_t row = 0; row < rowMax - rowMin; ++row) {
                std::copy_n((FileType*)src + row * _tileWidth, width, dest + _FOVWidth * (rowMin + row) + colMin);
            }
        }


    };

}

#endif //PYRAMIDBUILDING_TILELOADER_H
