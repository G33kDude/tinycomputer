#!/bin/sh

rm ./samples/tetris.bin

./bin/assembler.out ./samples/tetris.bin < ./samples/tetris.tc

./bin/emulator.out ./samples/tetris.bin
