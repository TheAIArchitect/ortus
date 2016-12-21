#!/bin/bash
# 
# Authors: Jim Bob's Creators
# 05/27/16
#

project_dir='/export/kuiper/delegans/delegans'
ws_file='workstations.txt'
host="$(hostname)"
while read line; do
    
    # Get the node name from the workstations.txt file
    node=$line
    if [ "$host" == "$node" ]; then
        # Can't call the kill script on current node or kill script also dies, dunno why...
        continue
    else
        reachable="$(nc -q 5 $node 22 < /dev/null | grep -c SSH)"
        if [ $reachable -eq 1 ]; then
            echo "Killing worms on $node"
            ssh -n $node "cd $project_dir; nohup ./killWorm_local.sh" 
        else
            echo "$node does not seem to be reachable at the moment."
            echo "The worms will probably not be killed, manually SSH into $node and check"
            ssh -n $node "cd $project_dir; nohup ./killWorm_local.sh"
        fi
    fi

done < $ws_file

./killWorm_local.sh

echo "Kill script done, if any nodes reported as unreachable make sure the worms have been killed"
