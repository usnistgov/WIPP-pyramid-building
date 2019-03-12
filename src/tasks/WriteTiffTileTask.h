//
// Created by gerardin on 3/11/19.
//

#ifndef PYRAMIDBUILDING_WRITETIFFTILETASK_H
#define PYRAMIDBUILDING_WRITETIFFTILETASK_H


#include <htgs/api/ITask.hpp>
#include "FastImage/api/FastImage.h"
#include "../data/Tile.h"
#include <tiffio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include "WriteTileTask.h"
#include "../api/Datatype.h"
#include "../utils/GridGenerator.h"

#include <experimental/filesystem>

using namespace std::experimental;

template <class T>
class WriteTiffTileTask : public htgs::ITask< Tile<T>, Tile<T> > {

public:
    WriteTiffTileTask(
            size_t numThreads, const std::string &_pathOut, const std::string &pyramidName,
            const ImageDepth imageDepth, const GridGenerator *gridGenerator) :
            htgs::ITask<Tile<T>, Tile<T>>(numThreads),
            _pathOut(_pathOut),
            pyramidName(pyramidName),
            imageDepth(imageDepth),
            info(gridGenerator) {

        //create the images directory structure
        filesystem::path path = _pathOut;

        if(! filesystem::exists(path)) {
            filesystem::create_directory(path);
        }

//        auto fullPath = path / (pyramidName + ".tif");
//        auto file = fullPath.c_str();
//        _tiff = TIFFOpen(file, "w");

        size_t bitsPerSample = 0;

        switch(imageDepth){
            case ImageDepth::_8U :
                bitsPerSample = sizeof(uint8) * 8;
                break;
            case ImageDepth::_16U :
                bitsPerSample = sizeof(uint16) * 8;
                break;
        }

//        if (_tiff != nullptr) {
//            TIFFSetField(_tiff, TIFFTAG_IMAGEWIDTH, info->getFullFovWidth());
//            TIFFSetField(_tiff, TIFFTAG_IMAGELENGTH, info->getFullFovHeight());
//            TIFFSetField(_tiff, TIFFTAG_TILELENGTH, info->getPyramidTileSize());
//            TIFFSetField(_tiff, TIFFTAG_TILEWIDTH, info->getPyramidTileSize());
//            TIFFSetField(_tiff, TIFFTAG_BITSPERSAMPLE,bitsPerSample);
//            TIFFSetField(_tiff, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
//            TIFFSetField(_tiff, TIFFTAG_ROWSPERSTRIP, 1);
//            TIFFSetField(_tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
//            TIFFSetField(_tiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
//            TIFFSetField(_tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
//            TIFFSetField(_tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
//        }

        layers = new std::vector<TIFF*>(info->getNumLevel());


        for(size_t l = 0;  l< info->getNumLevel(); l++){
            auto fullPath = path / (pyramidName + "_" + std::to_string(l) + ".tif");
            auto file = fullPath.c_str();
            auto tiff = TIFFOpen(file, "w");
            TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, info->getFullFovWidthAtLevel(l));
            TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, info->getFullFovHeightAtLevel(l));
            TIFFSetField(tiff, TIFFTAG_TILELENGTH, info->getPyramidTileSize());
            TIFFSetField(tiff, TIFFTAG_TILEWIDTH, info->getPyramidTileSize());
            TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE,bitsPerSample);
            TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
            TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, 1);
            TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
            TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
            TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
            TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

            if(l > 0){
//                TIFFSetField(tiff, TIFFTAG_SUBFILETYPE, FILETYPE_REDUCEDIMAGE );
            }

            layers->at(l) = tiff;
        }
//
//        int dircount = 0;
//        do {
//            dircount++;
//        } while (TIFFReadDirectory(_tiff));
//        printf("%d directories in %s\n", dircount, pyramidName.c_str());

    }


    void executeTask(std::shared_ptr<Tile<T>> data) override {

        size_t tileSize = info->getPyramidTileSize();
        size_t x = data->getCol() * tileSize;
        size_t y = data->getRow() * tileSize;
        size_t level = data->getLevel();

        size_t originalHeight = data->get_height();
        size_t originalWidth = data->get_width();


//        auto dir = TIFFSetDirectory(_tiff, level);
//        if(dir == 0){
//            throw std::runtime_error("tiff directory #" + std::to_string(level) + " does not exist");
//        }

//        auto dir = TIFFSetDirectory(_tiff, (uint16_t)level);
//
//        if(level > 0){
//
//            //return;
//        }

//        if(level == 0 || level > 1){
//            return;
//        }


        TIFF* tiff = this->layers->at(0);


        TIFFSetDirectory(tiff, level);


        T* tile = nullptr;

        //tiff only process tiles of the same size. If we process a border tile, we need to redimension it.
        if(data->getRow() == info->getGridMaxRow(level) || data->getCol() == info->getGridMaxCol(level)) {
            T *tile = new T[tileSize * tileSize]();
            for (uint32_t row = 0; row < data->get_height(); ++row) {
                std::copy_n(data->getData() + row * originalWidth, originalWidth, tile + row * tileSize);
            }
            TIFFWriteTile(tiff, (tdata_t)tile, (uint32)x, (uint32)y, 0, 0);
            delete[] tile;
        }
        else{
            tile = data->getData();
            TIFFWriteTile(tiff, (tdata_t)tile, (uint32)x, (uint32)y, 0, 0);
        }

        TIFFWriteDirectory(tiff);




        this->addResult(data);
    }

    /// \brief Close the tiff file
    void shutdown() override {
        for(auto l = 0;  l< info->getNumLevel(); l++){
            TIFFClose(layers->at(l));
        }
    }

    /// \brief Get the writer name
    /// \return Writer name
    std::string getName() override { return "TiledTiffWriter"; }

    ITask<Tile<T>, Tile<T>> *copy() override {
        return new WriteTiffTileTask(this->getNumThreads(), this->_pathOut, this->pyramidName, this->imageDepth, this->info);
    }

private:
    const uint8 nbPyramidLevel = 0;
    const std::string pyramidName;
    const std::string _pathOut;
    const ImageDepth imageDepth;
    const GridGenerator *info;
    std::vector<TIFF*> *layers;


};

#endif //PYRAMIDBUILDING_WRITETIFFTILETASK_H
