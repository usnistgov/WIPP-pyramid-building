//
// Created by Gerardin, Antoine D. (Assoc) on 1/19/19.
//

#ifndef PYRAMIDBUILDING_TIFFWRITER_H
#define PYRAMIDBUILDING_TIFFWRITER_H

#include <string>
#include <iostream>

#define uint64 uint64_hack_
#define int64 int64_hack_
#include <tiffio.h>
#undef uint64
#undef int64

class SingleTiledTiffWriter {

public:

    /**
     * Create a 32bits tiled tiff with only one tile the size of the whole image.
     * Useful to generate the level 0 of a pyramid with one image per pyramid tile.
     * @param filePath the path where to write the image
     * @param size the size of the image.
     */
    SingleTiledTiffWriter(std::string filePath, uint32_t size= 1024) : size(size) {
            auto output = filePath.c_str();
            tif = TIFFOpen(output, "w");
            if(tif!= nullptr) {
                    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, size);
                    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, size);
                    TIFFSetField(tif, TIFFTAG_TILELENGTH, size);
                    TIFFSetField(tif, TIFFTAG_TILEWIDTH, size);
                    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 32);
                    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
                    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1);
                    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
                    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
                    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
                    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
            }
            else {
                    std::cerr << "The File " << output << " can't be opened." << std::endl;
                    exit(1);
            }
    };

        ~SingleTiledTiffWriter() {
                TIFFClose(tif);
        }

    void write(uint32_t* tile) {
        TIFFWriteTile(tif, tile, 0, 0, 0, 0);
        TIFFClose(tif);
    }


private:
    TIFF* tif;
    uint32_t size;


};

#endif //PYRAMIDBUILDING_TIFFWRITER_H
