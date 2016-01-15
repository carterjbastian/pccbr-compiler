#! /bin/bash

./cscan < testfiles/flow_control.c
echo "NEXT TEST\n\n"
./cscan < testfiles/non_letters.c
echo "NEXT TEST\n\n"
./cscan < testfiles/strings_and_ints.c
echo "NEXT TEST\n\n"
./cscan < testfiles/pseudofile.c

