//
// Created by Gerardin, Antoine D. (Assoc) on 12/19/18.
//

#ifndef PYRAMIDBUILDING_WRITETILERULE_H
#define PYRAMIDBUILDING_WRITETILERULE_H

#include <cstdint>
#include <FastImage/api/FastImage.h>
#include "../data/Tile.h"

namespace pb {

    template<class T>
    class WriteTileRule : public htgs::IRule<Tile < T>, Tile<T>>

{
    public:

    explicit WriteTileRule(size_t level = 0) : level(level) {};

    void applyRule(std::shared_ptr<Tile < T>>

    data,
    size_t pipelineId
    ) override {

    size_t l = data->getLevel();
    if (l >= level) {
    this->
    addResult(data);
}
}


std::string getName()

override {
return "Write Rule";
}

private:
size_t level;

};

}

#endif //PYRAMIDBUILDING_WRITETILERULE_H
