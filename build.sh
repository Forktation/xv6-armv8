#!/bin/sh

# add the ARM cross toolchain to your path
export PATH=path_to_arm_cross_compiler_gcc/bin:$PATH

# Build Kernel
clear

echo Building xv6 for ARM 64 bit

#make -j`getconf _NPROCESSORS_ONLN`
make
