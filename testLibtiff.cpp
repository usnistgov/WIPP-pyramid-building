//
// Created by Gerardin, Antoine D. (Assoc) on 1/16/19.
//



#include <string>
#include <tiffio.h>


int main()
{
    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramidBuilding/resources/dataset1/tiled-images/";
    auto filename = "img_r001_c001.ome.tif";

    auto file = (directory + filename).c_str();
    TIFF* tif = TIFFOpen(file, "r");

    https://www.awaresystems.be/imaging/tiff/tifftags/sampleformat.html

    if (tif) {
        tdata_t buf;
        ttile_t tile;

        buf = _TIFFmalloc(TIFFTileSize(tif));
        for (tile = 0; tile < TIFFNumberOfTiles(tif); tile++)
            TIFFReadEncodedTile(tif, tile, buf, (tsize_t) -1);

        _TIFFfree(buf);
        TIFFClose(tif);
    }

    return 0;
}