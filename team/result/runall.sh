#!/bin/sh

rm -f a.out
rm -f output*.txt
rm -f simout*.txt


gcc -Wall p1.c -lm

a.out 1 2 0.01 256 4 0.5 128 > output02-full.txt

a.out 2 2 0.01 256 4 0.5 128 > output03-full.txt

a.out 16 2 0.01 256 4 0.75 64 > output04-full.txt

a.out 8 64 0.001 4096 4 0.5 2048 > output05-full.txt

# Oh, I think gold has some magic parameter here, but not required
gcc -Wall p1.c -lm -D DISPLAY_MAX_T=1000

a.out 1 2 0.01 256 4 0.5 128 > output02.txt
mv simout.txt simout02.txt

a.out 2 2 0.01 256 4 0.5 128 > output03.txt
mv simout.txt simout03.txt

a.out 16 2 0.01 256 4 0.75 64 > output04.txt
mv simout.txt simout04.txt

a.out 8 64 0.001 4096 4 0.5 2048 > output05.txt
mv simout.txt simout05.txt
