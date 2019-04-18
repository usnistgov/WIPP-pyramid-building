//
// Created by Gerardin, Antoine D. (Assoc) on 12/19/18.
//

#ifndef PYRAMIDBUILDING_UTILS_H
#define PYRAMIDBUILDING_UTILS_H

#include <iostream>
#include <algorithm>
#include <glog/logging.h>
#include <iomanip>

namespace pb {

    std::string getFileExtension(const std::string &path) {
        std::string extension = path.substr(path.find_last_of('.') + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        return extension;
    }

    template<class T>
    void printArray(std::string title, T *data, size_t w, size_t h) {
        std::ostringstream oss;

        oss << title << std::endl;
        for (size_t i = 0; i < h; ++i) {
            for (size_t j = 0; j < w; ++j) {
                oss << std::setw(3) << (int) (data[i * w + j]) << " ";
            }
            oss << std::endl;
        }
        oss << std::endl;

        VLOG(2) << oss.str();
    }
}

#endif //PYRAMIDBUILDING_UTILS_H
