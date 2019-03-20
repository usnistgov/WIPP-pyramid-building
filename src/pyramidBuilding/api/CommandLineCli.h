//
// Created by Gerardin, Antoine D. (Assoc) on 2/25/19.
//

#ifndef PYRAMIDBUILDING_COMMANDLINECLI_H
#define PYRAMIDBUILDING_COMMANDLINECLI_H

#include <string>
#include <iostream>
#include <algorithm>
#include <glog/logging.h>

#include <tclap/CmdLine.h>

#include "PyramidBuilding.h"


namespace pb {

    bool hasEnding(std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }

    ImageDepth parseImageDepth(const std::string depth) {
        if (depth == "16U") {
            return ImageDepth::_16U;
        } else if (depth == "8U") {
            return ImageDepth::_8U;
        } else {
            throw TCLAP::ArgException("image depth not recognized. Should  be one of : 8U, 16U");
        }
    }


    BlendingMethod parseBlendingMethod(const std::string blending) {
        if (blending == "max") {
            return BlendingMethod::MAX;
        } else if (blending == "overlay") {
            return BlendingMethod::OVERLAY;
        } else {
            throw TCLAP::ArgException("blending method not recognized. Should  be one of : overlay, max");
        }
    }


    int pyramidBuilding(int argc, const char **argv) {

        google::InitGoogleLogging(argv[0]);

        try {
            TCLAP::CmdLine cmd("Pyramid Building", ' ', "1.0");

            TCLAP::ValueArg<std::string> inputDirectoryArg("i", "images", "input images directory", true, "",
                                                           "nameString");
            cmd.add(inputDirectoryArg);

            TCLAP::ValueArg<std::string> inputVectorArg("v", "vector", "stitching vector (global positions)", true, "",
                                                        "nameString");
            cmd.add(inputVectorArg);

            TCLAP::ValueArg<std::string> outputDirectoryArg("o", "output", "output directory", true, "", "nameString");
            cmd.add(outputDirectoryArg);

            //TODO CHECK. Let PyramidBuilding take care of defaults?
            TCLAP::ValueArg<int> tilesizeArg("t", "tilesize", "Tile Size", false, 256, "int");
            cmd.add(tilesizeArg);

            TCLAP::ValueArg<std::string> nameArg("n", "name", "Pyramid Name", false, "output", "string");
            cmd.add(nameArg);

            TCLAP::ValueArg<std::string> depthArg("d", "depth", "Image Depth", false, "16U", "string");
            cmd.add(depthArg);

            //TODO CHECK. unused. Check best way to handle different format. PyramidBuilding expects DeepZoom.
            TCLAP::ValueArg<std::string> formatArg("f", "format", "Image Format", false, "png", "string");
            cmd.add(formatArg);

            TCLAP::ValueArg<std::string> blendingArg("b", "blending", "Blending Method", false, "overlay", "string");
            cmd.add(blendingArg);

            cmd.parse(argc, argv);

            std::string inputDir = inputDirectoryArg.getValue();
            std::string outputDir = outputDirectoryArg.getValue();
            std::string inputVector = inputVectorArg.getValue();
            std::string pyramidName = nameArg.getValue();
            std::string format = formatArg.getValue();
            std::string depth = depthArg.getValue();
            std::string blending = blendingArg.getValue();
            //TODO check if TCLAP accept uint32_t
            uint32_t tilesize = tilesizeArg.getValue();

            if (!hasEnding(inputDir, "/")) {
                inputDir += "/";
            }

            VLOG(1) << inputDirectoryArg.getDescription() << ": " << inputDir << std::endl;
            VLOG(1) << outputDirectoryArg.getDescription() << ": " << outputDir << std::endl;
            VLOG(1) << inputVectorArg.getDescription() << ": " << inputVector << std::endl;
            VLOG(1) << tilesizeArg.getDescription() << ": " << tilesize << std::endl;
            VLOG(1) << nameArg.getDescription() << ": " << pyramidName << std::endl;
            VLOG(1) << formatArg.getDescription() << ": " << format << std::endl;
            VLOG(1) << depthArg.getDescription() << ": " << depth << std::endl;
            VLOG(1) << blendingArg.getDescription() << ": " << blending << std::endl;

            ImageDepth d = parseImageDepth(depth);
            BlendingMethod b = parseBlendingMethod(blending);


            auto *options = new PyramidBuilding::Options();
            options->setTilesize(tilesize);
            options->setPyramidName(pyramidName);
            options->setOverlap(0);
            options->setPyramidFormat(PyramidFormat::DEEPZOOM);
            options->setDownsamplingType(DownsamplingType::NEIGHBORS_AVERAGE);
            options->setBlendingMethod(b);
            options->setDepth(d);

            auto builder = new PyramidBuilding(inputDir, inputVector, outputDir, options);
            builder->build();


        } catch (TCLAP::ArgException &e)  // catch any exceptions
        {
            DLOG(FATAL) << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        }

        exit(0);


    }

}

#endif //PYRAMIDBUILDING_COMMANDLINECLI_H
