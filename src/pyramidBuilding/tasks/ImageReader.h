//
// Created by Gerardin, Antoine D. (Assoc) on 4/8/19.
//

#ifndef PYRAMIDBUILDING_IMAGEREADER_H
#define PYRAMIDBUILDING_IMAGEREADER_H

#include <htgs/api/ITask.hpp>
#include <pyramidBuilding/rules/ReleaseFOVMemoryRule.h>
#include <pyramidBuilding/utils/TiffImageLoader.h>
#include <pyramidBuilding/data/FOVRequest.h>


namespace pb {

template <class T>
class ImageReader : public htgs::ITask<FOVRequest, htgs::MemoryData<T*>> {

public:

    ImageReader(size_t numThreads, TiffImageLoader<T> imageLoader) : htgs::ITask<FOVRequest, htgs::MemoryData<T*>>(numThreads), imageLoader(imageLoader) {
    }

    void executeTask(std::shared_ptr<FOVRequest> data) override {

        std::string filename = data->getFilename();
        htgs::m_data_t<T> fov = this->getMemory("fov", new ReleaseFOVMemoryRule(1));
        imageLoader.loadFullImage(fov->get(),filename);
        this->addResult(fov);
    }

    htgs::ITask <FOVRequest, htgs::MemoryData<T*>> *copy() override {
        return new ImageReader(this->getNumThreads(), this->getImageLoader);
    }

    const TiffImageLoader<T> &getImageLoader() const {
        return imageLoader;
    }


private:
    TiffImageLoader<T> imageLoader;

};



}


#endif //PYRAMIDBUILDING_IMAGEREADER_H
