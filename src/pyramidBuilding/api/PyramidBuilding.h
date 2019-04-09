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
#include <pyramidBuilding/utils/BaseTileGeneratorFastImage.h>
#include <pyramidBuilding/utils/AverageDownsampler.h>
#include <pyramidBuilding/utils/BaseTileGeneratorLibTiffWithCache.h>
#include <pyramidBuilding/utils/StitchingVectorParser.h>
#include <pyramidBuilding/data/FOVRequest.h>

#include "pyramidBuilding/data/TileRequest.h"
#include "pyramidBuilding/utils/StitchingVectorParserOld.h"
#include "pyramidBuilding/utils/BaseTileGeneratorLibTiff.h"
#include "pyramidBuilding/tasks/WriteDeepZoomTileTask.h"
#include "pyramidBuilding/rules/DeepZoomDownsamplingRule.h"
#include "OptionsType.h"
#include "pyramidBuilding/tasks/WriteTiffTileTask.h"
#include "pyramidBuilding/utils/Helper.h"
#include "pyramidBuilding/rules/WriteTileRule.h"
#include "pyramidBuilding/rules/PyramidRule.h"
#include "pyramidBuilding/tasks/CreateTileTask.h"
#include "pyramidBuilding/tasks/BaseTileTask.h"
#include "pyramidBuilding/data/Tile.h"
#include "../utils/AverageDownsampler.h"

namespace pb {

    using namespace std::experimental;

