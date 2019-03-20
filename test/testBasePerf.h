//
// Created by gerardin on 3/7/19.
//

#ifndef PYRAMIDBUILDING_TESTBASEPERF_H
#define PYRAMIDBUILDING_TESTBASEPERF_H

#include <opencv2/opencv.hpp>

#define uint64 uint64_hack_
#define int64 int64_hack_
#include <tiffio.h>
#undef uint64
#undef int64

#include <cstdint>
#include <algorithm>

using namespace std::experimental;

template <class T>
class FOVReader{
private:
    TIFF*
            _tiff;


    uint32_t
            _bitsPerSample,
            _sampleFormat,
            _tileHeight,
            _tileWidth,
            _FOVHeight,
            _FOVWidth,
            samplePerPixel;

    template<typename FileType>
    void loadTile(tdata_t src, T *dest, uint32_t rowMin, uint32_t colMin) {
        uint32_t
                rowMax = std::min(rowMin + _tileHeight, _FOVHeight),
                colMax = std::min(colMin + _tileWidth, _FOVWidth);

        for (uint32_t row = 0; row < rowMax - rowMin; ++row) {
            for (uint32_t col = 0; col < colMax - colMin; ++col) {
                dest[(rowMin + row) * _FOVWidth + (colMin + col)] =
                        (T) ((FileType *) (src))[row * _tileWidth + col];
            }
        }
    }

    void loadPartAndCastFOV(T *region, tdata_t buf, uint32_t rowMin, uint32_t
    colMin) {
        TIFFReadTile(_tiff,
                     buf,
                     colMin,
                     rowMin,
                     0,
                     0);
        switch (_sampleFormat) {
            case 1 :
                switch (_bitsPerSample) {
                    case 8:loadTile<uint8_t>(buf, region, rowMin, colMin);
                        break;
                    case 16:loadTile<uint16_t>(buf, region, rowMin, colMin);
                        break;
                    case 32:loadTile<uint32_t>(buf, region, rowMin, colMin);
                        break;
                    case 64:loadTile<uint64_t>(buf, region, rowMin, colMin);
                        break;
                    default:
                        exit(3);
                }
                break;
            case 2:
                switch (_bitsPerSample) {
                    case 8:loadTile<int8_t>(buf, region, rowMin, colMin);
                        break;
                    case 16:loadTile<int16_t>(buf, region, rowMin, colMin);
                        break;
                    case 32:loadTile<int32_t>(buf, region, rowMin, colMin);
                        break;
                    case 64:loadTile<int64_t>(buf, region, rowMin, colMin);
                        break;
                    default:
                        exit(3);
                }
                break;
            case 3:
                switch (_bitsPerSample) {
                    case 8:loadTile<float>(buf, region, rowMin, colMin);
                        break;
                    case 16:loadTile<float>(buf, region, rowMin, colMin);
                        break;
                    case 32:loadTile<float>(buf, region, rowMin, colMin);
                        break;
                    case 64:loadTile<double>(buf, region, rowMin, colMin);
                        break;
                    default:
                        exit(3);
                }
                break;
            default:
                exit(2);
        }
    }

public:
    FOVReader() = default;

public:

    T* readFOV(const char *inputPath){
        auto region = this->getFOV(inputPath);
        return region;
    }

    void writeFOV(T* data, std::string outputPath){
        _opencvWriteFOV(data, outputPath);
    }


    void readAndForgetFOV(const char *inputPath){
        auto region = this->getFOV(inputPath);
        delete[] region;
    }

    void readWriteFOV(const char* inputPath, std::string outputPath){
        auto region = this->getFOV(inputPath);
        _opencvWriteFOV(region, outputPath);
        delete[] region;
    }

private:
    T* getFOV (const char* file){
        _tiff = TIFFOpen(file, "r");
        T *region = nullptr;
        if (_tiff) {
            // Load/parse header
            TIFFGetField(_tiff, TIFFTAG_IMAGEWIDTH, &_FOVWidth);
            TIFFGetField(_tiff, TIFFTAG_IMAGELENGTH, &_FOVHeight);
            TIFFGetField(_tiff, TIFFTAG_TILEWIDTH, &_tileWidth);
            TIFFGetField(_tiff, TIFFTAG_TILELENGTH, &_tileHeight);
            TIFFGetField(_tiff, TIFFTAG_SAMPLESPERPIXEL, &samplePerPixel);
            TIFFGetField(_tiff, TIFFTAG_BITSPERSAMPLE, &_bitsPerSample);
            TIFFGetField(_tiff, TIFFTAG_SAMPLEFORMAT, &_sampleFormat);

            //https://www.awaresystems.be/imaging/tiff/tifftags/sampleformat.html
            region = new T[ _FOVWidth * _FOVHeight ]();
            auto buf = _TIFFmalloc(TIFFTileSize(_tiff));

            for (uint32_t posRow = 0; posRow < _FOVHeight; posRow += _tileHeight) {
                for (uint32_t posCol = 0; posCol < _FOVWidth; posCol += _tileWidth) {
                    loadPartAndCastFOV(region, buf, posRow, posCol);
                }
            }

            _TIFFfree(buf);
            TIFFClose(_tiff);
        }

        return region;
    }

    void _opencvWriteFOV(T* data, std::string outputPath){

        //TODO CHECK how this can vary with the template
        //TODO CHECK how opencv deals with the input array
        cv::Mat image(1040, 1392, CV_16UC1, data);
        cv::imwrite(outputPath + ".png", image);
        image.release();
    }
};

#endif //PYRAMIDBUILDING_TESTBASEPERF_H
