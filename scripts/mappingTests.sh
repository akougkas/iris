#!/usr/bin/env bash
for opType in 0 1
do
    for opSize in 32 64 128 256 512 1024 2048
    do
	    for mode in 0 1 2 3
	    do
            for async in 0 1
            do
                for i in 1 2 3
                do
                echo "Starting test:${opType}-${opSize}-${mode}-${async}-rep:${i}"
                ../bin/iris $opType $opSize $(($opSize * 4)) $mode $async
                sleep 3
                echo "Finished!"
                done
            done
        done
    done
done