# TODOS




## Checks

Check google style guide and comply.


### size_t for all indices.
Is that too much? Realistically what is the max 1Dindex value we can reach?
20000 * 20000 = 400M
log2(400M) = 29 bits necessary so uint32 should be enough?  
FastImage uses uint32, openCV uses uint32

##### Verify std::strtoul for parsing stichting vector.
##### Convert result of whole division back to indice type.

## Cleanup

Memory leaks

##### The execution may fail if the output directory does not exist.
##### Check inputs : if bad path, it does not detect it a return

Refactor mains into google tests.

## Refactor 

##### Make downsampling a strategy
##### Utils with print array methods

## Features

##### Stripe example : 1pixel stripes are averaged and disappear!
Bilinear downsampling could fix this for P. it is probably unecessary.

##### Check if need overlap? 
First tests seem to show we don't need to generate 1pixel overlap.
More tests on more browsers are necessary.

##### Support for non tiled images? (from big dataset)
It could be necessary but apparently P. set tiled tiff as a requirement to biologists
so we should be clear.
Maybe implement a tiff reader and benchmark.

##### Support for large images
P. says we need support for large images so let's keep FI implementation.
We can decide dynamically based on output which BaseTileGenerator algorithm we need. 

=======

## Next Steps


Presentation

Accurate benchmarks of memory consumption. (heap track)
Benchmark disk access.
Benchmark time spend in reading.
Make sure we have the minimum amount of objects at some point in time.


Benchmarking
Try different traversals. Probably very minor but let's try.

============

Claims

with HTGS impact of running a lots of threads is mitigated (if inputs are throttled) so it becomes a none issue.
-> easier to dimension the system. (however ex of problem : pyramidalTiledTiffWrite must be single threaded).



