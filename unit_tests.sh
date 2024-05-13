#!/bin/bash

rm -f 1.epchip

printf "\n>> POSITIVE TEST: test print help.\n"
./vEPROM

printf "\n>> POSITIVE TEST: test create chip.\n"
./vEPROM create 256

printf "\n>> POSITIVE TEST: test load chip.\n"
./vEPROM load 1.epchip

printf "\n>> POSITIVE TEST: test write.\n"
./vEPROM write unit_test_files/A.txt
./vEPROM write unit_test_files/B.txt
./vEPROM write unit_test_files/C.txt

printf "\n>> POSITIVE TEST: test write_raw.\n"
./vEPROM write_raw 10 ZZZTOPZZZ

printf "\n>> POSITIVE TEST: test read_raw.\n"
./vEPROM read_raw 10 9

printf "\n>> POSITIVE TEST: test list.\n"
./vEPROM list

printf "\n>> POSITIVE TEST: read.\n"
./vEPROM read A.txt

printf "\n>> POSITIVE TEST: erase.\n"
./vEPROM erase

printf "\n>> NEGATIVE TEST: wrong range.\n"
./vEPROM read_raw 1000000 9

printf "\n>> NEGATIVE TEST: wrong no file on eprom.\n"
./vEPROM read nonexisting_file.txt

printf "\n>> NEGATIVE TEST: wrong local file to load.\n"
./vEPROM load 7689.epchip
