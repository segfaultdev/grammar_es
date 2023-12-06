#!/bin/bash

gcc-13 -std=gnu2x $(find . -name "*.c") -Iinclude -o main -Og -g \
  -fsanitize=address,undefined

# gcc-13 -std=gnu2x $(find . -name "*.c") -Iinclude -o main -Ofast -s
