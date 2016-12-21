#!/bin/bash
#
# Authors: Jim Bob's Creators
# 05/26/16
#
#

if [ $# -lt 2 ]; then
    echo "Jim Bob is not ready to play"
    echo "$0 <first process number><number of processes>"
    exit
fi

first_proc=$1
num_procs=$2
loop_proc=$first_proc

counter=0
while true; do
    while [ $counter -lt $num_procs ]; do
        ./worm $loop_proc > /tmp/worm.out &
        ((loop_proc++))
        ((counter++))
        sleep 15s
    done
    sleep 8m
    pkill worm
    sleep 15s
    loop_proc=$first_proc
    counter=0
done
