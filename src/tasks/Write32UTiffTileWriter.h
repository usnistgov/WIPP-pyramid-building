//
// Created by Gerardin, Antoine D. (Assoc) on 1/29/19.
//

#ifndef PYRAMIDBUILDING_WRITE32UTIFFTILEWRITER_H
#define PYRAMIDBUILDING_WRITE32UTIFFTILEWRITER_H

#include "../utils/SingleTiledTiffWriter.h"
#include "WriteTileTask.h"

template <class T>
class Write32UTiffTileWriter : public WriteTileTask<T> {

public:

    Write32UTiffTileWriter(size_t numThreads, const std::string &_pathOut) :  WriteTileTask<T>(numThreads, _pathOut) {
    }

    //TODO check why compiler rejects that
    //  Write8UPngTileTask(const std::string &_pathOut, SingleTiledTiffWriter writer) : Write8UPngTileTask(1, &_pathOut, writer) {}

    void executeTask(std::shared_ptr<Tile<T>> data) override {

        WriteTileTask<T>::executeTask(data);

        std::string level = std::to_string(data->getLevel());
        auto outputFilename = "img_r" + std::to_string(data->getRow()) + "_c" + std::to_string(data->getCol()) + ".tif";
        auto fullImagePath = this->_pathOut + "/" + level + "/"  + outputFilename;

        //TODO CHECK how this can vary with the template
        auto writer = new SingleTiledTiffWriter(fullImagePath, data->get_width(), data->get_height(), 32);
        writer.write(data->getData());

        delete writer;

        addResult(data);

    }

    /// \brief Close the tiff file
    void shutdown() override {
    }

    /// \brief Get the writer name
    /// \return Writer name
    std::string getName() override { return "TiffTileWriteTask"; }


    htgs::ITask<Tile<T>, Tile<T>> *copy() override {
        return new Write32UTiffTileWriter(this->getNumThreads(), this->_pathOut);
    }

};

#endif //PYRAMIDBUILDING_WRITE32UTIFFTILEWRITER_H
