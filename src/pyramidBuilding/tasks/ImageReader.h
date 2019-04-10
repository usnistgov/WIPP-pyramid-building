//
// Created by Gerardin, Antoine D. (Assoc) on 4/8/19.
//

#ifndef PYRAMIDBUILDING_IMAGEREADER_H
#define PYRAMIDBUILDING_IMAGEREADER_H

#include <htgs/api/ITask.hpp>
#include <pyramidBuilding/rules/ReleaseFOVMemoryRule.h>
#include <pyramidBuilding/utils/TiffImageLoader.h>
#include <pyramidBuilding/utils/StitchingVectorParser.h>
#include <pyramidBuilding/data/FOVWithData.h>


namespace pb {

template <class T>
class ImageReader : public htgs::ITask<FOV, FOVWithData<T> > {

public:

    ImageReader(size_t numThreads, TiffImageLoader <T> *imageLoader) : htgs::ITask<FOV, FOVWithData<T> >(numThreads), imageLoader(imageLoader) {}


    void executeTask(std::shared_ptr<FOV> data) override {
        std::string filename = data->getFilename();
        htgs::m_data_t<T> image = this->template getMemory<T>("fov", new ReleaseFOVMemoryRule(1));
        imageLoader->loadFullImage(image->get(), filename);
        this->addResult(new FOVWithData<T>(data,image));
    }

    htgs::ITask<FOV, FOVWithData<T>> *copy() override {
        return nullptr;
    }

    const TiffImageLoader<T>* getImageLoader() const {
        return imageLoader;
    }




private:
    TiffImageLoader<T>* imageLoader;

};



}


#endif //PYRAMIDBUILDING_IMAGEREADER_H
