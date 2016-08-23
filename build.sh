#!/bin/sh

mkdir -p ./bin
rm -f ./bin/assembler.out
cd ./src/assembler

g++ -g -o ../../bin/assembler.out \
main.cpp macros.cpp labels.cpp assemble.cpp lines.cpp -std=c++11
