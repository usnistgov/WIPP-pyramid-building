//
// Created by Gerardin, Antoine D. (Assoc) on 12/27/18.
//

#ifndef PYRAMIDBUILDING_MATRIXMEMORYRULE_H
#define PYRAMIDBUILDING_MATRIXMEMORYRULE_H

#include <htgs/api/IMemoryReleaseRule.hpp>
#include <cstdio>

class MatrixMemoryRule : public htgs::IMemoryReleaseRule {
public:

    MatrixMemoryRule(size_t releaseCount) : releaseCount(releaseCount) {
    }

    void memoryUsed() {
        releaseCount--;
    }

    bool canReleaseMemory() {
        return releaseCount == 0;
    }

private:
    size_t releaseCount;
};

#endif //PYRAMIDBUILDING_MATRIXMEMORYRULE_H
