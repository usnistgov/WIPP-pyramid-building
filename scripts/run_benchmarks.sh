#!/bin/bash
# Benchmark an pyramid building run
#
# Example Usage :
# run_benchmarks.sh dataset1 5 -v vector -i /images/ -o /outputs/ -t 256 -d 8U
# $1 : name of the dataset1 for creating file entry
# $2 : number of runs to perform

# Needs install of getopt first :
# On OSX : brew install -y gnu-getopt && echo 'export PATH="/usr/local/opt/gnu-getopt/bin:$PATH"' >> ~/.bash_profile

echo "running as $USER on behalf of $SUDO_USER"

# saner programming env: these switches turn some bugs into errors
set -o errexit -o pipefail -o noclobber -o nounset

! getopt --test > /dev/null
if [[ ${PIPESTATUS[0]} -ne 4 ]]; then
    echo 'I’m sorry, `getopt --test` failed in this environment.'
    exit 1
fi

OPTIONS=i:v:o:t:d:n:b:e:k:
LONGOPTS=images:,vector:,output:,tilesize:,depth:,name:,blending:,expertmode:,benchmark:

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

images="" vector="" output="" tilesize=256 depth="8U" name="output" blending="overlay" benchmark="exectime"
expertmode=""
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
        -e|--expertmode)
            expertmode="$2"
            shift 2
            ;;
        -k|--benchmark)
            benchmark="$2"
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
if [[ $# -lt 1 ]]; then
    echo "$0: Problem parsing command line. Unparsed : $@"
    exit 4
fi

export GLOG_logtostderr=1
export GLOG_v=3

echo "log level: $GLOG_v"

DATASET_NAME=${1}
RUNS=${2-1}
OUTPUT_DIR=results
MASSIF_DIR=massif


mkdir -p results
mkdir -p results/massif

date=$(date +"%m_%d_%Y_%T")

CLEAR_CACHE=0;




for ((i = 1; i <= $RUNS; i++))
    do
        if [[ "$OSTYPE" == "linux-gnu" ]]; then
            echo "linux detected - can clear cache";
            CLEAR_CACHE=1;
        else
            echo "not a linux OS - cannot clear cache";
        fi

        if [[ "$CLEAR_CACHE" == "1" ]]; then
                sudo bash -c "sync; echo 1 > /proc/sys/vm/drop_caches"
        fi

        echo "checking if sources need to be recompiled..."
        echo "[TODO - NOT IMPLEMENTED]"

    if [[ "$benchmark" == "exectime" ]]; then
            echo "benchmarking execution time..."
#            ../cmake-build-release/main -i $images -v $vector -o $output -t $tilesize -d $depth -n $name -b $blending;
            { sudo -u $SUDO_USER time ../cmake-build-release/main -i $images -v $vector -o $output -t $tilesize -d $depth -n $name -b $blending -e $expertmode; } 2>> $OUTPUT_DIR/${benchmark}_${DATASET_NAME}_${date}.txt
    else
            echo "benchmarking memory consumption..."
            { sudo -u $SUDO_USER heaptrack ../cmake-build-release/main -i $images -v $vector -o $output -t $tilesize -d $depth -n $name -b $blending $expertmode; } 2>> $OUTPUT_DIR/${benchmark}_${DATASET_NAME}_${date}.txt
    fi

# "valgrind --tool=massif --stacks=yes --massif-out-file="
#            { valgrind --tool=massif --stacks=yes --massif-out-file=$OUTPUT_DIR/$MASSIF_DIR/${DATASET_NAME}_${date}.txt ../cmake-build-debug/main -i $images -v $vector -o $output -t $tilesize -d $depth -n $name -b $blending; } 2>> $OUTPUT_DIR/${DATASET_NAME}_${date}.txt

echo "benchmarks completed."

done
