# TODOS

## Checks

Check google style guide.

## size_t for all indices.
Is that too much? Realistically what is the max 1Dindex value we can reach?
20000 * 20000 = 400M
log2(400M) = 29 bits necessary so uint32 should be enough?  
FastImage uses uint32, openCV uses uint32

## Features

* Other forms of blending?
* Overlap
* Support for non tiled images? (from big dataset)
* Support for large FOVs with Fast Image




