//
// Created by gerardin on 3/20/19.
//

#ifndef PYRAMIDBUILDING_BASETILEGENERATOR_H
#define PYRAMIDBUILDING_BASETILEGENERATOR_H

#include <pyramidBuilding/data/Tile.h>

namespace pb {

    template<class T>
    class BaseTileGenerator {

    public:

        virtual ~BaseTileGenerator(){};

        virtual Tile <T> *generateTile(std::pair <size_t, size_t> index) = 0;
    };

}
#endif //PYRAMIDBUILDING_BASETILEGENERATOR_H
