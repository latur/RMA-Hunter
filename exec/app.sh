#!/bin/bash
key=$1
coding=$2
maxref=$3

sdf="public/data/sdf.v2.csv"
base="public/res/"
./exec/hunter $key $coding $sdf $base $maxref
cd $base

echo $(
  for i in 1 2 3; do
    cat $key.t$i | sort | awk {'print $2'} > $key.ts$i
    split -l 30 -a3 -d $key.ts$i $key.ts$i.p
    [[ -f $key.ts$i.p000 ]] || touch $key.ts$i.p000 
    cat $key.ts$i | wc -l
  done
)

find ./public/res/E* -mtime +1 -delete
