#!/bin/bash

mode=$1
nth=$2
LD_PRELOAD=../src/llvm/.libs/libreomprr.so
REOMP_MODE=$mode ./reomp_test_units $nth
