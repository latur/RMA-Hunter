#!/bin/bash
key=$1
tpx=$2

./exec/hunter $key $tpx "public/data/SDF_S2.csv" "public/res/"
cd ./public/res/

echo $(
  for i in 1 2 3; do
    cat $key.t$i | sort | awk {'print $2'} > $key.ts$i
    split --lines=30 -a3 -d $key.ts$i $key.ts$i.p
    [[ -f $key.ts$i.p000 ]] || touch $key.ts$i.p000 
    cat $key.ts$i | wc -l
  done
)
