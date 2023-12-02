#!/bin/bash

gcc-13 -std=c2x $(find . -name "*.c") -Iinclude -o main
