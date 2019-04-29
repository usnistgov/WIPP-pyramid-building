-v /home/gerardin/Documents/pyramidBuilding/resources/dataset1/stitching_vector/img-global-positions-1.txt
-i /home/gerardin/Documents/pyramidBuilding/resources/dataset1/tiled-images
-o /home/gerardin/Documents/pyramidBuilding/outputs
-t 256 -d 8U -n testFI2 
-e reader=1;builder=1;writer=1;downsampler=1;tile=1


 -v /home/gerardin/Documents/images/dataset7/manualStitching/img-global-positions-0.txt
 -i /home/gerardin/Documents/images/dataset7/tiled-images
 -o /home/gerardin/Documents/pyramidBuilding/outputs
 -t 1024 -d 8U -n testFI3 
 -e reader=2;builder=1;writer=40;downsampler=6;fovs=5


-v /home/gerardin/Documents/images/dataset2/img-global-positions-1.txt
-i /home/gerardin/Documents/images/dataset2/images
-o /home/gerardin/Documents/pyramidBuilding/outputs
-t 1024 -d 8U -n testFI4 

