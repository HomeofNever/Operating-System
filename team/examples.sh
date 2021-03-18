#!/bin/sh

## before using this one, read printTasks() method
python3 project.py 1 2 0.01 256 4 0.5 128 > examples/1.txt
python3 project.py 2 2 0.01 256 4 0.5 128 > examples/2.txt
python3 project.py 16 2 0.01 256 4 0.75 64 > examples/3.txt
python3 project.py 8 64 0.001 4096 4 0.5 2048 > examples/4.txt
