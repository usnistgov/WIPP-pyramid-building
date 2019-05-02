#!/bin/bash
# Benchmark an pyramid building run
#
# Example Usage :
# run_benchmarks.sh dataset1 5 -v vector -i /images/ -o /outputs/ -t 256 -d 8U
# $1 : name of the dataset1 for creating file entry
# $2 : number of runs to perform

# Needs install of getopt first :
# On OSX : brew install -y gnu-getopt && echo 'export PATH="/usr/local/opt/gnu-getopt/bin:$PATH"' >> ~/.bash_profile


# saner programming env: these switches turn some bugs into errors
set -o errexit -o pipefail -o noclobber -o nounset

! getopt --test > /dev/null
if [[ ${PIPESTATUS[0]} -ne 4 ]]; then
    echo 'I’m sorry, `getopt --test` failed in this environment.'
    exit 1
fi

OPTIONS=i:v:o:t:d:n:b:k:
LONGOPTS=images:,vector:,output:,tilesize:,depth:,name:,blending:,benchmark:,cache:,threads:,

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

images="" vector="" output="" tilesize=256 depth="8U" name="output" blending="max" blending="overlay" benchmark="exectime"
cache=0 threads=10
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
        -k|--benchmark)
            benchmark="$2"
            shift 2
            ;;
        --cache)
            cache="$2"
            shift 2
            ;;
        --threads)
            threads="$2"
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

mkdir -p results
mkdir -p results/massif

date=$(date +"%m_%d_%Y_%T")

CLEAR_CACHE=0;
export GLOG_logtostderr=1;
export GLOG_v=4


EXEC_PATH=$3
EXEC_NAME=pyramidio-1.1.1-SNAPSHOT-jar-with-dependencies.jar
DATASET_NAME=${1}
RUNS=${2-1}
OUTPUT_DIR=$4
MASSIF_DIR=massif

OUTPUT_FILE=$OUTPUT_DIR/${DATASET_NAME}_${date}.txt

echo "exec path: " $3
echo "output dir: " $4
echo "output file: " $OUTPUT_FILE

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

        echo "images: $images" >> $OUTPUT_FILE
        echo "stitching vector: $vector" >> $OUTPUT_FILE
        echo "output: $output" >> $OUTPUT_FILE
        echo "tilesize: $tilesize, imageCache: $cache, threads: $threads " >> $OUTPUT_FILE

    if [[ "$benchmark" == "exectime" ]]; then
            echo "benchmarking execution time..."
            cd $EXEC_PATH;
            { java -jar pyramidio-1.1.1-SNAPSHOT-jar-with-dependencies.jar $images $vector $output $tilesize $cache $threads ; }
#             { time  java -jar pyramidio-1.1.1-SNAPSHOT-jar-with-dependencies.jar $images $vector $output $tilesize $cache $threads ; } 2>> $OUTPUT_DIR/${benchmark}_${DATASET_NAME}_${date}.txt
#             sudo -u $SUDO_USER GLOG_logtostderr=1 GLOG_v=3 ../cmake-build-release/main -i $images -v $vector -o $output -t $tilesize -d $depth -n $name -b $blending -e $expertmode;
    else
            echo "benchmarking memory consumption..."

    fi

# "valgrind --tool=massif --stacks=yes --massif-out-file="
#            { valgrind --tool=massif --stacks=yes --massif-out-file=$OUTPUT_DIR/$MASSIF_DIR/${DATASET_NAME}_${date}.txt ../cmake-build-debug/main -i $images -v $vector -o $output -t $tilesize -d $depth -n $name -b $blending; } 2>> $OUTPUT_DIR/${DATASET_NAME}_${date}.txt

echo "benchmarks completed."

done







#java -jar pyramidio-1.1.1-SNAPSHOT-jar-with-dependencies.jar /home/gerardin/Documents/images/dataset7/tiled-images /home/gerardin/Documents/images/dataset7/img-global-positions-0.txt /home/gerardin/Documents/pyramidio-java/outputs
