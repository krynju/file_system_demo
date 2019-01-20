#!/usr/bin/env bash
./file_system_demo -c filesystem > /dev/null

for i in `seq 1 130`;
    do
        ./file_system_demo -f filesystem -a textfiles/t25.txt -o "$i" > /dev/null
    done

./file_system_demo -lr filesystem