    /***
     *  @class The pyramid building algorithm.
     *  @brief The HTGS graph that captures the pyramid building algorithm.
     *  @details
     *  HTGS Graph
     *   Create tileRequest
     *   Task1 : (BlockRequest, Tile) base level tile generation : generate individual pyramid tiles
     *   Task2 : (Tile) Bookeeper : receive the pyramid tile and cache it. Generate request for higher level when it can.
     *   Task3 : (Block, Tile) CreateDownscaledTile : create tile at higher level of the pyramid.
     *   Task 4 : write the Tile;
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

            uint32_t _tilesize = 256;
            DownsamplingType downsamplingType = DownsamplingType::NEIGHBORS_AVERAGE;
            PyramidFormat pyramidFormat = PyramidFormat::DEEPZOOM;
            BlendingMethod blendingMethod = BlendingMethod::OVERLAY;
            std::string pyramidName = "output";
            uint32_t overlap = 0;
            ImageDepth depth = ImageDepth::_16U;

        };

        /***
         * Pyramid Building
         * @param inputDirectory the directory where the FOVs are stored.
         * @param stitching_vector the stitching vector representing the position of each FOV in the global coordinates
         * of the full FOV.
         * @param outputDirectory in which the pyramid will be generated.
         * @param options
         */
        PyramidBuilding(std::string inputDirectory,
                        std::string stitching_vector,
                        std::string outputDirectory,
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

            VLOG(1) << "generating pyramid...";

            size_t nbThreadsPerTask = 10;

            auto begin = std::chrono::high_resolution_clock::now();

            std::string pyramidName = options->getPyramidName();
            uint32_t pyramidTileSize = options->getTilesize();
            uint32_t overlap = 0;
            std::string format = "png";


            auto gridGenerator = new StitchingVectorParser(_inputDir, _inputVector);

            auto grid = gridGenerator->getGrid();

            size_t numTileRow = gridGenerator->getMaxRow() + 1;
            size_t numTileCol = gridGenerator->getMaxCol() + 1;

            auto graph = new htgs::TaskGraphConf<FOVRequest, VoidData>();


//            size_t fullFovWidth = gridGenerator->getFullFovWidth();
//            size_t fullFovHeight = gridGenerator->getFullFovHeight();
//            int deepZoomLevel = 0;
//            //calculate pyramid depth
//            auto maxDim = std::max(fullFovWidth,fullFovHeight);
//            deepZoomLevel = int(ceil(log2(maxDim)) + 1);
//
//
//
//            auto generator = new BaseTileGeneratorLibTiffWithCache<px_t>(gridGenerator, this->options->getBlendingMethod());
//            auto baseTileTask = new BaseTileTask<px_t>(1, generator);
//            graph->setGraphConsumerTask(baseTileTask);
//
//            auto bookkeeper = new htgs::Bookkeeper<Tile<px_t>>();
//
//            auto writeRule = new WriteTileRule<px_t>();
//            auto pyramidRule = new PyramidRule<px_t>(numTileCol,numTileRow);
//
//            auto downsampler = new AverageDownsampler<px_t>();
//            auto createTileTask = new CreateTileTask<px_t>(1, downsampler);
//
//            //incoming edges from the bookeeper
//            graph->addEdge(baseTileTask, bookkeeper); //pyramid base level tile
//            graph->addEdge(createTileTask,bookkeeper); //pyramid higher level tile
//            graph->addRuleEdge(bookkeeper, pyramidRule, createTileTask); //caching tiles and creating a tile at higher level;
//
//            htgs::ITask< Tile<px_t>, htgs::VoidData> *writeTask = nullptr;
//            if(this->options->getPyramidFormat() == PyramidFormat::DEEPZOOM) {
//                auto outputPath = filesystem::path(_outputDir) / (pyramidName + "_files");
//                writeTask = new WriteDeepZoomTileTask<px_t>(nbThreadsPerTask, outputPath, deepZoomLevel, this->options->getDepth());
//            }
//            graph->addRuleEdge(bookkeeper, writeRule, writeTask); //exiting the graph;
//
//
//            //TODO CHECK for now we link to the writeTask but do not use it. We could.
//            // If large latency in write, it could be worthwhile. Otherwise thread management will dominate.
//            if(this->options->getPyramidFormat() == PyramidFormat::DEEPZOOM) {
//                auto outputPath = filesystem::path(_outputDir) / (pyramidName + "_files");
//                auto deepzoomDownsamplingRule = new DeepZoomDownsamplingRule<px_t>(numTileCol, numTileRow, deepZoomLevel,
//                                                                                   outputPath, this->options->getDepth(), downsampler);
//                graph->addRuleEdge(bookkeeper, deepzoomDownsamplingRule,
//                                   writeTask); //generating extra tiles up to 1x1 pixel to satisfy deepzoom format
//            }
//
//        //    auto tiledTiffWriteTask = new WriteTiffTileTask<px_t>(1,_outputDir, pyramidName, options->getDepth(), gridGenerator);
//        //    graph->addRuleEdge(bookkeeper, writeRule, tiledTiffWriteTask);


            //output task
           // graph->addGraphProducerTask(writeTask);

            auto *runtime = new htgs::TaskGraphRuntime(graph);

            #ifdef NDEBUG
            #else
                htgs::TaskGraphSignalHandler::registerTaskGraph(graph);
                htgs::TaskGraphSignalHandler::registerSignal(SIGTERM);
            #endif


            runtime->executeRuntime();

//           blockTraversal(graph, numTileRow, numTileCol);
       //   diagTraversal(graph, numTileRow, numTileCol);
     //     recursiveTraversal<px_t>(graph, numTileRow, numTileCol, (size_t)0, (size_t)0);

            fi::Traversal traversal = fi::Traversal(fi::TraversalType::DIAGONAL,numTileRow,numTileCol);



            for (auto step : traversal.getTraversal()) {
                auto row = step.first, col = step.second;
                auto request = new FOVRequest(row,col);
                VLOG(3) <<  "fov request : " << "(" << row << "," << col << ")"<< std::endl;
                graph->produceData(request);
            }


            graph->finishedProducingData();

//            while(!graph->isOutputTerminated()){
//                auto r = graph->consumeData();
//                if(r == nullptr){
//                    break;
//
//                }
//
//                VLOG(3) << "tile output needs to be consumed to release shared pointer!";
//
//            }


            if(this->options->getPyramidFormat() == PyramidFormat::DEEPZOOM) {
                std::ostringstream oss;
                oss << R"(<?xml version="1.0" encoding="utf-8"?><Image TileSize=")" << pyramidTileSize << "\" Overlap=\""
                    << overlap
                    << "\" Format=\"" << format << R"(" xmlns="http://schemas.microsoft.com/deepzoom/2008"><Size Width=")"
                        << gridGenerator->getFullFovWidth() << "\" Height=\"" << gridGenerator->getFullFovHeight()
                    << "\"/></Image>";

                std::ofstream outFile;
                outFile.open(filesystem::path(_outputDir) / (pyramidName + ".dzi"));
                outFile << oss.str();
                outFile.close();
            }

//TODO REMOVE only when caching is enabled
//            VLOG(3) << "read count : " << generator->getFovsCache()->readCount << std::endl;
       //     VLOG(3) << "total number of  reads necessary : " << gridGenerator->getCounter() << std::endl;

            runtime->waitForRuntime();

            VLOG(1) << "done generating pyramid." << std::endl;

            graph->writeDotToFile("graph", DOTGEN_COLOR_COMP_TIME);
            #ifdef NDEBUG
            #else
                graph->writeDotToFile("graph", DOTGEN_COLOR_COMP_TIME);
            #endif

            delete runtime;
            delete gridGenerator;
//            delete generator;
//            delete downsampler;

