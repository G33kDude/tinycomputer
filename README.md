# TinyComputer++
A C++ implementation of [Carlos Sanchez](https://github.com/randomouscrap98)'s
TinyComputer - a virtual machine based on a custom assembly language.

http://smilebasicsource.com/tinycomputer/

Architecture
------------

* Numbers:           Signed 32 bit
* Parameters:        Signed 16 bit
* Instruction Width: 64 bit
* Opcodes:           7 bits
* Program space:     65536 instructions
* Little Endian

Memory
------

There are two memory banks, the RAM and the ROM.
* RAM: 65536 32 bit values, numbered 0-65535
* ROM: 65536 32 bit values, numbered 0-65535

RAM can be referenced using the parameter prefixes `m` and `v`, meaning
memory and video resepctively. By convention, the `v` prefix is only used when
referencing the video area of the RAM. Additionally, RAM can be referenced via
pointer using the `*` prefix.

ROM can be referenced using the parameter prefix `b`, meaning button (legacy).
ROM cannot be referenced via pointer.

Video
-----

Video is implemented as a 32x32 black and white matrix display.
The first 32 memory addresses (0-31) are reserved for video memory. These
These addresses are bitfields representing the rows of the screen from top to
bottom, where the LSB is on the left and the MSB is on the right.

Structure of an instruction
---------------------------

An instruction is 64 bits wide, with the first 16 including the opcode and
metadata for the parameters, and the other three sets of 16 bits representing
the parameters for the instruction.

For the first 16 bits the 9 most significant bits are metadata triplets, and
the 7 least significant bits are the opcode.

```
PBM PBM PBM 0123456 | 0123456789ABCDEF | 0123456789ABCDEF | 0123456789ABCDEF
Metadata    Opcode  | First Parameter  | Second Parameter | Third Parameter
```

For the metadata the least significant triplet represents the first parameter.
Bit `P` indicates whether the parameter is a pointer, bit `B` represents if it
is an address in the ROM, and bit `M` represents if it is an address in the
RAM. If both bits `B` and `M` are set, `B` takes precedence. If neither `M`
nor `B` are set, the parameter is an integer literal.

Instructions
------------

Name  | Opcode | Param Count | Param1 Type | Param2 Type | Param3 Type
----- | ------ | ----------- | ----------- | ----------- | -----------
add   | 1      | 3           | RO/RW/Num   | RO/RW/Num   | RW
sub   | 2      | 3           | RO/RW/Num   | RO/RW/Num   | RW
mul   | 3      | 3           | RO/RW/Num   | RO/RW/Num   | RW
div   | 4      | 3           | RO/RW/Num   | RO/RW/Num   | RW
mod   | 5      | 3           | RO/RW/Num   | RO/RW/Num   | RW
and   | 6      | 3           | RO/RW/Num   | RO/RW/Num   | RW
or    | 7      | 3           | RO/RW/Num   | RO/RW/Num   | RW
xor   | 8      | 3           | RO/RW/Num   | RO/RW/Num   | RW
rs    | 9      | 3           | RO/RW/Num   | RO/RW/Num   | RW
ls    | 10     | 3           | RO/RW/Num   | RO/RW/Num   | RW
not   | 11     | 2           | RO/RW/Num   | RW          |
beq   | 12     | 3           | RO/RW/Num   | RO/RW/Num   | Label
jmp   | 13     | 1           | Label       |             |
vsync | 14     | 0           |             |             |
jrt   | 15     | 1           | Label       |             |
ret   | 16     | 0           |             |             |
bgt   | 17     | 3           | RO/RW/Num   | RO/RW/Num   | Label
bne   | 18     | 3           | RO/RW/Num   | RO/RW/Num   | Label

Types are checked regardless of pointer prefix. If the prefix is specified
when it would not make sense (e.g. for a label) that is undefined behavior.
