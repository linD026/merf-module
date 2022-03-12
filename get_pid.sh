#!/usr/bin/env bash
set -x
make
gcc -o fork fork.c
./fork &
insmod mtrace_module.ko target_pid="$!"
sleep 1
rmmod mtrace_module
