#!/bin/bash
FILE=./game
PROCESS=2
if [ ! -f "$FILE" ]; then
    mpicc game.c -o game
else
    mpirun -n $PROCESS ./game
fi