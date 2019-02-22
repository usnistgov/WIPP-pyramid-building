# TODOS

## CHECK

### size_t for all indices.
Is that too much? Realistically what is the max 1Dindex value we can reach?
20000 * 20000 = 400M
log2(400M) = 29 bits necessary so uint32 should be enough?

### problem with downsampling at the image edges
We blend the tile last row/column pixel value with the background value so we halves the values.
However this is tricky. Tiles are created from partially overlapping FOVs.
The only way to completely remove border effects would be to keep track of a mask of the actual overlaps and use that masks
to determine if a tile's pixel belongs to the original image and not the background.
The mask should also be compute at each new level.

### Minimizing the number of fast image instance initialized.
For each tile at level 0, we instantiate n FastImage, one for each overlapping FOV and desrtoyed them afterwards.
We could limit that by counting the number of tiles where a FOV appears and tie the lifecycle of a fast image instance to this usage count.
When this usage count drop to zero, we can destroy the fast image instance.

### Dealing with conversion
What image format should we support?
We need to build some flexibility in the code to perform conversion.
How do we deal with variability of inputs since we use templates? In production we need to run pre-compiled code.

### Convert result of whole division back to indice type.

### The execution may fail if the output directory does not exist.

### Stripe example : 1pixel stripes are averaged and disappear!

### Check if need overlap? 

### Support for non tiled images? (From big dataset)

### remove square tile assumption and generate tile of anysize at level 0?

### Check inputs : if bad path, it does not detect it a return