#!/usr/bin/env bash
set -x
make
gcc -o fork fork.c
./fork &
insmod merf_module.ko target_pid="$!"
sleep 1
rmmod merf_module
