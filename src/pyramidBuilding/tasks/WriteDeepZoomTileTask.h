//
// Created by Gerardin, Antoine D. (Assoc) on 12/19/18.
//

#ifndef PYRAMIDBUILDING_WRITEDEEPZOOMTILETASK_H
#define PYRAMIDBUILDING_WRITEDEEPZOOMTILETASK_H

#include <htgs/api/ITask.hpp>
#include "FastImage/api/FastImage.h"
#include "../data/Tile.h"
#include <tiffio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include "pyramidBuilding/api/OptionsType.h"

#include <experimental/filesystem>

namespace pb {

using namespace std::experimental;

    template <class T>
    class WriteDeepZoomTileTask : public htgs::ITask< Tile<T>, Tile<T> > {

    public:


        WriteDeepZoomTileTask(size_t numThreads, const std::string &_pathOut, const int nbPyramidLevel,
                const ImageDepth imageDepth) : htgs::ITask<Tile<T>, Tile<T>>(numThreads),
        _pathOut(_pathOut), nbPyramidLevel(nbPyramidLevel), imageDepth(imageDepth) {

            //create the images directory structure
            filesystem::path path = _pathOut;

            if(! filesystem::exists(path)) {
                    filesystem::create_directories(path);
            }

            for(int i= 0; i < nbPyramidLevel; i++){
                if(! filesystem::exists(path / std::to_string(i))) {
                    filesystem::create_directories(path / std::to_string(i));
                }
            }

        }

        void executeTask(std::shared_ptr<Tile<T>> data) override {

            filesystem::path path = _pathOut;

            //deepzoom base level is the maximum depth of the pyramid
            //TODO change level. uint8 is enough!
            int l = this->nbPyramidLevel - 1 - (int)data->getLevel();
            std::string level = std::to_string(l);

            auto dirPath = path / level;
            auto outputFilename =  std::to_string(data->getCol()) + "_" + std::to_string(data->getRow()) + ".png";
            auto fullImagePath = dirPath / outputFilename;

            VLOG(2) << "write tile (" << data->getRow() << "," << data->getCol()  << ") at level " << data->getLevel() <<
            " (deepzoom level " << level << ")"  << std::endl;

            //TODO CHECK how to clean up? Plus need to be factored
            switch(imageDepth){

                case ImageDepth::_16U : {
                    cv::Mat image(data->get_height(), data->get_width(), CV_16U, data->getData());
                    cv::imwrite(fullImagePath.string(), image);
                    image.release();
                    break;
                }
                case ImageDepth::_8U : {
                    cv::Mat image(data->get_height(), data->get_width(), CV_8U, data->getData());
                    cv::imwrite(fullImagePath.string(), image);
                    image.release();
                    break;
                }
            }

            this->addResult(data);
        }

        /// \brief Close the tiff file
        void shutdown() override {
        }

        /// \brief Get the writer name
        /// \return Writer name
        std::string getName() override { return "DeepZoomWriteTask"; }

        ITask<Tile<T>, Tile<T>> *copy() override {
            return new WriteDeepZoomTileTask(this->getNumThreads(), this->_pathOut, this->nbPyramidLevel, this->imageDepth);
        }

    private:
        const uint8 nbPyramidLevel = 0;
        const std::string _pathOut;
        const ImageDepth imageDepth;

    };

}

#endif //PYRAMIDBUILDING_WRITEDEEPZOOMTILETASK_H
