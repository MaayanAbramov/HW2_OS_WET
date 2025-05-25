#!/usr/bin/env bash

FILENAME="submission_hw2_$(date -Iminutes).zip"
zip -u ../$FILENAME submitters.txt kernel/hw2.c kernel/Makefile include/linux/* arch/x86/entry/syscalls/syscall_64.tbl 
