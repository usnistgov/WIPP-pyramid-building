//
// Created by Gerardin, Antoine D. (Assoc) on 1/16/19.
//



#include <string>
#include <iostream>


#include "experimental/filesystem"
#include "testBasePerf.h"

int main()
{
    std::string directory = "/home/gerardin/Documents/images/dataset5_big/images/";

    auto begin = std::chrono::high_resolution_clock::now();

//    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset01/images/";
//    auto filename = "test01-tiled.tif";

    if(! filesystem::exists(filesystem::current_path() / "testPerfOutput")) {
        filesystem::create_directory(filesystem::current_path() / "testPerfOutput");
    }

    auto fovReader = new FOVReader<uint16_t>();

//    auto directory = filesystem::current_path().string() + "/testPerfOutput/";


    for (const auto & entry : filesystem::directory_iterator(directory)) {
        std::cout << entry.path() << std::endl;
        std::string file = entry.path().string();
        std::cout << file << std::endl;

    // fovReader->readFOV(file.c_str());
        auto outputFile = filesystem::current_path() / "testPerfOutput/" / entry.path().filename();
        fovReader->readWriteFOV(file.c_str(), outputFile.string());
    //    fovReader->readAndForgetFOV(file.c_str());
    }
    delete fovReader;

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " mS" << std::endl;


    return 0;
}

//for (uint32_t rowTile = 0; rowTile < tileHeight; ++rowTile ){
//std::copy_n(tile + rowTile * tileWidth, tileWidth, region );
//}