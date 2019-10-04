//
// Created by Gerardin, Antoine D. (Assoc) on 2/25/19.
//

#ifndef PYRAMIDBUILDING_PYRAMIDBUILDING_H
#define PYRAMIDBUILDING_PYRAMIDBUILDING_H

//
// Created by Gerardin, Antoine D. (Assoc) on 2/25/19.
//

#include <iostream>
#include <FastImage/api/FastImage.h>
#include <FastImage/TileLoaders/GrayscaleTiffTileLoader.h>
#include <htgs/log/TaskGraphSignalHandler.hpp>
#define uint64 uint64_hack_
#define int64 int64_hack_
#include <tiffio.h>
#undef uint64
#undef int64
#include <assert.h>
#include <string>
#include <sstream>
#include <experimental/filesystem>
#include <glog/logging.h>
#include <pyramidBuilding/utils/AverageDownsampler.h>
#include <pyramidBuilding/memory/TileAllocator.h>
#include "pyramidBuilding/tasks/DeepZoomTileWriter.h"
#include "pyramidBuilding/rules/DeepZoomDownsampleTileRule.h"
#include "OptionsType.h"
#include "pyramidBuilding/tasks/PyramidalTiffWriter.h"
#include "pyramidBuilding/utils/Utils.h"
#include "pyramidBuilding/rules/WriteTileRule.h"
#include "pyramidBuilding/rules/PyramidCacheRule.h"
#include "pyramidBuilding/tasks/TileDownsampler.h"
#include "pyramidBuilding/data/Tile.h"
#include "pyramidBuilding/utils/AverageDownsampler.h"
#include <pyramidBuilding/pyramid/PyramidBuilder.h>
#include <pyramidBuilding/pyramid/fastImage/PyramidTileLoader.h>
#include <pyramidBuilding/tasks/TileResizer.h>
#include <pyramidBuilding/pyramid/RecursiveBlockTraversal.h>
#include <mem/Mem.h>

namespace pb {

    using namespace std::experimental;

    /***
     *  @class The pyramid building algorithm.
     *  @brief The HTGS graph that captures the pyramid building algorithm.
     */
    class PyramidBuilding {

    public:

        /**
         * @class Options
         * @brief internal class that defines all the options to configure the pyramid building.
         */
        class Options {

        public:
            uint32_t getTilesize() const {
                return _tilesize;
            }

            void setTilesize(uint32_t tilesize) {
                _tilesize = tilesize;
            }

            DownsamplingType getDownsamplingType() const {
                return downsamplingType;
            }

            void setDownsamplingType(DownsamplingType downsamplingType) {
                Options::downsamplingType = downsamplingType;
            }

            PyramidFormat getPyramidFormat() const {
                return pyramidFormat;
            }

            void setPyramidFormat(PyramidFormat pyramidFormat) {
                Options::pyramidFormat = pyramidFormat;
            }

            const std::string &getPyramidName() const {
                return pyramidName;
            }


            BlendingMethod getBlendingMethod() const {
                return blendingMethod;
            }

            void setBlendingMethod(BlendingMethod blendingMethod) {
                Options::blendingMethod = blendingMethod;
            }


            void setPyramidName(const std::string &pyramidName) {
                Options::pyramidName = pyramidName;
            }

            uint32_t getOverlap() const {
                return overlap;
            }

            void setOverlap(uint32_t overlap) {
                Options::overlap = overlap;
            }

            ImageDepth getDepth() const {
                return depth;
            }

            void setDepth(ImageDepth depth) {
                Options::depth = depth;
            }

        private:

            uint32_t _tilesize = 1024;
            DownsamplingType downsamplingType = DownsamplingType::NEIGHBORS_AVERAGE;
            PyramidFormat pyramidFormat = PyramidFormat::DEEPZOOM;
            BlendingMethod blendingMethod = BlendingMethod::OVERLAY;
            std::string pyramidName = "output";
            uint32_t overlap = 0;
            ImageDepth depth = ImageDepth::_16U;

        };

