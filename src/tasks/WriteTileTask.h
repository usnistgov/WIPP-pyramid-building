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

class WriteTileTask : public htgs::ITask< Tile<uint32_t>, Tile<uint32_t> > {

public:

    WriteTileTask(const std::string &_pathOut, uint32_t pyramidTileSize) : _pathOut(_pathOut),
                                                                           pyramidTileSize(pyramidTileSize) {


        auto dir = opendir(_pathOut.c_str());

        if(dir == nullptr){
            const int dir_err = mkdir(_pathOut.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (-1 == dir_err)
            {
                printf("Error creating directory!n");
                exit(1);
            }
        }
    }

    void executeTask(std::shared_ptr<Tile<uint32_t>> data) override {

        std::string level = std::to_string(data->getLevel());

        auto dirPath = (_pathOut + "/" + std::to_string(data->getLevel()));

        std::cout << dirPath << std::endl;

        auto dir = opendir(dirPath.c_str());

        if(dir == nullptr){
            const int dir_err = mkdir(dirPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (-1 == dir_err)
            {
                printf("Error creating directory!n");
                exit(1);
            }
        }

        //write as a tif output
        auto outputFilename = "img_r" + std::to_string(data->getRow()) + "_c" + std::to_string(data->getCol()) + ".tif";

        auto fullImagePath = _pathOut + "/" + level + "/"  + outputFilename;

        cv::Mat image(data->get_height(), data->get_width(), CV_32SC1, data->getData());
        cv::Mat tmp(data->get_height(), data->get_width(), CV_16U);
        image.convertTo(tmp, CV_16U, 1,0);
        cv::imwrite(fullImagePath + ".png", tmp);

//        auto w = new SingleTiledTiffWriter(fullImagePath, pyramidTileSize);
//        w->write(data->getData());

        addResult(data);
    }

    /// \brief Close the tiff file
    void shutdown() override {
    }

    /// \brief Get the writer name
    /// \return Writer name
    std::string getName() override { return "WriteTask"; }

    ITask<Tile<uint32_t>, Tile<uint32_t>> *copy() override {
        return new WriteTileTask(_pathOut, pyramidTileSize);
    }

private:

    std::string _pathOut;
    uint32_t pyramidTileSize;

};

#endif //PYRAMIDBUILDING_WRITETILETASK_H
