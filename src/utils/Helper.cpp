//
// Created by Gerardin, Antoine D. (Assoc) on 12/19/18.
//

#include "Helper.h"

std::string Helper::getExtension(const std::string &path) {
    std::string extension = path.substr(path.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension;
}