        class ExpertModeOptions {

        public:

           explicit  ExpertModeOptions(const std::map<std::string, size_t> &options = {}) : options(
                    options) {}

            size_t get(const std::string &key){
                return options[key];
            }

            bool has(const std::string &key){
                return options.find(key) != options.end();
           }

        private:
            std::map<std::string,size_t> options;
        };

        /***
         * Pyramid Building
         * @param inputDirectory the directory where the FOVs are stored.
         * @param stitching_vector the stitching vector representing the position of each FOV in the global coordinates
         * of the full FOV.
         * @param outputDirectory in which the pyramid will be generated.
         * @param options
         */
        PyramidBuilding(const std::string &inputDirectory,
                        const std::string &stitching_vector,
                        const std::string &outputDirectory,
                        Options* options) :
                _inputDir(inputDirectory), _inputVector(stitching_vector), _outputDir(outputDirectory), options(options) {
            if(!std::experimental::filesystem::exists(inputDirectory)) {
                throw std::invalid_argument("Images directory does not exists. Was : " + inputDirectory);
            }
            if(!std::experimental::filesystem::exists(stitching_vector)) {
                throw std::invalid_argument("Stitching vector does not exists. Was : " + stitching_vector);
            }
            if(!std::experimental::filesystem::exists(outputDirectory)) {
                VLOG(1) << "WARNING - Output directory does not exists. It will be created : " + outputDirectory;
                filesystem::create_directories(outputDirectory);
            }
        };


        void setExpertModeOptions(ExpertModeOptions *expertModeOptions) {
            this->expertModeOptions = expertModeOptions;
        }


        void build() {

            switch (this->options->getDepth()) {
                case ImageDepth::_8U:
                    _build<uint8_t>();
                    break;
                case ImageDepth::_16U:
                default:
                    _build<uint16_t>();
            };
        }


    private :


