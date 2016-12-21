#!/bin/bash
#
# Authors: Jim Bob's Creators
# 05/26/16
#
#

#if [ $# -lt 2 ]; then
#    echo "Jim Bob is not ready to play"
#    echo "$0 <first process number><number of processes>"
#    exit
#fi

#first_proc=$1
#num_procs=$2
#loop_proc=$first_proc

#counter=0
#while true; do
#    while [ $counter -lt $num_procs ]; do
#        ./worm $loop_proc > /dev/null &
#        ((loop_proc++))
#        ((counter++))
#    done
#    sleep 10m
#    pkill worm
#    loop_proc=$first_proc
#    counter=0
#done

while true
do
./worm 1&
./worm 2&
./worm 3&
./worm 4&
./worm 5&
./worm 6&
./worm 7&
./worm 8&
sleep 15m
pkill worm
done
