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
#include "../data/Tile.h"
#include "../data/BlockRequest.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

template <class T>
class DeepZoomDownsamplingRule : public htgs::IRule<Tile<T>, Tile<T>> {

public:
    DeepZoomDownsamplingRule(size_t numTileCol, size_t numTileRow, int maxDeepZoomLevel, const std::string &_pathOut) :  numTileCol(numTileCol), numTileRow(numTileRow), maxDeepZoomLevel(maxDeepZoomLevel), _pathOut(_pathOut) {

        //calculate pyramid depth
        auto maxDim = std::max(numTileCol,numTileRow);
        numLevel = static_cast<size_t>(ceil(log2(maxDim)) + 1);

    }

    std::string getName() override {
        return "DeepZoomDownsampling Rule";
    }

    void applyRule(std::shared_ptr<Tile<T>> data, size_t pipelineId) override {

        size_t level = data->getLevel();


        if(level == this->numLevel -1){

            T* newTileData = data->getData();
            size_t width = data->get_width();
            size_t height = data->get_height();

            //TODO level should be int
            for(int i = (int)this->numLevel; i < maxDeepZoomLevel; i++){

                int l = this->maxDeepZoomLevel - 1 - i;
                std::string level = std::to_string(l);

                filesystem::path path = _pathOut;
                auto dirPath = path / level;
                if(! filesystem::exists(dirPath)) {
                    filesystem::create_directory(dirPath);
                }

                if(l > 1){
                    T *downsampledData = generateDownsampleData(newTileData, width, height);
                    newTileData = downsampledData;
                }

                //TODO CHECK conversion. Using size_t we ended up with problems since we had whole division behavior.Check the rest of the code.
                width = static_cast<size_t>(ceil((double) width / 2));
                height = static_cast<size_t>(ceil((double) height / 2));

                auto outputFilename =  std::to_string(data->getCol()) + "_" + std::to_string(data->getRow()) + ".png";
                auto fullImagePath = dirPath / outputFilename;
                //TODO CHECK how this can vary with the template
                //TODO CHECK how opencv deals with the input array
                cv::Mat image(height, width, CV_16U, newTileData);
                cv::imwrite(fullImagePath.string(), image);
                image.release();
            }

            done = true;
            return;
        }

    }

    bool canTerminateRule(size_t pipelineId) override {
        return done;
    }


private:


    T* generateDownsampleData(T* newTileData, size_t width, size_t height) {

        //TODO check this kind of conversion throughout
        //in particular : when we have size_t as inputs, how do we ensure there is no overflow
        auto downsampleWidth = static_cast<size_t>(ceil( (double)width / 2));
        auto downsampleHeight = static_cast<size_t>(ceil( (double)height / 2));

        T* downsampleData = new T[ downsampleWidth * downsampleHeight ]();

        for(size_t j= 0 ; j < downsampleHeight -1; j++) {
            for(size_t i= 0 ; i < downsampleWidth - 1; i++){
                size_t index = j * downsampleWidth + i;
                downsampleData[index] = (newTileData[2 * j * width + 2 * i] + newTileData[2 * j * width + 2 *i + 1] +
                                         newTileData[2 * (j+1) * width + 2 * i] + newTileData[2 * (j+1) * width + 2 *i + 1] ) / 4;
            }
        }

        //     print("d1", downsampleData, downsampleWidth, downsampleHeight);

        for(size_t i= 0 ; i < downsampleWidth - 1; i++) {
            size_t index = (downsampleHeight - 1) * downsampleWidth + i;
            downsampleData[index] = (newTileData[(height - 1) * width + 2 * i] + newTileData[(height - 2) * width + 2 * i] + newTileData[(height -1) * width + 2 * i + 1] + newTileData[(height -2) * width + 2 * i + 1]) / 4;
        }

        //   print("d2", downsampleData, downsampleWidth, downsampleHeight);


        for(size_t i= 0 ; i < downsampleHeight - 1; i++) {
            size_t index = downsampleWidth * i + downsampleWidth - 1;
            downsampleData[index] = (newTileData[width * 2 * i + width - 1] + newTileData[width * 2 * i + width - 2] + newTileData[ width * ( 2 * i + 1) + width - 1] + newTileData[ width * (2 * i + 1) + width - 1]) / 4;
        }

        // print("d3", downsampleData, downsampleWidth, downsampleHeight);

        downsampleData[downsampleWidth * downsampleHeight - 1] = (newTileData[ width * height - 1] + newTileData[ width * (height - 1) - 1] + newTileData[width * height - 2] + newTileData[width * (height - 1) - 2]) / 4;

        // print("d4", downsampleData, downsampleWidth, downsampleHeight);

        // downsampleData[downsampleWidth * downsampleHeight - 1] = (newTileData[ 4 * width * height - 1] + newTileData[ 2 * width * (2 * height - 1) - 1]);

        //TODO REMOVE FOR DEBUG
//        cv::Mat  mat = cv::Mat(downsampleHeight, downsampleWidth, CV_8U , downsampleData);
//        cv::imwrite("createTileTaskDownsample" + std::to_string(counter) + "orig.png", mat);
//        cv::Mat tmp(downsampleHeight, downsampleWidth, CV_16U);
//        mat.convertTo(tmp, CV_16U, 1,0);
//        cv::imwrite("createTileTaskDownsample" + std::to_string(counter) + ".png", tmp);


        return downsampleData;

    }


    void print(std::string title, T* data, size_t w, size_t h){
        std::cout << title << std::endl;
        for (size_t i = 0; i < h; ++i){
            for(size_t j = 0; j < w; ++j){
                std::cout << std::setw(3) << (int)(data[i * w + j]) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    size_t numTileCol = 0;
    size_t numTileRow = 0;
    size_t numLevel = 0;
    int maxDeepZoomLevel = 0;
    bool done = false;
    const std::string _pathOut;



};

#endif //PYRAMIDBUILDING_DEEPZOOMDOWNSAMPLINGRULE_H