        template<typename px_t>
        void _build(){

            bool LOW_FOOTPRINT = false; //make that an option?

            size_t concurrentTiles = 4;
            size_t readerThreads = 5;
            size_t builderThreads =  1;
            size_t downsamplerThreads = 4;
            size_t writerThreads = 5;

            VLOG(1) << "generating pyramid..." << std::endl;

            if(expertModeOptions != nullptr){
                VLOG(3) << "expert mode flags" << std::endl;
                if(expertModeOptions->has("tile"))  concurrentTiles =  expertModeOptions->get("tile");
                if(expertModeOptions->has("reader")) readerThreads = expertModeOptions->get("reader");
                if(expertModeOptions->has("builder")) builderThreads = expertModeOptions->get("builder");
                if(expertModeOptions->has("downsampler")) downsamplerThreads = expertModeOptions->get("downsampler");
                if(expertModeOptions->has("writer")) writerThreads = expertModeOptions->get("writer");
                if(expertModeOptions->has("lowfootprint")) LOW_FOOTPRINT = (expertModeOptions->get("lowfootprint") == 1);

            }


            if(LOW_FOOTPRINT && concurrentTiles < 4 + builderThreads) {
                    concurrentTiles = 4 + builderThreads;
                    VLOG(1) << "WARNING : System needs to be configured with at least 4 concurrent tiles plus one additional"
                               "tile per tile resizer threads Set to :" << concurrentTiles;
            }
            else if ( concurrentTiles < 4 * builderThreads ) {
                    concurrentTiles = 4 * builderThreads;
                VLOG(1) << "WARNING : System needs more tiles to prevent deadlock. Set to :" << concurrentTiles;
            }


            VLOG(1) << "Execution model : " << std::endl;
            VLOG(1) << "reader threads : " << readerThreads  << std::endl;
            VLOG(1) << "concurrent tiles : " << concurrentTiles  << std::endl;
            VLOG(1) << "builder threads : " << builderThreads  << std::endl;
            VLOG(1) << "downsampler threads : " << downsamplerThreads  << std::endl;
            VLOG(1) << "writer threads : " << writerThreads  << std::endl;
            VLOG(1) << "low footprint (preserving order) : " << LOW_FOOTPRINT  << std::endl;

            auto begin = std::chrono::high_resolution_clock::now();

            std::string pyramidName = options->getPyramidName();
            uint32_t pyramidTileSize = options->getTilesize();
            uint32_t overlap = 0;
            std::string format = "png";

            //parse the stitching vector and create the problem representation
            auto pyramidBuilder = std::make_shared<PyramidBuilder>(_inputDir, _inputVector, pyramidTileSize);

            //INIT
            auto graph = new htgs::TaskGraphConf<VoidData, VoidData>();
            //basic info
            auto pyramid = pyramidBuilder->getPyramid();
            auto numTileRow = pyramid.getNumTileRow(0);
            auto numTileCol = pyramid.getNumTileCol(0);
            size_t fullFovWidth = pyramidBuilder->getFullFovWidth();
            size_t fullFovHeight = pyramidBuilder->getFullFovHeight();
            auto level = pyramid.getNumLevel();
            auto maxDim = std::max(fullFovWidth,fullFovHeight);
            auto deepZoomLevel = int(ceil(log2(maxDim)) + 1);
            VLOG(2) << "full FOV width : " << fullFovWidth  << std::endl;
            VLOG(2) << "full FOV height : " << fullFovHeight  << std::endl;
            VLOG(2) << "numTileRow : " << numTileRow  << std::endl;
            VLOG(2) << "numTileCol : " << numTileCol  << std::endl;

            //GENERATING TILES
            auto tiffImageLoader = new TiffImageLoader<px_t>(_inputDir, pyramidTileSize);
            auto tileLoader = new PyramidTileLoader<px_t>(readerThreads, pyramidBuilder, tiffImageLoader, pyramidTileSize);
            auto *fi = new fi::FastImage<px_t>(tileLoader, 0);
            fi->getFastImageOptions()->setNumberOfViewParallel((uint32_t)concurrentTiles);
            fi->getFastImageOptions()->setNumberOfTilesToCache((uint32_t)concurrentTiles);
            if(LOW_FOOTPRINT){
                fi->getFastImageOptions()->setPreserveOrder(true);
            }
            auto fastImage = fi->configureAndMoveToTaskGraphTask("Fast Image");

            //RESIZING TILES
            auto tileResizer = new TileResizer<px_t>(builderThreads,pyramidTileSize, pyramidBuilder);
            graph->addEdge(fastImage, tileResizer);

            //CACHE
            auto bookkeeper = new htgs::Bookkeeper<Tile<px_t>>();
            graph->addEdge(tileResizer, bookkeeper);
            auto writeRule = new WriteTileRule<px_t>();
            auto pyramidRule = new PyramidCacheRule<px_t>(numTileCol,numTileRow);

            //DOWNSAMPLING TILES
            auto downsampler = new AverageDownsampler<px_t>();
            auto tileDownsampler = new TileDownsampler<px_t>(downsamplerThreads, downsampler);
            graph->addRuleEdge(bookkeeper, pyramidRule, tileDownsampler); //caching tiles and creating a tile at higher level;
            graph->addEdge(tileDownsampler,bookkeeper); //pyramid higher level tile

            //WRITING TILE
            htgs::ITask< Tile<px_t>, htgs::VoidData> *writeTask = nullptr;
            if(this->options->getPyramidFormat() == PyramidFormat::DEEPZOOM) {
                auto outputPath = filesystem::path(_outputDir) / (pyramidName + "_files");
                writeTask = new DeepZoomTileWriter<px_t>(writerThreads, outputPath, deepZoomLevel, this->options->getDepth());
            }
            graph->addRuleEdge(bookkeeper, writeRule, writeTask);

            //DOWNSAMPLING LAST TILE FOR DEEPZOOM COMPATIBILITY
            if(this->options->getPyramidFormat() == PyramidFormat::DEEPZOOM) {
                auto outputPath = filesystem::path(_outputDir) / (pyramidName + "_files");
                auto deepzoomDownsamplingRule = new DeepZoomDownsampleTileRule<px_t>(numTileCol, numTileRow, deepZoomLevel,
                                                                                   outputPath, this->options->getDepth(), downsampler);
                graph->addRuleEdge(bookkeeper, deepzoomDownsamplingRule,
                                   writeTask); //generating extra tiles up to 1x1 pixel to satisfy deepzoom format
            }

            auto writeRule2 = new WriteTileRule<px_t>();
            auto tiledTiffWriteTask = new PyramidalTiffWriter<px_t>(1,_outputDir, pyramidName, options->getDepth(), pyramid);
            graph->addRuleEdge(bookkeeper, writeRule2, tiledTiffWriteTask);

            //MEMORY MANAGEMENT
            graph->addMemoryManagerEdge("basetile",tileResizer, new TileAllocator<px_t>(pyramidTileSize , pyramidTileSize),concurrentTiles, htgs::MMType::Dynamic);
            //dimension to not deadlock for each branch of recursive block traversal
            auto tileCacheSize = (3 * (pyramid.getNumLevel() -1 ) + 1 ) * concurrentTiles / 4;
            VLOG(3) << "nb of higher level tile available in tile cache: " << tileCacheSize;
            graph->addMemoryManagerEdge("tile",tileDownsampler, new TileAllocator<px_t>(pyramidTileSize , pyramidTileSize), tileCacheSize , htgs::MMType::Dynamic);


            //DEBUG
            htgs::TaskGraphSignalHandler::registerTaskGraph(graph);
            htgs::TaskGraphSignalHandler::registerSignal(SIGTERM);

            auto *runtime = new htgs::TaskGraphRuntime(graph);

#ifdef NDEBUG
#else
//            htgs::TaskGraphSignalHandler::registerTaskGraph(graph);
//                htgs::TaskGraphSignalHandler::registerSignal(SIGTERM);
#endif


            runtime->executeRuntime();

            //REQUEST TILES
            auto traversal = new RecursiveBlockTraversal(pyramid);
            for(auto step : traversal->getTraversal()){
                auto row = step.first;
                auto col = step.second;
//                VLOG(3) << row << "," << col;
                fi->requestTile(row,col,false,0);
            }
            fi->finishedRequestingTiles();
            graph->finishedProducingData();

            //GENERATING DEEPZOOM METADATA FILE
            if(this->options->getPyramidFormat() == PyramidFormat::DEEPZOOM) {
                std::ostringstream oss;
                oss << R"(<?xml version="1.0" encoding="utf-8"?><Image TileSize=")" << pyramidTileSize << "\" Overlap=\""
                    << overlap
                    << "\" Format=\"" << format << R"(" xmlns="http://schemas.microsoft.com/deepzoom/2008"><Size Width=")"
                    << pyramidBuilder->getFullFovWidth() << "\" Height=\"" << pyramidBuilder->getFullFovHeight()
                    << "\"/></Image>";

                std::ofstream outFile;
                outFile.open(filesystem::path(_outputDir) / (pyramidName + ".dzi"));
                outFile << oss.str();
                outFile.close();
            }

            runtime->waitForRuntime();

            VLOG(1) << "done generating pyramid." << std::endl;

            graph->writeDotToFile("colorGraph.xdot", DOTGEN_COLOR_COMP_TIME);
#ifdef NDEBUG
#else
            graph->writeDotToFile("graph", DOTGEN_COLOR_COMP_TIME);
#endif

            delete runtime;
            delete downsampler;
            delete tiffImageLoader;

            auto end = std::chrono::high_resolution_clock::now();
            VLOG(1) << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " mS" << std::endl;

        }



        std::string _inputDir;
        std::string _inputVector;
        std::string _outputDir;
        Options* options;
        ExpertModeOptions* expertModeOptions = nullptr;


    };

}

#endif //PYRAMIDBUILDING_PYRAMIDBUILDING_H
