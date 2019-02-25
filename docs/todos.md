# TODOS

## Checks

### Requirements for C++ version?
c++17 has file system managemnent in std. Otherwise, needs to use boosts?

### How to add debugging statement only in debu?
Pragmas?

### size_t for all indices.
Is that too much? Realistically what is the max 1Dindex value we can reach?
20000 * 20000 = 400M
log2(400M) = 29 bits necessary so uint32 should be enough?  
FastImage uses uint32, openCV uses uint32

##### Verify std::strtoul for parsing stichting vector.
##### Convert result of whole division back to indice type.

## Cleanup

##### The execution may fail if the output directory does not exist.
##### Check inputs : if bad path, it does not detect it a return


## Refactor 

##### Make downsampling a strategy
##### Utils with print array methods

## Features

##### Blending of original tiles
Original code used last-tile-win strategy when partial FOVs overlap for base tile creation.
We kept it this way.

##### Minimizing the number of fast image instance initialized.
For each tile at level 0, we instantiate n FastImage, one for each overlapping FOV and desrtoyed them afterwards.
We could limit that by counting the number of tiles where a FOV appears and tie the lifecycle of a fast image instance to this usage count.
When this usage count drop to zero, we can destroy the fast image instance.

##### Dealing with conversion
What image format should we support?
We need to build some flexibility in the code to perform conversion.
How do we deal with variability of inputs since we use templates? In production we need to run pre-compiled code.

##### Stripe example : 1pixel stripes are averaged and disappear!

##### Check if need overlap? 

##### Support for non tiled images? (From big dataset)