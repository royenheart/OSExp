#!/bin/bash

echo "=====TEST select semaphore====="
for (( i=1;i < 100;i=i+1 )); do
    test=$(./select_sem | tail -n 1)
    if [[ ${test} != "SELECT PASSED!" ]]; then
        echo "select semaphore not done right!"
        break
    fi
done

if [[ $i -ge 100 ]]; then
    echo "select semaphore correct!"
fi
echo "=====TEST select semaphore====="

echo "=====TEST select mutex & cond====="
for (( i=1;i < 100;i=i+1 )); do
    test=$(./select_mutex_cond | tail -n 1)
    if [[ ${test} != "SELECT PASSED!" ]]; then
        echo "select mutex & cond not done right!"
        break
    fi
done

if [[ $i -ge 100 ]]; then
    echo "select mutex & cond correct!"
fi
echo "=====TEST select mutex & cond====="