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
#include "../utils/Helper.h"
#include "../rules/WriteTileRule.h"
#include "../tasks/Write8UPngTileTask.h"
#include "../rules/PyramidRule.h"
#include "../tasks/CreateTileTask.h"
#include "../tasks/BaseTileTask.h"
#include "../utils/MatrixAllocator.h"
#include "../data/Tile.h"
#include <htgs/log/TaskGraphSignalHandler.hpp>
#define uint64 uint64_hack_
#define int64 int64_hack_
#include <tiffio.h>
#undef uint64
#undef int64
#include <assert.h>
#include "../data/TileRequest.h"
#include "../utils/SingleTiledTiffWriter.h"
#include "../utils/GridGenerator.h"
#include "../utils/BaseTileGenerator.h"
#include "../tasks/WriteDeepZoomTileTask.h"
#include "../rules/DeepZoomDownsamplingRule.h"
#include "./Datatype.h"
#include <string>
#include <sstream>

//HTGS Graph
//Create tileRequest
//Task1 : (BlockRequest, Tile) base level tile generation : generate individual pyramid tiles
//Task2 : (Tile) Bookeeper : receive the pyramid tile and cache it. Generate request for higher level when it can.
//Task3 : (Block, Tile) CreateDownscaledTile : create tile at higher level of the pyramid.
//Task 4 : write the Tile;
class PyramidBuilding {

public:

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
        BlendingMethod blendingMethod = BlendingMethod::MAX;
        std::string pyramidName = "output";
        uint32_t overlap = 0;
        ImageDepth depth = ImageDepth::_16U;

    };

    PyramidBuilding(std::string inputDirectory, std::string stitching_vector, Options* options) : _inputDir(inputDirectory), _inputVector(stitching_vector), options(options) {};

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


    template<typename px_t>
    void _build(){

        size_t nbThreadsPerTask = 10;

        auto begin = std::chrono::high_resolution_clock::now();

        std::string pyramidName = options->getPyramidName();
        uint32_t pyramidTileSize = options->getTilesize();
        uint32_t overlap = 0;
        std::string format = "png";


        auto gridGenerator = new GridGenerator(_inputDir, _inputVector, options->getTilesize());

        auto grid = gridGenerator->getGrid();


        size_t numTileRow = gridGenerator->getGridMaxRow() + 1;
        size_t numTileCol = gridGenerator->getGridMaxCol() + 1;


        size_t fullFovWidth = gridGenerator->getFullFovWidth();
        size_t fullFovHeight = gridGenerator->getFullFovHeight();
        int deepZoomLevel = 0;
        //calculate pyramid depth
        auto maxDim = std::max(fullFovWidth,fullFovHeight);
        deepZoomLevel = int(ceil(log2(maxDim)) + 1);


        auto graph = new htgs::TaskGraphConf<TileRequest, Tile<px_t>>();

        auto generator = new BaseTileGenerator<px_t>(gridGenerator, BlendingMethod::MAX);
        auto baseTileTask = new BaseTileTask<px_t>(nbThreadsPerTask, generator);

        auto bookkeeper = new htgs::Bookkeeper<Tile<px_t>>();

        auto writeRule = new WriteTileRule<px_t>();

        auto pyramidRule = new PyramidRule<px_t>(numTileCol,numTileRow);

        auto createTileTask = new CreateTileTask<px_t>(nbThreadsPerTask);

        htgs::ITask< Tile<px_t>, Tile<px_t>> *writeTask = nullptr;

        if(this->options->getPyramidFormat() == PyramidFormat::DEEPZOOM) {
            writeTask = new WriteDeepZoomTileTask<px_t>(nbThreadsPerTask, pyramidName + "_files", deepZoomLevel, this->options->getDepth());
        }
        graph->setGraphConsumerTask(baseTileTask);

        //incoming edges from the bookeeper
        graph->addEdge(baseTileTask, bookkeeper); //pyramid base level tile
        graph->addEdge(createTileTask,bookkeeper); //pyramid higher level tile

        //outgoing edges
        graph->addRuleEdge(bookkeeper, pyramidRule, createTileTask); //caching tiles and creating a tile at higher level;

        //TODO CHECK for now we link to the writeTask but do not use it. We could.
        // If large latency in write, it could be worthwhile. Otherwise thread management will dominate.
        if(this->options->getPyramidFormat() == PyramidFormat::DEEPZOOM) {
            auto deepzoomDownsamplingRule = new DeepZoomDownsamplingRule<px_t>(numTileCol, numTileRow, deepZoomLevel,
                                                                               pyramidName + "_files", this->options->getDepth());
            graph->addRuleEdge(bookkeeper, deepzoomDownsamplingRule,
                               writeTask); //generating extra tiles up to 1x1 pixel to satisfy deepzoom format
        }

        graph->addRuleEdge(bookkeeper, writeRule, writeTask); //exiting the graph;

        //output task
        graph->addGraphProducerTask(writeTask);

        auto *runtime = new htgs::TaskGraphRuntime(graph);

        htgs::TaskGraphSignalHandler::registerTaskGraph(graph);
        htgs::TaskGraphSignalHandler::registerSignal(SIGTERM);

        runtime->executeRuntime();

        size_t numberBlockHeight,numberBlockWidth = 0;

        numberBlockHeight = static_cast<size_t>(ceil((double)numTileRow/2));
        numberBlockWidth = static_cast<size_t>(ceil((double)numTileCol/2));

        //we traverse the grid in blocks to minimize memory footprint of the pyramid generation.
    for(size_t j = 0; j < numberBlockHeight; j++){
            for(size_t i = 0; i < numberBlockWidth; i++){
                if(2*i < numTileCol && 2*j < numTileRow) {
                    std::cout << 2*j << "," << 2*i << std::endl;
                    auto tileRequest = new TileRequest(2 * j, 2 * i);
                    graph->produceData(tileRequest);
                }
                if(2*i+1 < numTileCol) {
                    std::cout << 2 * j << "," << 2 * i + 1 << std::endl;
                    auto tileRequest = new TileRequest(2 * j, 2 * i + 1);
                    graph->produceData(tileRequest);
                }

                if(2*j+1 < numTileRow) {
                    std::cout << 2 * j + 1 << "," << 2 * i << std::endl;
                    auto tileRequest = new TileRequest(2 * j + 1, 2 * i);
                    graph->produceData(tileRequest);
                }

                if(2*j+1 < numTileRow && 2*i+1 < numTileCol) {
                    std::cout << 2 * j + 1 << "," << 2 * i + 1 << std::endl;
                    auto tileRequest = new TileRequest(2 * j + 1, 2 * i + 1);
                    graph->produceData(tileRequest);
                }
            }
        }


        graph->finishedProducingData();

    while(!graph->isOutputTerminated()){
            auto r = graph->consumeData();
            if(r == nullptr){
                break;
            }
            //  std::cout << "output : " << r->getLevel() << ": " << r->getRow() << "," << r->getCol() << std::endl;
        }

        if(this->options->getPyramidFormat() == PyramidFormat::DEEPZOOM) {
            std::ostringstream oss;
            oss << "<?xml version=\"1.0\" encoding=\"utf-8\"?><Image TileSize=\"" << pyramidTileSize << "\" Overlap=\""
                << overlap
                << "\" Format=\"" << format << "\" xmlns=\"http://schemas.microsoft.com/deepzoom/2008\"><Size Width=\""
                << gridGenerator->getFullFovWidth() << "\" Height=\"" << gridGenerator->getFullFovHeight()
                << "\"/></Image>";

            std::ofstream outFile;
            outFile.open(pyramidName + ".dzi");
            outFile << oss.str();
            outFile.close();
        }



        std::cout << "we should be done" << std::endl;
        runtime->waitForRuntime();

        graph->writeDotToFile("graph", DOTGEN_FLAG_SHOW_ALL_THREADING | DOTGEN_COLOR_COMP_TIME);

    delete runtime;
    delete gridGenerator;
    delete generator;

        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " mS" << std::endl;

        }



protected:
    std::string _inputDir;
    std::string _inputVector;
    Options* options;


};


#endif //PYRAMIDBUILDING_PYRAMIDBUILDING_H
