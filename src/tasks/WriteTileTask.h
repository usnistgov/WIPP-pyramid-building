//
// Created by Gerardin, Antoine D. (Assoc) on 1/29/19.
//

#ifndef PYRAMIDBUILDING_WRITETASK_H
#define PYRAMIDBUILDING_WRITETASK_H


#include <htgs/api/ITask.hpp>
#include <dirent.h>
#include "../data/Tile.h"
#include <sys/stat.h>
#include <sys/types.h>

//TODO Subclassing is probably not quite necessary
template <class T>
class WriteTileTask  : public htgs::ITask< Tile<T>, Tile<T> > {


public:

    WriteTileTask(size_t numThreads, const std::string &_pathOut) : htgs::ITask<Tile<T>, Tile<T>>(numThreads), _pathOut(_pathOut) {

        auto dir = opendir(_pathOut.c_str());
        if(dir == nullptr){
            const int dir_err = mkdir(_pathOut.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (-1 == dir_err)
            {
                printf("Error creating output directory!n");
                exit(1);
            }
        }

        delete dir;
    }

    virtual void executeTask(std::shared_ptr<Tile<T>> data) override {

        std::string level = std::to_string(data->getLevel());

        auto dirPath = (this->_pathOut + "/" + std::to_string(data->getLevel()));

        //  Create directory if it does not exists
        auto dir = opendir(dirPath.c_str());
        if(dir == nullptr){
            const int dir_err = mkdir(dirPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (-1 == dir_err)
            {
                std::cout << "Error creating output directory for level" << level  << "!n";
                exit(1);
            }
        }
        delete dir;
    }

    /// \brief Close the tiff file
    void shutdown() override {
    }

    /// \brief Get the writer name
    /// \return Writer name
    std::string getName() override { return "WriteTileTask"; }


protected:

    const std::string _pathOut;



};


#endif //PYRAMIDBUILDING_WRITETASK_H
