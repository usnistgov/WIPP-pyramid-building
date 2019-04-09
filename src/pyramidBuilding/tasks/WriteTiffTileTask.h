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
#include "pyramidBuilding/api/OptionsType.h"
#include "pyramidBuilding/utils/StitchingVectorParser.h"

#include <experimental/filesystem>

namespace pb {

using namespace std::experimental;

template <class T>
class WriteTiffTileTask : public htgs::ITask< Tile<T>, htgs::VoidData > {

public:
    WriteTiffTileTask(
            size_t numThreads, const std::string &_pathOut, const std::string &pyramidName,
            const ImageDepth imageDepth, const StitchingVectorParser *gridGenerator) :
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


        for(size_t l = 0;  l< info->getNumLevel(); l++){
            auto fullPath = path / (pyramidName + ".tif");
            auto file = fullPath.c_str();

            if(l == 0){
                _tiff = TIFFOpen(file, "w");
            }

            TIFFSetField(_tiff, TIFFTAG_IMAGEWIDTH, info->getFullFovWidthAtLevel(l));
            TIFFSetField(_tiff, TIFFTAG_IMAGELENGTH, info->getFullFovHeightAtLevel(l));
            TIFFSetField(_tiff, TIFFTAG_TILELENGTH, info->getPyramidTileSize());
            TIFFSetField(_tiff, TIFFTAG_TILEWIDTH, info->getPyramidTileSize());
            TIFFSetField(_tiff, TIFFTAG_BITSPERSAMPLE,bitsPerSample);
            TIFFSetField(_tiff, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
            TIFFSetField(_tiff, TIFFTAG_ROWSPERSTRIP, 1);
            TIFFSetField(_tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
            TIFFSetField(_tiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
            TIFFSetField(_tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
            TIFFSetField(_tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

            auto buf = new T[info->getFullFovWidthAtLevel(l) * info->getFullFovHeightAtLevel(l)];
            TIFFWriteTile(_tiff, (tdata_t)buf, 0, 0, 0, 0);
            delete[] buf;
            TIFFWriteDirectory(_tiff);
        }
    }


    void executeTask(std::shared_ptr<Tile<T>> data) override {

        size_t tileSize = info->getPyramidTileSize();
        size_t x = data->getCol() * tileSize;
        size_t y = data->getRow() * tileSize;
        size_t level = data->getLevel();

        size_t originalWidth = data->get_width();

        auto dir = TIFFSetDirectory(_tiff, (uint16_t)level);

        if(dir != 1){
            DLOG(FATAL) << "error while trying to access tiff directory " << level << std::endl;
        }
        TIFF* tiff = _tiff;

        VLOG(3) << "printing tile in directory : " << std::to_string(dir) << std::endl;

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

        TIFFRewriteDirectory(_tiff);

        if(data->getRow() == info->getGridMaxRow(level) && data->getCol() == info->getGridMaxCol(level)){
            TIFFCheckpointDirectory(_tiff);
        }

        this->addResult(data);
    }

    /// \brief Close the tiff file
    void shutdown() override {
        TIFFClose(_tiff);
    }

    /// \brief Get the writer name
    /// \return Writer name
    std::string getName() override { return "PyramidalTiledTiffWriter"; }

    ITask<Tile<T>, VoidData> *copy() override {
        return new WriteTiffTileTask(this->getNumThreads(), this->_pathOut, this->pyramidName, this->imageDepth, this->info);
    }

private:
    const uint8 nbPyramidLevel = 0;
    const std::string pyramidName;
    const std::string _pathOut;
    const ImageDepth imageDepth;
    const StitchingVectorParser *info;
    TIFF* _tiff;


};

}

#endif //PYRAMIDBUILDING_WRITETIFFTILETASK_H
