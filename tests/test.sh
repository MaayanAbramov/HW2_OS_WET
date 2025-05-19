#!/usr/bin/env bash
mkdir -p ../sandbox
pushd ../sandbox
gcc ../tests/test_basic.c ../tests/instrument.c -DINST -I../kernel -I../tests -o test_basic.exe
gcc ../tests/test_set_sec.c ../tests/instrument.c -DINST -I../kernel -I../tests -o test_set_get_sec.exe
gcc ../tests/test_check_sec.c ../tests/instrument.c -DINST -I../kernel -I../tests -o test_check_sec.exe
popd
