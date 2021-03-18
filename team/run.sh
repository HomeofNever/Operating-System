#!/bin/sh

rm -f output*.txt
rm -f simout*.txt

python3 project.py 1 2 0.01 256 4 0.5 128 > output02.txt
mv simout.txt simout02.txt

python3 project.py 2 2 0.01 256 4 0.5 128 > output03.txt
mv simout.txt simout03.txt

python3 project.py 16 2 0.01 256 4 0.75 64 > output04.txt
mv simout.txt simout04.txt

python3 project.py 8 64 0.001 4096 4 0.5 2048 > output05.txt
mv simout.txt simout05.txt
