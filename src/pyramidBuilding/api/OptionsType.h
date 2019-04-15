//
// Created by Gerardin, Antoine D. (Assoc) on 2/25/19.
//

#ifndef PYRAMIDBUILDING_DATATYPE_H
#define PYRAMIDBUILDING_DATATYPE_H

namespace pb {

    enum class DownsamplingType {
        NEIGHBORS_AVERAGE,
    };

    enum class PyramidFormat {
        DEEPZOOM,
    };

    enum class BlendingMethod {
        OVERLAY,
        MAX
    };

    enum class ImageDepth {
        _8U,
        _16U,
    };

}

#endif //PYRAMIDBUILDING_DATATYPE_H
