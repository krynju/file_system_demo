#!/usr/bin/env bash

./file_system_demo -c filesystem >/dev/null

./file_system_demo -f filesystem -a textfiles/t25.txt -o file.txt


./file_system_demo -l filesystem
./file_system_demo -lr filesystem

