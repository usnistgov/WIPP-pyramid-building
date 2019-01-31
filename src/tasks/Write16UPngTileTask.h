//
// Created by Gerardin, Antoine D. (Assoc) on 12/19/18.
//

#ifndef PYRAMIDBUILDING_WRITETILETASK_H
#define PYRAMIDBUILDING_WRITETILETASK_H

#include <htgs/api/ITask.hpp>
#include "FastImage/api/FastImage.h"
#include "../data/Tile.h"
#include <tiffio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include "../utils/SingleTiledTiffWriter.h"
#include "WriteTileTask.h"

template <class T>
class Write16UPngTileTask : public WriteTileTask<T> {

public:


    Write16UPngTileTask(size_t numThreads, const std::string &_pathOut) :  WriteTileTask<T>(numThreads, _pathOut) {}

    Write16UPngTileTask(const std::string &_pathOut) : Write16UPngTileTask(1, _pathOut) {}


    void executeTask(std::shared_ptr<Tile<T>> data) override {

        WriteTileTask<T>::executeTask(data);

        std::string level = std::to_string(data->getLevel());
        auto outputFilename = "img_r" + std::to_string(data->getRow()) + "_c" + std::to_string(data->getCol()) + ".png";
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
        return new Write16UPngTileTask(this->getNumThreads(), this->_pathOut);
    }

};

#endif //PYRAMIDBUILDING_WRITETILETASK_H
