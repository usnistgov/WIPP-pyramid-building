#!/bin/bash
# Shell script to run the pyramid building algorithm
#
# NOTE : Requires getopt
# On OSX : brew install -y gnu-getopt && echo 'export PATH="/usr/local/opt/gnu-getopt/bin:$PATH"' >> ~/.bash_profile
#
# Example Usage:
# ./pyramid_building -i $images -v $vector -o $output -t $tilesize -d $depth -n $name -b $blending

# saner programming env: these switches turn some bugs into errors
set -o errexit -o pipefail -o noclobber -o nounset

! getopt --test > /dev/null
if [[ ${PIPESTATUS[0]} -ne 4 ]]; then
    echo 'I’m sorry, `getopt --test` failed in this environment.'
    exit 1
fi

OPTIONS=i:v:o:t:d:n:b
LONGOPTS=images:,vector:,output:,tilesize:,depth:,name:,blending

# -use ! and PIPESTATUS to get exit code with errexit set
# -temporarily store output to be able to check for errors
# -activate quoting/enhanced mode (e.g. by writing out “--options”)
# -pass arguments only via   -- "$@"   to separate them correctly
! PARSED=$(getopt --options=$OPTIONS --longoptions=$LONGOPTS --name "$0" -- "$@")
if [[ ${PIPESTATUS[0]} -ne 0 ]]; then
    # e.g. return value is 1
    #  then getopt has complained about wrong arguments to stdout
    exit 2
fi
# read getopt’s output this way to handle the quoting right:
eval set -- "$PARSED"

echo $PARSED

images="" vector="" output="" tilesize=256 depth="8U" name="output" blending="max"
# now enjoy the options in order and nicely split until we see --
while true; do
    case "$1" in
        -i|--images)
            images="$2"
            shift 2
            ;;
        -v|--vector)
            vector="$2"
            shift 2
            ;;
        -o|--output)
            output="$2"
            shift 2
            ;;
        -t|--tilesize)
            tilesize="$2"
            shift 2
            ;;
        -d|--depth)
            depth="$2"
            shift 2
            ;;
        -n|--name)
            name="$2"
            shift 2
            ;;
        -b|--blending)
            blending="$2"
            shift 2
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "Programming error"
            exit 3
            ;;
    esac
done

# handle non-option arguments
if [[ $# -ne 0 ]]; then
    echo "$0: Problem parsing command line. Unparsed : $@"
    exit 4
fi

echo "images: $images vector: $vector output: $output"

../cmake-build-release/main -i $images -v $vector -o $output -t $tilesize -d $depth -n $name -b $blending

