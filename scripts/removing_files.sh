#!/usr/bin/env bash

./file_system_demo -c filesystem > /dev/null

for i in `seq 1 5`;
    do
        ./file_system_demo -f filesystem -a textfiles/t25.txt -o "$i" > /dev/null
    done

echo "files added"

./file_system_demo -l filesystem

for i in `seq 1 5`;
    do
        ./file_system_demo -f filesystem -d "$i" > /dev/null
    done


echo "files removed"

./file_system_demo -l filesystem