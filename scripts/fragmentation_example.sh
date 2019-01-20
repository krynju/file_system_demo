#!/usr/bin/env bash

./file_system_demo -c filesystem >/dev/null

for i in `seq 1 60`;
    do
        ./file_system_demo -f filesystem -a textfiles/t1000.txt -o "$i" > /dev/null
    done

for i in `seq 1 2 60`;
    do
        ./file_system_demo -f filesystem -d "$i" > /dev/null
    done


./file_system_demo -lr filesystem

for i in `seq 100 105`
    do
        ./file_system_demo -f filesystem -a textfiles/t2500.txt -o "$i"
    done



./file_system_demo -lr filesystem

./file_system_demo -l filesystem