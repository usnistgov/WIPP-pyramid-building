//
// Created by Gerardin, Antoine D. (Assoc) on 12/20/18.
//

#ifndef PYRAMIDBUILDING_DEEPZOOMDOWNSAMPLETILERULE_H
#define PYRAMIDBUILDING_DEEPZOOMDOWNSAMPLETILERULE_H

#include <FastImage/api/FastImage.h>
#include <math.h>
#include <array>
#include <assert.h>
#include <sys/stat.h>
#include <experimental/filesystem>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include "../data/Tile.h"
#include "pyramidBuilding/data/TileBlock.h"
#include "pyramidBuilding/api/OptionsType.h"
#include "../utils/Downsampler.h"


namespace pb {

    using namespace std::experimental;

/***
 * @class DeepZoomDownsamplingRule DeepZoomDownsamplingRule.h <pyramidBuilding/rules/DeepZoomDownsamplingRule.h>
 *
 * @brief generates extra level required by deepzoom.
 *
 * @details This task is executed only once when a data is received fulfilling the condition (level == this->numLevel -1).
 * The reason is that deepzoom expects extra pyramid levels.
 * Deepzoom needs the downsampling to continue until a 1x1 pixel tile is generated.
 * We downsample the top level tile, halving its dimension and adding a new level then repeat the process until we generate a 1x1 tile.
 *
 * @tparam T The depth of the output image.
 */
    template<class T>
    class DeepZoomDownsampleTileRule : public htgs::IRule<Tile<T>, Tile<T>> {

    public:
        DeepZoomDownsampleTileRule(size_t numTileCol, size_t numTileRow, int maxDeepZoomLevel,
                                 const std::string &_pathOut,
                                 ImageDepth imageDepth,
                                 Downsampler<T>* downsampler
                                 ) :
                numTileCol(numTileCol), numTileRow(numTileRow), maxDeepZoomLevel(maxDeepZoomLevel), _pathOut(_pathOut),
                imageDepth(imageDepth), downsampler(downsampler) {

            //calculate pyramid depth
            auto maxDim = std::max(numTileCol, numTileRow);
            numLevel = static_cast<size_t>(ceil(log2(maxDim)) + 1);

        }

        void applyRule(std::shared_ptr<Tile<T>> data, size_t pipelineId) override {



            size_t level = data->getLevel();

            //We receive the tile at the top of a regular pyramid. We generate deepZoom pyramid extra level
            // by downsampling this tile over and over until we generate a 1x1 pixel tile.
            if (level == this->numLevel - 1) {

                VLOG(3) << "downsampling last tile...";
//                printArray("last tile downsampling", data->getData(), data->getWidth(), data->getHeight());

                T* originalData = data->getData();
                T* newTileData = nullptr;

                size_t width = data->getWidth();
                size_t height = data->getHeight();

                int levelLeft = (int)(this -> maxDeepZoomLevel -  this->numLevel);

                for (int i = levelLeft -1 ; i >= 0; i--) {

                    auto newWidth = static_cast<size_t>(ceil((double) width / 2));
                    auto newHeight = static_cast<size_t>(ceil((double) height / 2));


                    newTileData = new T[newWidth * newHeight]();
                    downsampler->downsample(newTileData, originalData, width, height);

                    width = newWidth;
                    height = newHeight;


                    auto originalLevel = (size_t)(this->maxDeepZoomLevel - 1 - i);

                    auto tile = new Tile<T>(originalLevel, 0, 0, width, height, newTileData);
                    originalData = newTileData;

                    this->addResult(tile);

//                    if(i == levelLeft -1 ){
//                        data->getMemoryData()->releaseMemory();
//                    }
                }


                done = true;
                return;
            }

        }


        std::string getName() override {
            return "DeepZoomDownsampleTileRule";
        }

        bool canTerminateRule(size_t pipelineId) override {
            return done;
        }


    private:
        size_t numTileCol = 0;
        size_t numTileRow = 0;
        size_t numLevel = 0;
        int maxDeepZoomLevel = 0;
        bool done = false;
        const std::string _pathOut;
        ImageDepth imageDepth;
        Downsampler<T>* downsampler;

    };

}

#endif //PYRAMIDBUILDING_DEEPZOOMDOWNSAMPLETILERULE_H
