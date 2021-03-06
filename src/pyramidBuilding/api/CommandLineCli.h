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

//TODO do we need to wrap those namespaced global methods in another namespace or class to avoid name collision and
//produce intelligible documentation?
/***
 * @brief Collection of methods to parse a command line into inputs expected by the pyramid building algorithm.
 */
namespace pb {

    bool hasEnding(std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }

    ImageDepth parseImageDepth(const std::string &depth) {
        if (depth == "16U") {
            return ImageDepth::_16U;
        } else if (depth == "8U") {
            return ImageDepth::_8U;
        } else {
            throw std::invalid_argument("image depth not recognized. Should  be one of : 8U, 16U");
        }
    }

    PyramidFormat parseFormat(const std::string &format) {
        if (format == "deepzoom") {
            return PyramidFormat::DEEPZOOM;
        } else if (format == "tiff") {
            return PyramidFormat::PYRAMIDAL_TIFF;
        } else if (format == "all") {
            return PyramidFormat::DEEPZOOM_AND_PYRAMIDAL_TIFF;
        } else {
            throw std::invalid_argument("format not recognized. Should  be one of : deepzoom, tiff, all");
        }
    }


    BlendingMethod parseBlendingMethod(const std::string &blending) {
        if (blending == "max") {
            return BlendingMethod::MAX;
        } else if (blending == "overlay") {
            return BlendingMethod::OVERLAY;
        } else {
            throw std::invalid_argument("blending method not recognized. Should  be one of : overlay, max");
        }
    }

    std::map<std::string,size_t> parseExpertMode(std::string &expertMode) {

        std::map<std::string,size_t> flags = {};

        std::string flagDelimiter = ";";
        std::string valueDelimiter = "=";

        size_t pos = 0;
        std::string flag;
        do {
            pos = expertMode.find(flagDelimiter);
            flag = expertMode.substr(0, pos);
            size_t pos2 = flag.find(valueDelimiter);
            if(pos2 != std::string::npos){
                auto key = flag.substr(0,pos2);
                auto value = flag.substr(pos2 + valueDelimiter.size(),std::string::npos);
                flags[key] = static_cast<size_t>(std::stoul(value,nullptr,10));
            }
            expertMode.erase(0, pos + flagDelimiter.length());
        }
        while(pos != std::string::npos);
        return flags;
    }


    int pyramidBuilding(int argc, const char **argv) {

        google::InitGoogleLogging(argv[0]);

        try {
            TCLAP::CmdLine cmd("Pyramid Building", ' ', "1.0");

            TCLAP::ValueArg<std::string> inputDirectoryArg("i", "images", "input images directory", true, "",
                                                           "filePath");
            cmd.add(inputDirectoryArg);

            TCLAP::ValueArg<std::string> inputVectorArg("v", "vector", "stitching vector (global positions)", true, "",
                                                        "filePath");
            cmd.add(inputVectorArg);

            TCLAP::ValueArg<std::string> outputDirectoryArg("o", "output", "output directory", true, "outputs", "filePath");
            cmd.add(outputDirectoryArg);

            TCLAP::ValueArg<uint32_t> tilesizeArg("t", "tilesize", "Tile Size", false, 1024, "uint32_t");
            cmd.add(tilesizeArg);

            TCLAP::ValueArg<std::string> nameArg("n", "name", "Pyramid Name", false, "pyramid", "filename");
            cmd.add(nameArg);

            TCLAP::ValueArg<std::string> depthArg("d", "depth", "Image Depth", false, "16U", "string");
            cmd.add(depthArg);

            TCLAP::ValueArg<std::string> formatArg("f", "format", "Output Format", false, "deepzoom", "string");
            cmd.add(formatArg);

            TCLAP::ValueArg<std::string> blendingArg("b", "blending", "Blending Method", false, "overlay", "string");
            cmd.add(blendingArg);

            TCLAP::ValueArg<std::string> expertModeArg("e", "expertmode", "Expert mode", false, "", "string");
            cmd.add(expertModeArg);

            cmd.parse(argc, argv);

            std::string inputDir = inputDirectoryArg.getValue();
            std::string outputDir = outputDirectoryArg.getValue();
            std::string inputVector = inputVectorArg.getValue();
            std::string pyramidName = nameArg.getValue();
            std::string format = formatArg.getValue();
            std::string depth = depthArg.getValue();
            std::string blending = blendingArg.getValue();
            uint32_t tilesize = tilesizeArg.getValue();
            std::string expertMode = expertModeArg.getValue();

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
            VLOG(1) << expertModeArg.getDescription() << ": " << blending << std::endl;

            ImageDepth d = parseImageDepth(depth);
            BlendingMethod b = parseBlendingMethod(blending);
            auto f = parseFormat(format);
            auto expertModeOptions = new PyramidBuilding::ExpertModeOptions(parseExpertMode(expertMode));


            auto *options = new PyramidBuilding::Options();
            options->setTilesize(tilesize);
            options->setPyramidName(pyramidName);
            options->setOverlap(0);
            options->setPyramidFormat(f);
            options->setDownsamplingType(DownsamplingType::NEIGHBORS_AVERAGE);
            options->setBlendingMethod(b);
            options->setDepth(d);

            auto builder = new PyramidBuilding(inputDir, inputVector, outputDir, options);

            builder->setExpertModeOptions(expertModeOptions);

            builder->build();


        } catch (TCLAP::ArgException &e)  // catch any exceptions
        {
            DLOG(FATAL) << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        }

        exit(0);

    }

}

#endif //PYRAMIDBUILDING_COMMANDLINECLI_H
