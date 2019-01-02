//
// Created by Gerardin, Antoine D. (Assoc) on 12/19/18.
//

#ifndef PYRAMIDBUILDING_WRITETILETASK_H
#define PYRAMIDBUILDING_WRITETILETASK_H

#include <htgs/api/ITask.hpp>
#include "FastImage/api/FastImage.h"
#include "../data/Tile.h"
#include <tiffio.h>

class WriteTileTask : public htgs::ITask< Tile<uint32_t>, Tile<uint32_t> > {

public:

    /// \brief Create the write task
    /// \param numThreads Number of writer in parallel
    /// \param pathOut Output file path
    /// \param imageWidth Image width
    /// \param imageHeight Image height
    /// \param tileWidth Tile width
    /// \param tileHeight Tile height
    WriteTileTask(const char *pathOut, uint32_t imageWidth, uint32_t imageHeight, uint32_t tileWidth,
              uint32_t tileHeight) :
            htgs::ITask< Tile<uint32_t>, Tile<uint32_t> >(1), _pathOut(pathOut), _imageWidth(imageWidth),
            _imageHeight(imageHeight), _tileWidth(tileWidth), _tileHeight(tileHeight) {}

    /// \brief Task initializer
    void initialize() override {
        // Initialize the tiff
        _tif = TIFFOpen(_pathOut, "w");
        if (_tif != nullptr) {
            TIFFSetField(_tif, TIFFTAG_IMAGEWIDTH, _imageWidth);
            TIFFSetField(_tif, TIFFTAG_IMAGELENGTH, _imageHeight);
            TIFFSetField(_tif, TIFFTAG_TILELENGTH, _tileHeight);
            TIFFSetField(_tif, TIFFTAG_TILEWIDTH, _tileWidth);
            TIFFSetField(_tif, TIFFTAG_BITSPERSAMPLE, 32);
            TIFFSetField(_tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);
            TIFFSetField(_tif, TIFFTAG_ROWSPERSTRIP, 1);
            TIFFSetField(_tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
            TIFFSetField(_tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
            TIFFSetField(_tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
            TIFFSetField(_tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
        } else {
            std::cerr << "The File " << _pathOut << " can't be opened." << std::endl;
            exit(1);
        }
    }

    void executeTask(std::shared_ptr<Tile<uint32_t>> data) override {


        //   std::cout << "write tile : " << data->get()->getRow() << "," << data->get()->getCol() << std::endl;
        // find out how to call the destructr    ~data();
      //  printf( "ref count for tile : %d %d %d :: %d \n", data->getRow() , data->getCol() , data->getLevel() , data.use_count());

      //cannot write at any level. Generate level 0 tiff then use vips?
     //   TIFFWriteTile(_tif, data->getData(), data->getRow(), data->getCol(), data->getLevel(), 0);

        if(data->getLevel() == 0){
   //         TIFFWriteTile(_tif, data->getData(), data->getRow(), data->getCol(), data->getLevel(), 0);
            if(data != nullptr) {
                data->getOrigin()->releaseMemory();
                std::ostringstream oss;
                oss << "count of tile release : " << ++count;
                std::cout << oss.str() << std::endl;
            }
        }
        addResult(data);
    }

    /// \brief Close the tiff file
    void shutdown() override {
        TIFFClose(_tif);
    }

    /// \brief Get the writer name
    /// \return Writer name
    std::string getName() override { return "WriteTask"; }

    /// \brief Copy  the writing task
    /// \return A new writing task copy of the first
    WriteTileTask *copy() override {
        return new WriteTileTask(_pathOut, _imageWidth, _imageHeight, _tileWidth, _tileHeight);
    }

    uint32_t count;

private:
    TIFF
            *_tif;        ///< Tiff file to write tile into

    const char
            *_pathOut;    ///< Tiff file path

    uint32_t
            _imageWidth,  ///< Image width
            _imageHeight, ///< image height
            _tileWidth,   ///< Tile width
            _tileHeight;  ///< Tile Height

};

#endif //PYRAMIDBUILDING_WRITETILETASK_H