            auto end = std::chrono::high_resolution_clock::now();
            VLOG(1) << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " mS" << std::endl;

            }

        template<class px_t>
        void recursiveTraversal(htgs::TaskGraphConf<TileRequest, Tile<px_t>>* graph, size_t totalNumTileRow, size_t totalNumTileCol, size_t numTileRow, size_t numTileCol) {

            if(numTileCol == totalNumTileCol && numTileRow == totalNumTileRow){
                return;
            }

            auto min = std::min(totalNumTileRow - numTileRow, totalNumTileCol - numTileCol);

            for (auto i = 0; i < min; i++) {
                for (auto j = 0; j < i; j++) {
                    auto row = numTileRow + i;
                    auto col = numTileCol + j;
                    auto tileRequest = new TileRequest(row, col);
                    VLOG(2) <<  "(" << row << "," << col << ")" << std::endl;
                    graph->produceData(tileRequest);
                }
                for (auto j = 0; j <= i; j++) {
                    auto row = numTileRow + j;
                    auto col = numTileCol + i;
                    auto tileRequest = new TileRequest(row, col);
                    VLOG(2) <<  "(" << row << "," << col << ")"<< std::endl;
                    graph->produceData(tileRequest);
                }
            }

            auto colLeft = totalNumTileCol - numTileCol;
            auto rowLeft = totalNumTileRow - numTileRow;


            if( colLeft > rowLeft){
                recursiveTraversal(graph, totalNumTileRow, totalNumTileCol, numTileRow, numTileCol + min);
            }
            else if( colLeft < rowLeft){
                recursiveTraversal(graph, totalNumTileRow, totalNumTileCol, numTileRow + min, numTileCol);
            }
            else {
                recursiveTraversal(graph, totalNumTileRow, totalNumTileCol, numTileRow + min, numTileCol + min);
            }
        }

        template<class px_t>
        void blockTraversal(htgs::TaskGraphConf<TileRequest, Tile<px_t>>* graph, size_t numTileRow, size_t numTileCol) {

            size_t numberBlockHeight,numberBlockWidth = 0;

            numberBlockHeight = static_cast<size_t>(ceil((double)numTileRow/2));
            numberBlockWidth = static_cast<size_t>(ceil((double)numTileCol/2));

            //we traverse the grid in blocks to minimize memory footprint of the pyramid generation.
            for(size_t j = 0; j < numberBlockHeight; j++){
                for(size_t i = 0; i < numberBlockWidth; i++){
                    if(2*i < numTileCol && 2*j < numTileRow) {
                        VLOG(2) << "requesting tile (" << 2*j << "," << 2*i << ")" << std::endl;
                        auto tileRequest = new TileRequest(2 * j, 2 * i);
                        graph->produceData(tileRequest);
                    }
                    if(2*i+1 < numTileCol) {
                        VLOG(2) << "requesting tile ("  << 2 * j << "," << 2 * i + 1 << ")" << std::endl;
                        auto tileRequest = new TileRequest(2 * j, 2 * i + 1);
                        graph->produceData(tileRequest);
                    }

                    if(2*j+1 < numTileRow) {
                        VLOG(2) << "requesting tile ("  << 2 * j + 1 << "," << 2 * i << ")" << std::endl;
                        auto tileRequest = new TileRequest(2 * j + 1, 2 * i);
                        graph->produceData(tileRequest);
                    }

                    if(2*j+1 < numTileRow && 2*i+1 < numTileCol) {
                        VLOG(2) << "requesting tile ("  << 2 * j + 1 << "," << 2 * i + 1 << ")" << std::endl;
                        auto tileRequest = new TileRequest(2 * j + 1, 2 * i + 1);
                        graph->produceData(tileRequest);
                    }
                }
            }
        }

        template<class px_t>
        void diagTraversal(htgs::TaskGraphConf<TileRequest, Tile<px_t>>* graph, size_t numTileRow, size_t numTileCol){
            auto min = std::min(numTileRow, numTileCol);

            for (size_t i = 0; i < min; i++) {
                for (size_t j = 0; j < i; j++) {
                    auto tileRequest = new TileRequest(i, j);
                    VLOG(2) <<  "(" << i << "," << j << ")" << std::endl;
                    graph->produceData(tileRequest);
                }
                for (auto j = 0; j <= i; j++) {
                    auto tileRequest = new TileRequest(j, i);
                    VLOG(2) <<  "(" << j << "," << i << ")"<< std::endl;
                    graph->produceData(tileRequest);
                }
            }

            if(numTileRow > numTileCol){
                for(size_t i = numTileCol; i < numTileRow; i++ ){
                    for(size_t j = 0; j < numTileCol; j++){
                        auto tileRequest = new TileRequest(i, j);
                        VLOG(2) <<  "(" << i << "," << j << ")"<< std::endl;
                        graph->produceData(tileRequest);
                    }
                }
            }
            else {
                for(size_t i = numTileRow; i < numTileCol; i++ ){
                    for(size_t j = 0; j < numTileRow; j++){
                        auto tileRequest = new TileRequest(j, i);
                        VLOG(2) <<  "(" << j << "," << i << ")"<< std::endl;
                        graph->produceData(tileRequest);
                    }
                }
            }
        }

        std::string _inputDir;
        std::string _inputVector;
        std::string _outputDir;
        Options* options;


    };

}

#endif //PYRAMIDBUILDING_PYRAMIDBUILDING_H
