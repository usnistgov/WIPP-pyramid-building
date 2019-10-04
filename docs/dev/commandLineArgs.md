-v /home/gerardin/Documents/pyramidBuilding/resources/dataset1/stitching_vector/img-global-positions-1.txt
-i /home/gerardin/Documents/pyramidBuilding/resources/dataset1/tiled-images
-o /home/gerardin/Documents/pyramidBuilding/outputs
-t 256 -d 8U -n testFI2 
-e reader=1;builder=1;writer=1;downsampler=1;tile=1


 -v /home/gerardin/Documents/images/dataset7-NanoFibers/manualStitching/img-global-positions-0.txt
 -i /home/gerardin/Documents/images/dataset7-NanoFibers/tiled_images
 -o /home/gerardin/Documents/pyramidBuilding/outputs
 -t 1024 -d 8U -n testFI3 
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
  