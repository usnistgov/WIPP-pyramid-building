//
// Created by Gerardin, Antoine D. (Assoc) on 2/25/19.
//

#ifndef PYRAMIDBUILDING_COMMANDLINECLI_H
#define PYRAMIDBUILDING_COMMANDLINECLI_H

#include <string>
#include <iostream>
#include <algorithm>
#include <tclap/CmdLine.h>
#include "PyramidBuilding.h"


bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

int pyramidBuilding(int argc, const char** argv)
{

    try {
        TCLAP::CmdLine cmd("Pyramid Building", ' ', "1.0");

        TCLAP::UnlabeledValueArg<std::string>  inputDirectoryArg( "directory", "input images directory", true, "","nameString"  );
        cmd.add( inputDirectoryArg );

        TCLAP::ValueArg<std::string>  inputVectorArg( "v", "vector", "stitching vector (global positions)", true, "","nameString"  );
        cmd.add(inputVectorArg);

        //TODO CHECK. Let PyramidBuilding take care of defaults?
        TCLAP::ValueArg<int> tilesizeArg("t","tilesize","Tile Size",false, 256, "int");
        cmd.add( tilesizeArg );

        TCLAP::ValueArg<std::string> nameArg("n","name","Pyramid Name",false,"output","string");
        cmd.add( nameArg );

        //TODO CHECK. unused. Check best way to handle different format. PyramidBuilding expects DeepZoom.
        TCLAP::ValueArg<std::string> formatArg("f","format","Image Format",false,"png","string");
        cmd.add( formatArg );

        cmd.parse( argc, argv );

        std::string inputDir = inputDirectoryArg.getValue();
        std::string inputVector = inputVectorArg.getValue();
        std::string pyramidName = nameArg.getValue();
        std::string format = formatArg.getValue();
        //TODO check if TCLAP accept uint32_t
        uint32_t tilesize = tilesizeArg.getValue();

        if(!hasEnding(inputDir,"/")){
            inputDir += "/";
        }

        std::cout << inputDirectoryArg.getDescription() << ": " << inputDir << std::endl;
        std::cout << inputVectorArg.getDescription() << ": "  << inputVector << std::endl;
        std::cout << tilesizeArg.getDescription() << ": "  << tilesize << std::endl;
        std::cout << nameArg.getDescription() << ": " << pyramidName << std::endl;
        std::cout << formatArg.getDescription() << ": " << format << std::endl;

        auto *options = new PyramidBuilding::Options();
        options->setTilesize(tilesize);
        options->setPyramidName(pyramidName);
        options->setOverlap(0);
        options->setPyramidFormat(PyramidFormat::DEEPZOOM);
        options->setDownsamplingType(DownsamplingType::NEIGHBORS_AVERAGE);
        auto builder = new PyramidBuilding(inputDir,inputVector,options);
        builder->build();


    } catch (TCLAP::ArgException &e)  // catch any exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        exit(1);
    }

    exit(0);


}

#endif //PYRAMIDBUILDING_COMMANDLINECLI_H
