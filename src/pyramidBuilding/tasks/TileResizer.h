//
// Created by gerardin on 4/23/19.
//

#ifndef PYRAMIDBUILDING_TILERESIZER_H
#define PYRAMIDBUILDING_TILERESIZER_H

#include <pyramidBuilding/data/Tile.h>
#include <htgs/api/ITask.hpp>
#include <utility>
#include <pyramidBuilding/pyramid/PyramidBuilder.h>

namespace pb {

    template<class T>
class TileResizer : public htgs::ITask<htgs::MemoryData<fi::View <T>>, Tile <T>> {

    public:

        TileResizer(size_t numThreads, uint32_t pyramidTileSize, std::shared_ptr<PyramidBuilder> tileRequestBuilder, size_t referenceCount) : htgs::ITask<htgs::MemoryData<fi::View <T>>, Tile <T>> (numThreads), pyramidTileSize(pyramidTileSize), tileRequestBuilder(std::move(tileRequestBuilder)), referenceCount(referenceCount) {}


    void executeTask(htgs::m_data_t<fi::View <T>> data) override {
            auto view = data->get();
            auto row = view->getRow();
            auto col = view->getCol();
            auto fullFovWidth = tileRequestBuilder->getFullFovWidth();
            auto fullFovHeight = tileRequestBuilder->getFullFovHeight();

            VLOG(2) << "tile resizer: " << row << "," << col;

            assert(view->getPyramidLevel() == 0);

            //we copy to the view data and crop it to the tile dimension
            uint32_t width = std::min(pyramidTileSize, (uint32_t)(fullFovWidth - col * pyramidTileSize));
            uint32_t height = std::min(pyramidTileSize, (uint32_t)(fullFovHeight - row * pyramidTileSize));

            //release count depends on how many write rules we have
            auto tileMemoryData = this-> template getDynamicMemory<T>("basetile", new ReleaseMemoryRule(referenceCount), width * height);
            auto tileData = tileMemoryData->get();

            for (uint32_t tileRow = 0 ;tileRow < height; tileRow++){
                std::copy_n(view->getData() + tileRow * view->getViewWidth(), width, tileData + tileRow * width);
            }

//                    cv::Mat image2(height, width, CV_8U, data);
//                    auto path2 = "/home/gerardin/Documents/pyramidBuilding/outputs/DEBUG/tiles/" +  std::to_string(row) + "_" + std::to_string(col) + ".png";
//                    cv::imwrite(path2, image2);
//                    image2.release();

            auto tile = new Tile<T>(view->getPyramidLevel(), row, col, width, height, tileMemoryData);
            this->addResult(tile);

            data->releaseMemory();
        }

        htgs::ITask<htgs::MemoryData<fi::View <T>>, Tile <T>> *copy() override {
            return new TileResizer(this->getNumThreads(), pyramidTileSize, tileRequestBuilder, referenceCount);
        }


        std::string getName() override {
        return "Tile Resizer";
        }




    private:
        uint32_t pyramidTileSize = 0;
        std::shared_ptr<PyramidBuilder> tileRequestBuilder;
        size_t referenceCount;

};


}
#endif //PYRAMIDBUILDING_TILERESIZER_H
