## BIGBOX

#### Dataset1 - small test images from original pyramid building

-v /home/gerardin/Documents/images/dataset1/stitching_vector/img-global-positions-1.txt
-i /home/gerardin/Documents/images/dataset1/tiled-images/
-o /home/gerardin/Documents/projects/pyramid-building/outputs
-t 256 -d 8U -n dataset1_pyramid
-e reader=1;builder=1;writer=1;downsampler=1;tile=1

#### Dataset2 - large 1GB stitched image

-v /home/gerardin/Documents/images/dataset2/img-global-positions-1.txt
    -i /home/gerardin/Documents/images/dataset2/images/tiled
    -o /home/gerardin/Documents/projects/pyramid-building/outputs
    -t 1024 -d 16U -n dataset2_pyramid

#### NanoFibers
 -v /home/gerardin/Documents/images/dataset7-NanoFibers/manualStitching/img-global-positions-0.txt
 -i /home/gerardin/Documents/images/dataset7-NanoFibers/tiled_images
 -o /home/gerardin/Documents/projects/pyramid-building/outputs
 -t 1024 -d 8U -n poster  -e reader=2;builder=4;writer=30;downsampler=4;tile=100;


==================================================================
## OLD



-v /home/gerardin/Documents/pyramidBuilding/resources/dataset1/stitching_vector/img-global-positions-1.txt
-i /home/gerardin/Documents/pyramidBuilding/resources/dataset1/tiled-images
-o /home/gerardin/Documents/pyramidBuilding/outputs
-t 256 -d 8U -n testFI2 
-e reader=1;builder=1;writer=1;downsampler=1;tile=1


 -v /home/gerardin/Documents/images/dataset7-NanoFibers/manualStitching/img-global-positions-0.txt
 -i /home/gerardin/Documents/images/dataset7-NanoFibers/tiled_images
 -o /home/gerardin/Documents/pyramidBuilding/outputs
 -t 1024 -d 8U -n poster
-e reader=2;builder=2;writer=30;downsampler=4;tile=100;


-v /home/gerardin/Documents/images/dataset2/img-global-positions-1.txt
    -i /home/gerardin/Documents/images/dataset2/images
-o /home/gerardin/Documents/pyramidBuilding/outputs
-t 1024 -d 8U -n testFI4 

-v /home/gerardin/Documents/pyramidBuilding/resources/dataset1/stitching_vector/img-global-positions-1.txt 
-i /home/gerardin/Documents/pyramidBuilding/resources/dataset1/tiled-images 
-o /home/gerardin/Documents/pyramidBuilding/outputs 
-t 16 -d 8U -n FIFINAL 
-e reader=2;builder=2;writer=30;downsampler=1;tile=4;


 -v /home/gerardin/Documents/images/celegansMAPS3_EMTileSet1/stitching/img-global-positions-0.txt 
  -i /home/gerardin/Documents/images/celegansMAPS3_EMTileSet1
  -o /home/gerardin/Documents/pyramidBuilding/outputs  -t 1024 -d 8U -n celegans -e  "reader=4;builder=3;writer=20;downsampler=4;tile=100;" 
  