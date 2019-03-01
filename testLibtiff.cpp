//
// Created by Gerardin, Antoine D. (Assoc) on 1/16/19.
//



#include <string>
#include "experimental/filesystem"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#define uint64 uint64_hack_
#define int64 int64_hack_
#include <tiffio.h>
#include <iostream>

#undef uint64
#undef int64

using namespace std::experimental;

int main()
{
    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset03/images/";
    auto filename = "tiled16_tracer.tiff";

//    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset01/images/";
//    auto filename = "test01-tiled.tif";

    auto file = (directory + filename).c_str();
    TIFF* tif = TIFFOpen(file, "r");

    uint32_t width, height, tileWidth, tileHeight, samplePerPixel, bitsPerSample, sampleFormat, tileX, tileY;

    // Load/parse header
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tileWidth);
    TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileHeight);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplePerPixel);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
    TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sampleFormat);



    if(! filesystem::exists(filesystem::current_path() / "testLibTiff")) {
        filesystem::create_directory(filesystem::current_path() / "testLibTiff");
    }

    //https://www.awaresystems.be/imaging/tiff/tifftags/sampleformat.html
    uint8_t *region = new uint8_t[ width * height ]();

    if (tif) {
        tdata_t buf;
        uint32_t tileSize =TIFFTileSize(tif);
        buf = _TIFFmalloc(tileSize);
//        for (tile = 0; tile < TIFFNumberOfTiles(tif); tile++) {
//            TIFFReadEncodedTile(tif, tile, buf, (tsize_t) -1);
        for (tileY = 0; tileY < height; tileY += tileHeight){
            for (tileX = 0; tileX < width; tileX += tileWidth) {
                TIFFReadTile(tif, buf, tileX, tileY, 0, 0);
              //  std::copy_n((int8_t *) buf, tileWidth * tileHeight, region + tileY * tileWidth);
                for (uint32_t row = 0; row < tileHeight; ++row) {
                    for (uint32_t col = 0; col < tileWidth; ++col) {
                      //  std::cout << std::setw(5) << (uint32_t)((uint8_t*)buf)[row * tileWidth + col] << " ";
                        uint32_t y = tileY + row;
                        uint32_t x = tileX + col;
                        uint32_t index = y * width + (tileX + col);
                        uint8_t value = ((uint8_t*)buf)[row * tileWidth + col];
                 //       std::cout << index << ":" << (uint32_t)  value << std::endl;
                        if(y<height && x < width) { //those values are not defined for tiles at the border (tile's dimensions are fixed).
                            region[index] = value;
                        }
                    }
                    std::cout << "\n";
                }
                std::cout << std::endl;
            }
        }

        _TIFFfree(buf);
        TIFFClose(tif);

        cv::Mat image(height, width, CV_8U, region);
        std::string fullImagePath = (filesystem::current_path() / "testLibTiff" / "output").string() + ".png";
        cv::imwrite(fullImagePath, image);
    }

    return 0;
}

//for (uint32_t rowTile = 0; rowTile < tileHeight; ++rowTile ){
//std::copy_n(tile + rowTile * tileWidth, tileWidth, region );
//}