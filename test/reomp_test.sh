#!/bin/bash

mode=$1
nth=$2
loops=$3
LD_PRELOAD=../src/llvm/.libs/libreomprr.so
REOMP_MODE=$mode srun -n 1 ./reomp_test_units $nth $loops omp_reduction
