#!/bin/bash
# 
# Authors: Jim Bob's Creators
# 05/26/16
#
# Some content taken from a previous script written by Linge Bai
#

# Initial check
if [ $# -lt 1 ]; then
    echo "$0 <num instances per node>"
    exit
fi

# Keep a core available for I/O and OS related tasks
num_procs_per_node=$1
if [ $num_procs_per_node -gt 7 ]; then
    echo "Keep number of worms per node < 8 please"
    exit
fi

project_dir='/export/kuiper/delegans/delegans'
ws_file='workstations.txt'
host="$(hostname)"
node_counter=1
process_counter=0
CCIIT="Jake"
TEST_NODE='kuiper-07'
while read line; do
    # Get the node name from the workstations.txt file
    node=$line
    
    if [ "$host" != "$node" ]; then
        # Determine if the node is reachable
        reachable="$(nc -q 5 $node 22 < /dev/null | grep -c SSH)"
        if [ $reachable -eq 1 ]; then
            # Okay, so havePhun has been modified, it takes a starting process number and number
            # of processes per node. It will create num_procs_per_node instances of the worm, and
            # the proc number passed to worm will start at the starting process number and be
            # incremented for each process. This way we won't overwrite files until we implement
            # something that writes to a DB
               
            echo "$node is reachable, starting Jim now."
            
            ssh -n $node "cd $project_dir; nohup ./havePhun.sh $process_counter $num_procs_per_node" &

            echo "Worms should now be running on $node"

            process_counter=$((process_counter + num_procs_per_node))
        
        else
            echo "$node is not reachable, email $CCIIT."
        fi
    fi

done < $ws_file

# Jim has been started on all the other nodes, start Jim on this node
echo "Currently not running worm on node that called the script"
echo "Ideally you would always launch from kuiper-01 so kuiper-01 doesn't get overloaded"
#./havePhun.sh $process_counter $num_procs_per_node > output_$host.txt &

exit
