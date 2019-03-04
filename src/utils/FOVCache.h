//
// Created by Gerardin, Antoine D. (Assoc) on 3/4/19.
//

#ifndef PYRAMIDBUILDING_FOVCACHE_H
#define PYRAMIDBUILDING_FOVCACHE_H


#include <string>
#include <map>
#include <mutex>
#include <FastImage/api/FastImage.h>
#include <FastImage/TileLoaders/GrayscaleTiffTileLoader.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <experimental/filesystem>

using namespace std::experimental;

template <class T>
class FOVCache {


public:

    FOVCache(std::string directory, std::map<std::string, uint32_t> fovsUsageCount) : _directory(directory), fovsUsageCount(fovsUsageCount) {};

    T* getFOV(std::string filename){

        std::lock_guard<std::mutex> guard(lock);

        T* fov = nullptr;

        auto it = fovsCache.find(filename);
        if(it == fovsCache.end()) {
            std::cout << "FOV not already loaded : " << filename << std::endl;
            fov = loadFullFOV(filename);
            fovsCache[filename] = fov;
        }
        else{
            std::cout << "FOV already loaded : " << filename <<  std::endl;
            fov = it->second;
        }

        return fov;
    }


    uint32_t releaseFOV(std::string filename) {
        std::lock_guard<std::mutex> guard(lockCount);

        auto count = --fovsUsageCount[filename];

        if(count == 0) {
            //TODO CHECK we should eventually cache the fast image instances since they are used for each overlap.
            //depending on the overlap factor, some performance should be expected.
            std::cout << "delete " << filename << std::endl;
            auto it = fovsCache.find(filename);
            if(it == fovsCache.end()) {
                std::cout << "error in cache implementation " << filename << std::endl;
                exit(1);
            }
            delete it->second;
            fovsCache.erase(it);
        }

        return count;
    }


private:

    std::mutex lockCount;

    //TODO CHANGE. We are making a copy! Check how to store a reference.
    std::map<std::string, uint32_t> fovsUsageCount;

    std::string _directory;

    std::map<std::string, T*> fovsCache;

    std::mutex lock;

    uint32_t counter = 0;

    TIFF*
            _tiff;


    uint32_t
            _bitsPerSample,
            _sampleFormat,
            _tileHeight,
            _tileWidth,
            _FOVHeight,
            _FOVWidth;

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
        switch (this->_sampleFormat) {
            case 1 :
                switch (this->_bitsPerSample) {
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
                switch (this->_bitsPerSample) {
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
                switch (this->_bitsPerSample) {
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

    T* loadFullFOV(std::string filename){
        auto fullPath = _directory + filename;
        auto file = fullPath.c_str();
        _tiff = TIFFOpen(file, "r");
        uint32_t samplePerPixel = 0;
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
            void* buf = _TIFFmalloc(TIFFTileSize(_tiff));

            for (uint32_t posRow = 0; posRow < _FOVHeight; posRow += _tileHeight) {
                for (uint32_t posCol = 0; posCol < _FOVWidth; posCol += _tileWidth) {
                    loadPartAndCastFOV(region, buf, posRow, posCol);
                }
            }

            _TIFFfree(buf);
            TIFFClose(_tiff);
        }

//        if(! filesystem::exists(filesystem::current_path() / "debugSimpleTile")) {
//            filesystem::create_directory(filesystem::current_path() / "debugSimpleTile");
//        }
//        cv::Mat image(_FOVHeight, _FOVWidth, CV_16UC1, region);
//     //   counter++;
//        std::string fullImagePath = filesystem::current_path().string() +  "/debugSimpleTile/" + std::to_string(counter) + ".png";
//        cv::imwrite(fullImagePath, image);

        return region;

    }
};

#endif //PYRAMIDBUILDING_FOVCACHE_H
