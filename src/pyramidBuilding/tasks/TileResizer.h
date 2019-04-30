//
// Created by gerardin on 4/23/19.
//

#ifndef PYRAMIDBUILDING_TILERESIZER_H
#define PYRAMIDBUILDING_TILERESIZER_H

#include <pyramidBuilding/data/Tile.h>
#include <htgs/api/ITask.hpp>
#include <pyramidBuilding/fastImage/utils/TileRequestBuilder.h>

namespace pb {

    template<class T>
class TileResizer : public htgs::ITask<htgs::MemoryData<fi::View <T>>, Tile <T>> {




    public:

        TileResizer(size_t numThreads, uint32_t pyramidTileSize, const std::shared_ptr<TileRequestBuilder> &tileRequestBuilder) : htgs::ITask<htgs::MemoryData<fi::View <T>>, Tile <T>> (numThreads), pyramidTileSize(pyramidTileSize), tileRequestBuilder(tileRequestBuilder) {}


    void executeTask(htgs::m_data_t<fi::View <T>> data) override {




            auto view = data->get();
            auto row = view->getRow();
            auto col = view->getCol();
            auto fullFovWidth = tileRequestBuilder->getFullFovWidth();
            auto fullFovHeight = tileRequestBuilder->getFullFovHeight();

            VLOG(3) << "tile resizer: " << row << "," << col;

            assert(view->getPyramidLevel() == 0);

            //we copy to the view data and crop it to the tile dimension
            uint32_t r = (uint32_t)col * pyramidTileSize;
            uint32_t width = std::min(pyramidTileSize, (uint32_t)(fullFovWidth - r));
            uint32_t height = std::min(pyramidTileSize, (uint32_t)(fullFovHeight - row * pyramidTileSize));

            auto tileMemoryData = this-> template getDynamicMemory<T>("basetile", new ReleaseMemoryRule(2), width * height);
            auto tileData = tileMemoryData->get();

            for (auto x =0 ; x < height; x++){
                std::copy_n(view->getData() + x * view->getViewWidth(), width, tileData + x * width);
            }

//                    cv::Mat image2(height, width, CV_8U, data);
//                    auto path2 = "/home/gerardin/Documents/pyramidBuilding/outputs/DEBUG/tiles/" +  std::to_string(row) + "_" + std::to_string(col) + ".png";
//                    cv::imwrite(path2, image2);
//                    image2.release();

            auto tile = new Tile<T>(view->getPyramidLevel(), row, col, width, height, tileMemoryData, data);
            this->addResult(tile);

//            data->releaseMemory();
        }

        htgs::ITask<htgs::MemoryData<fi::View <T>>, Tile <T>> *copy() override {
            return new TileResizer(this->getNumThreads(), pyramidTileSize, tileRequestBuilder);
        }


        std::string getName() override {
        return "Tile Resizer";
        }




    private:
        uint32_t pyramidTileSize = 0;
        std::shared_ptr<TileRequestBuilder> tileRequestBuilder;

};


}
#endif //PYRAMIDBUILDING_TILERESIZER_H
