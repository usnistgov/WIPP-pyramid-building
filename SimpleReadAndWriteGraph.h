//
// Created by gerardin on 3/8/19.
//

#ifndef PYRAMIDBUILDING_SIMPLEREADANDWRITEGRAPH_H
#define PYRAMIDBUILDING_SIMPLEREADANDWRITEGRAPH_H

#include <FastImage/api/FastImage.h>
#include "testBasePerf.h"


class InputFile : public IData {



private:
    filesystem::path inputFile;

public:
    InputFile(filesystem::path inputFile) : inputFile(inputFile) {}

    const filesystem::path getInputFile() {
        return inputFile;
    }
};


template<class T>
class ImageData : public IData {

private:
    T* data;
    filesystem::path inputFile;

public:

    ImageData(T *data, filesystem::path inputFile) : data(data), inputFile(inputFile) {}


    ~ImageData() override {
    }

    T *getData() {
        return data;
    }

    const filesystem::path getInputFile() {
        return inputFile;
    }
};



template<class T>
class TestReadTask : public ITask<InputFile, ImageData<T>> {

public:
    TestReadTask(size_t numThreads) : ITask<InputFile, ImageData<T>>(numThreads) {}

    void executeTask(std::shared_ptr<InputFile> data) override {
        auto fovReader = new FOVReader<T>();
        filesystem::path inputFile = data->getInputFile();
        VLOG(2) << "reading: " << data->getInputFile().string() << std::endl;
        T* imageData = fovReader->readFOV(inputFile.c_str());
        delete fovReader;
        VLOG(2) << data->getInputFile().string() << std::endl;

        if(imageData!= nullptr) {
            this->addResult(new ImageData<T>(imageData, inputFile));
        }
        else{
            VLOG(2) << "not a valid image : " << data->getInputFile().string();
        }
    }

    ITask<InputFile, ImageData<T>> *copy() override {
        return new TestReadTask(this->getNumThreads());
    }
};

template<class T>
class TestWriteTask : public ITask<ImageData<T>, ImageData<T>> {


private:
    const filesystem::path &outputPath;


public:
    TestWriteTask(const filesystem::path &outputPath, size_t numThreads) : ITask<ImageData<T>, ImageData<T>>(numThreads), outputPath(outputPath) {}

    void executeTask(std::shared_ptr<ImageData<T>> data) override {
        auto fovReader = new FOVReader<T>();
        auto outputPathFile = outputPath / ( data->getInputFile().filename().string() + ".png");
        VLOG(2) << "writing " + data->getInputFile().string() << " to " << outputPathFile.string()  << std::endl;
        fovReader->writeFOV(data->getData(), outputPathFile);
        delete fovReader;
        this->addResult(data);
    }

    ITask<ImageData<T>, ImageData<T>> *copy() override {
        return new TestWriteTask(this->outputPath, this->getNumThreads());
    }
};




#endif //PYRAMIDBUILDING_SIMPLEREADANDWRITEGRAPH_H
