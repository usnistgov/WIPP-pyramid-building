//
// Created by Gerardin, Antoine D. (Assoc) on 12/19/18.
//

#ifndef PYRAMIDBUILDING_DEEPZOOMTILEWRITER_H
#define PYRAMIDBUILDING_DEEPZOOMTILEWRITER_H

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
#include <pyramidBuilding/utils/Utils.h>

namespace pb {

using namespace std::experimental;

template <class T>
class DeepZoomTileWriter : public htgs::ITask< Tile<T>, htgs::VoidData > {

    public:


        DeepZoomTileWriter(size_t numThreads, const std::string &_pathOut, const size_t nbPyramidLevel,
                const ImageDepth imageDepth) : htgs::ITask<Tile<T>, htgs::VoidData>(numThreads),
        _pathOut(_pathOut), nbPyramidLevel(nbPyramidLevel), imageDepth(imageDepth) {

            //create the images directory structure
            filesystem::path path = _pathOut;

            if(! filesystem::exists(path)) {
                    filesystem::create_directories(path);
            }

            for(auto i= 0; i < nbPyramidLevel; i++){
                if(! filesystem::exists(path / std::to_string(i))) {
                    filesystem::create_directories(path / std::to_string(i));
                }
            }

        }



        void executeTask(std::shared_ptr<Tile<T>> data) override {

            filesystem::path path = _pathOut;

            //deepzoom levels are reversed. level 0 represent the top of the pyramid
            size_t l = this->nbPyramidLevel - 1 - data->getLevel();


            std::string level = std::to_string(l);
            auto dirPath = path / level;
            //also deepzoom stores tiles as col_row
            auto outputFilename =  std::to_string(data->getCol()) + "_" + std::to_string(data->getRow()) + ".png";
            auto fullImagePath = dirPath / outputFilename;

            VLOG(2) << "write tile (" << data->getRow() << "," << data->getCol()  << ") at level " << data->getLevel() <<
            " (deepzoom level " << level << ")"  << std::endl;

//            printArray("write",data->getData(), data->getWidth(), data->getHeight());

            switch(this->imageDepth){
                case ImageDepth::_16U : {
                    cv::Mat image(data->getHeight(), data->getWidth(), CV_16U, data->getData());
                    cv::imwrite(fullImagePath.string(), image);
                    image.release();
                    break;
                }
                case ImageDepth::_8U : {
                    cv::Mat image(data->getHeight(), data->getWidth(), CV_8U, data->getData());
                    cv::imwrite(fullImagePath.string(), image);
                    image.release();
                    break;
                }
            }

            if(data->getMemoryData() != nullptr){
                data->getMemoryData()->releaseMemory();
                VLOG(3) << "freeing tile : level " << level << "(" << data->getRow() << "," << data->getCol() << ")";
            }
        }

        /// \brief Close the tiff file
        void shutdown() override {
        }

        /// \brief Get the writer name
        /// \return Writer name
        std::string getName() override { return "DeepZoomTileWriter"; }

        ITask<Tile<T>, VoidData> *copy() override {
            return new DeepZoomTileWriter(this->getNumThreads(), this->_pathOut, this->nbPyramidLevel, this->imageDepth);
        }

    private:
        const size_t nbPyramidLevel = 0;
        const std::string _pathOut = "/tmp/";
        const ImageDepth imageDepth = ImageDepth::_8U;

    };

}

#endif //PYRAMIDBUILDING_DEEPZOOMTILEWRITER_H
