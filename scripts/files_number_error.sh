#!/usr/bin/env bash
./file_system_demo -c filesystem > /dev/null

./file_system_demo -f filesystem -a textfiles/t10000.txt -o 1t > /dev/null
./file_system_demo -f filesystem -a textfiles/t10000.txt -o 2t > /dev/null
./file_system_demo -f filesystem -a textfiles/t10000.txt -o 3t > /dev/null
./file_system_demo -f filesystem -a textfiles/t10000.txt -o 4t > /dev/null
./file_system_demo -f filesystem -a textfiles/t10000.txt -o 5t > /dev/null
./file_system_demo -f filesystem -a textfiles/t10000.txt -o 6t > /dev/null
./file_system_demo -f filesystem -a textfiles/t10000.txt -o 7t > /dev/null
./file_system_demo -f filesystem -a textfiles/t10000.txt -o 8t > /dev/null

./file_system_demo -lr filesystem