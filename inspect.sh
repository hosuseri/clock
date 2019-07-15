#!/bin/sh
trap "echo; date +'%Y-%m-%d %H:%M:%S'" 2 15
date +'%Y-%m-%d %H:%M:%S'
./clock.exe
