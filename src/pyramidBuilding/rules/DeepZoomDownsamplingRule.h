//
// Created by Gerardin, Antoine D. (Assoc) on 12/20/18.
//

#ifndef PYRAMIDBUILDING_DEEPZOOMDOWNSAMPLINGRULE_H
#define PYRAMIDBUILDING_DEEPZOOMDOWNSAMPLINGRULE_H

#include <FastImage/api/FastImage.h>
#include <math.h>
#include <array>
#include <assert.h>
#include <sys/stat.h>
#include <experimental/filesystem>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include "../data/Tile.h"
#include "../data/BlockRequest.h"
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
    class DeepZoomDownsamplingRule : public htgs::IRule<Tile<T>, Tile<T>> {

    public:
        DeepZoomDownsamplingRule(size_t numTileCol, size_t numTileRow, int maxDeepZoomLevel,
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

        std::string getName() override {
            return "DeepZoomDownsampling Rule";
        }

        void applyRule(std::shared_ptr<Tile<T>> data, size_t pipelineId) override {

            size_t level = data->getLevel();


            if (level == this->numLevel - 1) {

                T* originalData = data->getData();

                size_t width = data->get_width();
                size_t height = data->get_height();

                T* newTileData = new T[width * height];

                for(uint32_t i = 0; i < (width * height); i++){
                    newTileData[i] = originalData[i];
                }

                //TODO level should be int
                for (int i = (int) this->numLevel; i < maxDeepZoomLevel; i++) {

                    int l = this->maxDeepZoomLevel - 1 - i;
                    std::string level_string = std::to_string(l);

                    filesystem::path path = _pathOut;
                    auto dirPath = path / level_string;
                    if (!filesystem::exists(dirPath)) {
                        filesystem::create_directories(dirPath);
                    }

                    if (l > 1) {
                        auto res = downsampler->downsample(newTileData, width, height);
                        //TODO valgrind still detect a leak of a few bytes here
                        delete[] newTileData;
                        newTileData = res;
                    }

                    width = static_cast<size_t>(ceil((double) width / 2));
                    height = static_cast<size_t>(ceil((double) height / 2));

                    auto outputFilename =
                            std::to_string(data->getCol()) + "_" + std::to_string(data->getRow()) + ".png";
                    auto fullImagePath = dirPath / outputFilename;


                    //TODO CHECK how to clean up? Plus need to be factored
                    switch (imageDepth) {
                        case ImageDepth::_16U : {
                            cv::Mat image(height, width, CV_16U, newTileData);
                            cv::imwrite(fullImagePath.string(), image);
                            image.release();
                            break;
                        }
                        case ImageDepth::_8U : {
                            cv::Mat image(height, width, CV_8U, newTileData);
                            cv::imwrite(fullImagePath.string(), image);
                            image.release();
                            break;
                        }
                    }

                }

                done = true;
                return;
            }

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

#endif //PYRAMIDBUILDING_DEEPZOOMDOWNSAMPLINGRULE_H
