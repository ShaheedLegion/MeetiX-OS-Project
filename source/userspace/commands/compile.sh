#!/bin/bash

# compile sources
for entry in ./*;
do
    if [ -d $entry ];
	then
        cd $entry
        bash build.sh $1
        cd ..
    fi
done
