#!/bin/bash
key=$1
coding=$2
maxafs=$3
./exec/hunter /tmp/$key.xvcf /tmp/$key.xbed "$coding" "$maxafs" public/res/$key public/data/sdf.csv public/data/sdf_plus.csv

cd public/res/
echo $(
  for i in 1 2 3 4; do
    cat $key.t$i | sort | awk {'print $2'} > $key.ts$i
    split -l 30 -a3 -d $key.ts$i $key.ts$i.p
    [[ -f $key.ts$i.p000 ]] || touch $key.ts$i.p000
    cat $key.ts$i | wc -l
  done
)

find ./E* -mtime +1 -delete
# find ./public/res/E* -mtime -1 -delete
