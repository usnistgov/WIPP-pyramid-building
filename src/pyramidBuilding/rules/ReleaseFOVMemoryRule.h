//
// Created by gerardin on 3/28/19.
//

#ifndef PYRAMIDBUILDING_RELEASETILEMEMORYRULE_H
#define PYRAMIDBUILDING_RELEASETILEMEMORYRULE_H

#include <htgs/api/IMemoryReleaseRule.hpp>
#include <glob.h>

class ReleaseFOVMemoryRule : public htgs::IMemoryReleaseRule {

public:

    ReleaseFOVMemoryRule(size_t releaseCount) : releaseCount(releaseCount) {}

    void memoryUsed() override {
        releaseCount--;
    }

    bool canReleaseMemory() override {
        return releaseCount == 0;
    }


private:
    size_t releaseCount;
};


#endif //PYRAMIDBUILDING_RELEASETILEMEMORYRULE_H
