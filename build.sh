#!/bin/sh

mkdir -p ./bin
rm -f ./bin/assembler.out
cd ./src/assembler

g++ -g -o ../../bin/assembler.out \
main.cpp macros.cpp labels.cpp assemble.cpp lines.cpp -std=c++11

cd ../emulator

g++ -g -o ../../bin/emulator.out main.cpp -lncurses -std=c++11
