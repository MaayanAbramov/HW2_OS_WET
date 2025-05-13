#!/usr/bin/env bash
mkdir -p ../sandbox
pushd ../sandbox
gcc ../tests/test_basic.c ../tests/instrument.c -DINST -I../kernel -I../tests -o test_basic.exe
popd
