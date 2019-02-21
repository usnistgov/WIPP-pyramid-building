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
#include "WriteTileTask.h"

template <class T>
class WriteDeepZoomTileTask : public htgs::ITask< Tile<T>, Tile<T> > {

public:


    WriteDeepZoomTileTask(size_t numThreads, const std::string &_pathOut, const int maxPyramidLevel) : htgs::ITask<Tile<T>, Tile<T>>(numThreads), _pathOut(_pathOut), maxPyramidLevel(maxPyramidLevel) {

        auto dir = opendir(_pathOut.c_str());
        if(dir == nullptr){
            const int dir_err = mkdir(_pathOut.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (-1 == dir_err)
            {
                printf("Error creating output directory!n");
                exit(1);
            }
        }
        else {
            closedir(dir);
        }
    }


    void executeTask(std::shared_ptr<Tile<T>> data) override {


        //TODO change level. uint8 is enough!
        int l = this->maxPyramidLevel - (int)data->getLevel();
        std::string level = std::to_string(l);

        auto dirPath = (this->_pathOut + "/" + level);

        //  Create directory if it does not exists
        auto dir = opendir(dirPath.c_str());
        if(dir == nullptr){
            const int dir_err = mkdir(dirPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (-1 == dir_err)
            {
                // std::cout << "Error creating output directory for level" << level  << "!n";
                exit(1);
            }
        }
        else {
            closedir(dir);
        }


        //auto outputFilename = "img_r" + std::to_string(data->getRow()) + "_c" + std::to_string(data->getCol()) + ".png";
        auto outputFilename = std::to_string(data->getRow()) + "_" + std::to_string(data->getCol()) + ".png";
        auto fullImagePath = this->_pathOut + "/" + level + "/"  + outputFilename;

        //TODO CHECK how this can vary with the template
        //TODO CHECK how opencv deals with the input array
        cv::Mat image(data->get_height(), data->get_width(), CV_8UC1, data->getData());
//        cv::Mat image(data->get_height(), data->get_width(), CV_32SC1, data->getData());
        //TODO CHECK if opencv can convert directly from 32U to 16U
      //  cv::Mat tmp(data->get_height(), data->get_width(), CV_16U);
     //   image.convertTo(tmp, CV_16U, 1,0);
     //   cv::imwrite(fullImagePath, tmp);

     //   tmp.release();
        cv::imwrite(fullImagePath, image);
        image.release();

        this->addResult(data);
    }

    /// \brief Close the tiff file
    void shutdown() override {
    }

    /// \brief Get the writer name
    /// \return Writer name
    std::string getName() override { return "PngTileWriteTask"; }

    ITask<Tile<T>, Tile<T>> *copy() override {
        return new WriteDeepZoomTileTask(this->getNumThreads(), this->_pathOut, this->maxPyramidLevel);
    }

private:
    const uint8 maxPyramidLevel = 0;
    const std::string _pathOut;

};

#endif //PYRAMIDBUILDING_WRITEDEEPZOOMTILETASK_H
