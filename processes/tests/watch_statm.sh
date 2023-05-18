#!/bin/bash

exec=$1
tmp=`mktemp`

pid=`nohup ${exec} > ${tmp} 2>&1 & echo $!`

watch -e -n 1 """
echo "Size Resident Shared Trs Lrs Drs Dt" &&
cat /proc/${pid}/statm
"""
