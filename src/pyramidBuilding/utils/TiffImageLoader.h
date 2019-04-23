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
#include <pyramidBuilding/fastImage/data/PartialFOV.h>
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

        explicit TiffImageLoader(std::string) : _directory(std::move(_directory)) {}


        TiffImageLoader(const std::string &_directory, size_t pyramidTileSize) : _directory(_directory),
                                                                                 _pyramidTileSize(pyramidTileSize) {}


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

        T* loadPartialImage(std::string filename, size_t x, size_t y, size_t width, size_t height) {

            auto fullPath = _directory + filename;
            auto file = fullPath.c_str();
            TIFF* tiff = TIFFOpen(file, "r");

            collectMetadata(tiff);


            T *region = new T[width * height]();
            if (tiff) {
                void *buf = _TIFFmalloc(TIFFTileSize(tiff));

                assert(y + height < _FOVHeight);
                assert(x + width < _FOVWidth);

                for (uint32_t posRow = y; posRow < y + height; posRow += _tileHeight) {
                    for (uint32_t posCol = x; posCol < x + width; posCol += _tileWidth) {
                        loadAndCastImageTile(tiff, region, buf, posRow, posCol);
                    }
                }
                _TIFFfree(buf);
                TIFFClose(tiff);
            }
            return region;

        }


        void loadPartialImageIntoTile( T *tile, uint32_t row, uint32_t col, PartialFOV *fov) {
            //open FOV
            auto fullPath = _directory + fov->getFilename();
            auto file = fullPath.c_str();
            TIFF *tiff = TIFFOpen(file, "r");
            collectMetadata(tiff);

            //info FOV subregion to extract
            auto x = fov->getFovOverlap()->originX;
            auto y = fov->getFovOverlap()->originY;
            auto width = fov->getFovOverlap()->width;
            auto height = fov->getFovOverlap()->height;


            //we will read a bunch of tiles in this FOV
            if (tiff) {
                void *buf = _TIFFmalloc(TIFFTileSize(tiff));


                //calculate start and end coordinates of each FOV tile containing a overlapping region
                auto startX = x / _tileWidth * _tileWidth;
                auto startY = y / _tileHeight * _tileHeight;
                auto endX = std::ceil(x + width) / _tileWidth * _tileWidth;
                auto endY = std::ceil(y + height) / _tileHeight * _tileHeight;

                //copy start from the overlap origin to bottom right tile
                for (auto posRow = startY; posRow < endY; posRow += _tileHeight) {
                    for (auto posCol = startX; posCol < endX; posCol += _tileWidth) {
                        TIFFReadTile(tiff, buf, (uint32_t)posCol, (uint32_t)posRow, 0, 0);
                        //now we copy the buffer representing a fov tile in the pyramid tile
                        //we make sure we copy only the region of interest
//
//                        cv::Mat image(this->_tileHeight, this->_tileWidth, CV_8U, (uint16_t *)buf);
//                        auto path = "/home/gerardin/Documents/pyramidBuilding/outputs/DEBUG/tiles/" + fov->getFilename() +  std::to_string(row) + "_" + std::to_string(col) + "---" + std::to_string(posRow) + "_" + std::to_string(posCol) + ".png";
//                        cv::imwrite(path, image);
//                        image.release();

                        //global coordinates in the FOV
                        auto minCol = std::max(posCol, x),
                             minRow = std::max(posRow, y);

                        auto maxRow = std::min(posRow + _tileHeight, y + height),
                             maxCol = std::min(posCol + _tileWidth, x + width);


                        auto rangeMinRow = minRow - posRow,
                             rangeMinCol = minCol - posCol,
                             rangeHeight = maxRow - minRow,
                             rangeWidth = maxCol - minCol;

                        auto fovOverlapMinCol = minCol - x;
                        auto fovOverlapMinRow = minRow - y;


                        for (auto rangeRow = 0; rangeRow < rangeHeight; ++rangeRow) {
                            std::copy_n(
                                    (uint16_t *)buf + (rangeRow + rangeMinRow) * _tileWidth + rangeMinCol,
                                    rangeWidth,
                                    tile + this->_pyramidTileSize * (fov->getTileOverlap()->originY + rangeRow + fovOverlapMinRow) + fov->getTileOverlap()->originX + fovOverlapMinCol);
                        }



                        cv::Mat image2(this->_pyramidTileSize, this->_pyramidTileSize, CV_8U, tile);
                        auto path2 = "/home/gerardin/Documents/pyramidBuilding/outputs/DEBUG/tiles/" + fov->getFilename() +  std::to_string(row) + "_" + std::to_string(col) + "---" + std::to_string(posRow) + "_" + std::to_string(posCol) + ".png";
                        cv::imwrite(path2, image2);
                        image2.release();





//                        auto
//                                posRowMax = std::min((size_t)posRow + _tileHeight, y + height),
//                                posColMax = std::min((size_t)posCol + _tileWidth, x + width);
//
//                        auto
//                                rangeWidth = posColMax - posCol,
//                                rangeHeight = posRowMax - posRow;
//
//                        auto
//                                rowMin = std::max((int32_t)(y - posRow), 0),
//                                colMin = std::max((int32_t)(x - posCol), 0);
//
//                        for (auto rangeRow = ; rangeRow < rangeHeight - rowMin; ++rangeRow) {
//                            std::copy_n((T *) buf + (rangeRow + rowMin) * _tileWidth, rangeWidth, tile + this->_pyramidTileSize *
//                                                                                                    (fov->getTileOverlap()->originY +
//                                                                                                     posRow * _tileHeight +
//                                                                                                     rangeRow) +
//                                                                                             posCol * _tileWidth +
//                                                                                             fov->getTileOverlap()->originX);
//                        }

                    }
                }

                _TIFFfree(buf);
                TIFFClose(tiff);
            }
        }



    private :

        std::string _directory;
        size_t _pyramidTileSize = 0;

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


        template<typename FileType>
        void loadTileRegion(tdata_t src, T *dest, uint32_t rowMin, uint32_t colMin, uint32_t rowMax, uint32_t colMax) {
            for (uint32_t row = 0; row < rowMax - rowMin; ++row) {
                std::copy_n((FileType*)src + rowMin, rowMax-rowMin, dest + _FOVWidth * (rowMin + row) + colMin);
            }
        }





    };

}

#endif //PYRAMIDBUILDING_TILELOADER_H
