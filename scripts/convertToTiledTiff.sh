#!/usr/bin/env bash

usage_msg="Usage: $0 INPUT_DIR OUTPUT_DIR (TILE_WIDTH) (TILE_HEIGHT) (COMPRESSION)"

if [ $# -eq 0 ]
	  then
		      echo $usage_msg;
		          exit 1;
		  fi

		  : ${1?$usage_msg}
		  : ${2?$usage_msg}

		  if [ -z "$3" ]
			    then
				        echo "No tile width supplied - default to 256"
				fi

				if [ -z "$4" ]
					  then
						      echo "No tile height supplied - default to 256"
					      fi

					      WIDTH=${3-256}
					      HEIGHT=${4-256}
					      COMPRESSION=${5-"none"}
					      mkdir -p $2

					      for image in $(ls $1); do 
						        vips tiffsave $1/$image $2/$image --tile --tile-width=$WIDTH --tile-height=$HEIGHT --compression=$COMPRESSION
						done
